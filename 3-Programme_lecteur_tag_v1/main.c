#include <stdio.h>
#include <time.h>

#include "include/NurAPI.h"

#include "fileController.h"
#include "NurAPIApi.h"
#include "config.h"

#define IS_TEST_WITHOUT_ANTENNA 1

void displayDetectedTagsInConsole(struct NUR_TAG_DATA tagsData[], int tagCount);
void updateDetectedTagsInFile(struct NUR_TAG_DATA tagsData[], int tagCount, const char *fileName);

#if IS_TEST_WITHOUT_ANTENNA
void stringToEpc(BYTE epc[NUR_MAX_EPC_LENGTH], int epcLen, const char *epcStr)
{
    DWORD n;

    for (n = 0; n < epcLen * 2; n += 2)
    {
        char substr[2];
        strncpy(substr, &(epcStr[n]), 2);
        BYTE byte;
        if (substr[0] == '0')
        {
            byte = (BYTE)strtol(&(substr[1]), NULL, 16);
        }
        else
        {
            byte = (BYTE)strtol(substr, NULL, 16);
        }
        // printf("%c%c => %d\n", epcStr[n], epcStr[n + 1], byte);
        epc[n / 2] = byte;
    }

    if (epcLen % 2 != 0)
    {
        BYTE byte = (BYTE)(epcStr[n] - '0');
        printf("%c => %d\n", epcStr[n], byte);
        epc[n] = byte;
    }

    epc[n] = 0;
}

// Functions created in order to save lines of code and avoid errors in duplicate code
void createTagData(struct NUR_TAG_DATA *tagData, const BYTE *epcStr, signed char rssi, BYTE antennaId)
{
    tagData->epcLen = strlen(epcStr) / 2 + strlen(epcStr) % 2;
    tagData->epcLen = tagData->epcLen > MAX_EPC_LENGTH ? MAX_EPC_LENGTH : tagData->epcLen;
    stringToEpc(tagData->epc, tagData->epcLen, epcStr);
    tagData->rssi = rssi;
    tagData->antennaId = antennaId;
}
#endif

int main()
{
    HANDLE hApi = INVALID_HANDLE_VALUE;
    int error;

#pragma region create_nurApi_object
    hApi = NurApiCreate();
    if (hApi == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Impossible de créer un objet nurAPI\n"));
        return 1;
    }
#pragma endregion create_nurApi_object

    system("cls");

#if IS_TEST_WITHOUT_ANTENNA == 0
#pragma region enable_auto_connect_usb
    error = NurApiSetUsbAutoConnect(hApi, TRUE);
    if (error != NUR_NO_ERROR)
    {
        _tprintf(_T("Impossible de se connecter via USB à la station de base (setUsbAutoConnect failed)\n"));
        return 1;
    }
#pragma endregion

#pragma region wait_for_usb_connection
    printf("En attente de connexion de la station de base...");
    while (NurApiIsConnected(hApi) != 0)
        ; // Wait for base station to be connected
    printf("Station de base connectée !\n");
#pragma endregion
#endif

    struct timespec ts = {.tv_sec = 0, .tv_nsec = 500 * 1000000};
    while (1)
    {
#if IS_TEST_WITHOUT_ANTENNA
        int tagCount = 3;
        struct NUR_TAG_DATA tagsData[tagCount];
        createTagData(&(tagsData[0]), "ad2360f7bbcc000ad23604c7", -57, 1);
        createTagData(&(tagsData[1]), "ad2360g7bbcc000ad23604c8", -58, 1);
        createTagData(&(tagsData[2]), "c00004691744353412000000", -50, 1);
#else
#pragma region make_inventory
        struct NUR_INVENTORY_RESPONSE invResp;

        error = simpleInventory(hApi);
        if (error != NUR_NO_ERROR)
        {
            printf("Erreur levée lors de l'exécution de simpleInventory()\n");
            return error;
        }

        int tagCount;
        error = NurApiGetTagCount(hApi, &tagCount);
        if (error != NUR_NO_ERROR)
        {
            printf("Erreur levée lors de l'exécution de NurApiGetTagCount()\n");
            return error;
        }

        struct NUR_TAG_DATA tagsData[tagCount];
        error = fetchInventoriedTagsData(hApi, tagsData, tagCount);
        if (error != NUR_NO_ERROR)
        {
            printf("Erreur levée lors de l'exécution de fetchInventoriedTagsData()\n");
            return error;
        }
#pragma endregion

#pragma region filter_detected_tags_with_whitelist
        // TODO 4 - Filtrer : garder seulement les tags enregistrés dans la whitelist

#pragma endregion
#endif

#pragma region update_results
        displayDetectedTagsInConsole(tagsData, tagCount);
        updateDetectedTagsInFile(tagsData, tagCount, DETECTED_TAGS_FILE);
#pragma endregion

        nanosleep(&ts, &ts);
    }

    return 0;
}

void displayDetectedTagsInConsole(struct NUR_TAG_DATA tagsData[], int tagCount)
{
    static int lineCounter = 0;

    int numberOfTagsToDisplay = tagCount > MAX_TERMINAL_LINES ? MAX_TERMINAL_LINES : tagCount;

    if (lineCounter == 0)
    {
        printf("Displaying the detected tags :\n");
        lineCounter++;
    }

    while (lineCounter > 1)
    {
        printf("\33[2K\r\033[A\33[2K\r"); // "\33[2K\r" deletes the content of the current line, "\033[A" puts the cursor one line up
        lineCounter--;
    }

    for (int i = 0; i < numberOfTagsToDisplay; i++)
    {
        char epcStr[MAX_EPC_LENGTH];
        getEpcStrOfTagData(tagsData[i], epcStr);

        printf("Tag %d: %s, RSSI = %d, antennaId = %d\n\r", i + 1, epcStr, tagsData[i].rssi, tagsData[i].antennaId);
        lineCounter++;
    }
}

void updateDetectedTagsInFile(struct NUR_TAG_DATA tagsData[], int tagCount, const char *fileName)
{
    char tagsEPCs[tagCount][MAX_EPC_LENGTH];
    for (int i = 0; i < tagCount; i++)
        getEpcStrOfTagData(tagsData[i], tagsEPCs[tagCount]);

    writeAllLinesInFile(fileName, tagsEPCs, tagCount);
}