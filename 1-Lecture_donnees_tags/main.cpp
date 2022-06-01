#include <stdio.h>
#include <string.h>
#include <time.h>

#include "include/NurAPI.h"
#include "include/NurAPIErrors.h"

#define MAX_TERMINAL_LINES 30
#define MAX_EPC_LENGTH 25

int simpleInventory(HANDLE hapi);
int fetchInventoriedTagsData(HANDLE hapi, NUR_TAG_DATA tagsData[], int tagCount);
void displayDetectedTags(NUR_TAG_DATA tagsData[], int tagCount);

int main()
{
    HANDLE hApi = INVALID_HANDLE_VALUE;
    int error;

    // 1 - Cr‚er l'object NurAPI
    hApi = NurApiCreate();
    if (hApi == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Impossible de cr‚er un objet nurAPI\n"));
        return 1;
    }

    system("cls");

    // 2 - Activer la connexion auto … la station de base par USB
    error = NurApiSetUsbAutoConnect(hApi, TRUE);
    if (error != NUR_NO_ERROR)
    {
        _tprintf(_T("Impossible de se connecter via USB … la station de base (setUsbAutoConnect failed)\n"));
        return 1;
    }

    // 2a - Attendre que la station de base est connect‚e
    printf("En attente de connexion de la station de base...");
    while (NurApiIsConnected(hApi) != 0)
        ; // Wait for base station to be connected
    printf("Station de base connect‚e !\n");

    struct timespec ts =
        {
            .tv_sec = 0,
            .tv_nsec = 500 * 1000000,
        };

    while (1)
    {
        // 3 - Faire un inventaire des tags lisibles
        struct NUR_INVENTORY_RESPONSE invResp;

        error = simpleInventory(hApi);
        if (error != NUR_NO_ERROR)
        {
            printf("Erreur lev‚e lors de l'ex‚cution de simpleInventory()\n");
            return error;
        }

        int tagCount;
        error = NurApiGetTagCount(hApi, &tagCount);
        if (error != NUR_NO_ERROR)
        {
            printf("Erreur lev‚e lors de l'ex‚cution de NurApiGetTagCount()\n");
            return error;
        }

        struct NUR_TAG_DATA tagsData[tagCount];
        error = fetchInventoriedTagsData(hApi, tagsData, tagCount);
        if (error != NUR_NO_ERROR)
        {
            printf("Erreur lev‚e lors de l'ex‚cution de fetchInventoriedTagsData()\n");
            return error;
        }

        // TODO 4 - Filtrer : garder seulement les tags enregistr‚s dans la whitelist

        // 5 - Afficher le r‚sultat
        displayDetectedTagsInConsole(tagsData, tagCount);
        // updateDetectedTagsInFile(tagsData, tagCount);

        nanosleep(&ts, &ts);
    }

    return 0;
}

int simpleInventory(HANDLE hApi)
{
    int error;
    struct NUR_INVENTORY_RESPONSE invResp;

    error = NurApiClearTags(hApi);
    if (error != NUR_NO_ERROR)
    {
        printf("Erreur lev‚e lors de l'ex‚cution de NurApiClearTags()\n");
        return error;
    }

    for (int i = 0; i < 5; i++)
    {
        error = NurApiSimpleInventory(hApi, &invResp);
        if (error != NUR_NO_ERROR && error != NUR_ERROR_NO_TAG)
        {
            printf("Erreur lev‚e lors de l'ex‚cution de NurApiSimpleInventory()\n");
            return error;
        }
    }

    error = NurApiFetchTags(hApi, TRUE, NULL);
    if (error != NUR_NO_ERROR && error != NUR_ERROR_NO_TAG)
    {
        printf("Erreur lev‚e lors de l'ex‚cution de NurApiFetchTags()\n");
        return error;
    }

    return NUR_NO_ERROR;
}

int fetchInventoriedTagsData(HANDLE hapi, NUR_TAG_DATA tagsData[], int tagCount)
{
    for (int i = 0; i < tagCount; i++)
    {
        int error;
        struct NUR_TAG_DATA tagData;

        error = NurApiGetTagData(hapi, i, &tagData);
        if (error != NUR_NO_ERROR)
        {
            char errorMsg[128 + 1];
            NurApiGetErrorMessage(error, errorMsg, 128);
            printf("Erreur lors de la lecture du tag nø%d : %s", i + 1, errorMsg);
            return error;
        }

        tagsData[i] = tagData;
    }

    return NUR_NO_ERROR;
}

void EpcToString(const BYTE *epc, DWORD epcLen, char *epcStr)
{
    DWORD n;
    int pos = 0;
    for (n = 0; n < epcLen; n++)
    {
        pos += _stprintf_s(&epcStr[pos], MAX_EPC_LENGTH - pos, _T("%02x"), epc[n]);
    }
    epcStr[pos] = 0;
}

void displayDetectedTagsInConsole(NUR_TAG_DATA tagsData[], int tagCount)
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
        int epcLen = tagsData[i].epcLen > MAX_EPC_LENGTH ? MAX_EPC_LENGTH : tagsData[i].epcLen;
        EpcToString(tagsData[i].epc, epcLen, epcStr);
        if (tagsData[i].epcLen > MAX_EPC_LENGTH)
            epcStr[MAX_EPC_LENGTH - 1] = '?';

        printf("Tag %d: %s, RSSI = %d, antennaId = %d\n\r", i + 1, epcStr, tagsData[i].rssi, tagsData[i].antennaId);
        lineCounter++;
    }
}