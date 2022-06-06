#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/NurAPI.h"

#define LINE_MAX_LENGTH 25
#define MAX_EPC_LENGTH 25

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
        printf("%c%c => %d\n", epcStr[n], epcStr[n + 1], byte);
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

void testEpcGeneratorFromString()
{
    struct NUR_TAG_DATA tagData;
    char epcTest[] = "ad2360f7bbcc000ad23604c7";
    tagData.epcLen = 12;
    int epcLen = tagData.epcLen > MAX_EPC_LENGTH ? MAX_EPC_LENGTH : tagData.epcLen;
    stringToEpc(tagData.epc, epcLen, epcTest);
    for (int i = 0; i < tagData.epcLen; i++)
        printf("tagData.epc[%d] = %d => %x\n", i * 2, tagData.epc[i], tagData.epc[i]);

    char epcStr[epcLen];
    EpcToString(tagData.epc, epcLen, epcStr);
    printf("%s\n", epcStr);
}

void strtokManipulations()
{
    // 1 - Get epcstr and print result
    char data[] = "ad2360f7bbcc000ad23604c7,1654093393";
    char delimiter[] = ",";

    char *epcStr;
    epcStr = strtok(data, delimiter);

    printf("epcstr : %s\n", epcStr);

    free(epcStr);

    // 2 - get index of ','
    char *delimiterAddr;
    int index;

    delimiterAddr = strchr(data, delimiter[0]);
    index = (int)(delimiterAddr - data);
    printf("',' index : %d", index);
}

int main()
{
    testEpcGeneratorFromString();

    return 0;
}