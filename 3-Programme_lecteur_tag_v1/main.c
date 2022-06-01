#include <stdio.h>
#include <time.h>

#include "include/NurAPI.h"

#include "fileController.h"
#include "NurAPIApi.h"
#include "config.h"

// void filterDetectedTagsWithRegisteredTags(struct NUR_TAG_DATA detectedTagsData[], char registeredTags[][MAX_EPC_LENGTH]);
void displayDetectedTagsInConsole(struct NUR_TAG_DATA tagsData[], int tagCount);
void updateDetectedTagsInFile(struct NUR_TAG_DATA tagsData[], int tagCount, const char *fileName);

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

    struct timespec ts = {.tv_sec = 0, .tv_nsec = 500 * 1000000};
    while (1)
    {
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
        printf("\33[2K\r\033[A"); // "\33[2K\r" deletes the content of the current line, "\033[A" puts the cursor one line up
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