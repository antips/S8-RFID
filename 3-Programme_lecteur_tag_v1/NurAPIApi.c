#include "NurAPIApi.h"
#include "config.h"

#include <string.h>
#include <stdio.h>

int simpleInventory(HANDLE hApi)
{
    int error;
    struct NUR_INVENTORY_RESPONSE invResp;

    error = NurApiClearTags(hApi);
    if (error != NUR_NO_ERROR)
    {
        printf("Erreur levée lors de l'exécution de NurApiClearTags()\n");
        return error;
    }

    for (int i = 0; i < 5; i++)
    {
        error = NurApiSimpleInventory(hApi, &invResp);
        if (error != NUR_NO_ERROR && error != NUR_ERROR_NO_TAG)
        {
            printf("Erreur levée lors de l'exécution de NurApiSimpleInventory()\n");
            return error;
        }
    }

    error = NurApiFetchTags(hApi, TRUE, NULL);
    if (error != NUR_NO_ERROR && error != NUR_ERROR_NO_TAG)
    {
        printf("Erreur levée lors de l'exécution de NurApiFetchTags()\n");
        return error;
    }

    return NUR_NO_ERROR;
}

int fetchInventoriedTagsData(HANDLE hapi, struct NUR_TAG_DATA tagsData[], int tagCount)
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
            printf("Erreur lors de la lecture du tag n°%d : %s", i + 1, errorMsg);
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

void getEpcStrOfTagData(struct NUR_TAG_DATA tagData, char *output)
{
    char epcStr[MAX_EPC_LENGTH];
    int epcLen = tagData.epcLen > MAX_EPC_LENGTH ? MAX_EPC_LENGTH : tagData.epcLen;

    EpcToString(tagData.epc, epcLen, epcStr);
    if (tagData.epcLen > MAX_EPC_LENGTH)
        epcStr[MAX_EPC_LENGTH - 1] = '?';

    strcpy(output, epcStr);
}