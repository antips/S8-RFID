#include "fileController.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"

int getNumberOfLinesInFile(const char *filename)
{
    FILE *fptr;
    int count = 0;
    char c;

    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s en mode lecture\n", filename);
        return -1;
    }

    for (c = getc(fptr); c != EOF; c = getc(fptr))
        if (c == '\n')
            count++;

    fclose(fptr);

    return count;
}

int readAllLinesOfFile(const char *fileName, char outputBuffer[][LINE_MAX_LENGTH])
{
    FILE *fptr;

    fptr = fopen(fileName, "r");
    if (fptr == NULL)
    {
        printf("Erreur : impossible d'ouvrir le fichier %s en mode lecture\n", fileName);
        return -1;
    }

    int i = 0;
    char line[LINE_MAX_LENGTH];
    while (fgets(line, LINE_MAX_LENGTH, fptr))
    {
        strcpy(outputBuffer[i], line);
        i++;
    }

    fclose(fptr);

    return 0;
}

int writeAllLinesInFile(const char *fileName, char stringBuffer[][LINE_MAX_LENGTH], int tagCount)
{
    FILE *fptr;

    fptr = fopen(fileName, "w");
    if (fptr == NULL)
    {
        printf("Erreur : impossible d'ouvrir le fichier %s en mode écriture\n", fileName);
        return -1;
    }

    for (int i = 0; i < tagCount; i++)
        fprintf(fptr, "%s\n", stringBuffer[i]);

    fclose(fptr);

    return 0;
}

int updateFile(const char *fileName, char EPCsStr[][MAX_EPC_LENGTH], int tagCount)
{
    if (tagCount == 0)
        return 0;

    char delimiter[] = ",";
    int error;
    int numberOfLinesInFile = getNumberOfLinesInFile(fileName);

    char fileLines[numberOfLinesInFile][LINE_MAX_LENGTH];
    error = readAllLinesOfFile(fileName, fileLines);
    if (error != 0)
    {
        printf("[UpdateFile] Erreur lors de la lecture du fichier %s\n", fileName);
        return -1;
    }

    int newTags[tagCount];
    for (int i = 0; i < tagCount; i++)
        newTags[i] = 1;

    int numberOfTagsEverDetected = numberOfLinesInFile + tagCount;

    for (int i = 0; i < numberOfLinesInFile; i++)
    {
        char *epcStrOfLine = strtok(fileLines[i], delimiter);

        for (int k = 0; k < tagCount; k++)
            if (strcmp(epcStrOfLine, EPCsStr[k]) == 0)
            {
                newTags[k] = 0;
                numberOfTagsEverDetected--;
            }
    }

    char outputLines[numberOfTagsEverDetected][LINE_MAX_LENGTH];
    int line;

    for (line = 0; line < numberOfLinesInFile; line++)
    {
        char *epcStrOfLine = strtok(fileLines[line], delimiter);

        int tagWasAlreadyDetected = 0;
        for (int k = 0; k < tagCount; k++)
        {
            if (strcmp(epcStrOfLine, EPCsStr[k]) == 0)
            {
                tagWasAlreadyDetected = 1;
                break;
            }
        }

        if (tagWasAlreadyDetected)
            sprintf(outputLines[line], "%s,%d", epcStrOfLine, (int)time(NULL));
        else
            strcpy(outputLines[line], fileLines[line]);
    }

    for (int i = 0; i < tagCount; i++)
    {
        if (newTags[i] == 1)
        {
            sprintf(outputLines[line++], "%s,%d", EPCsStr[i], (int)time(NULL));
        }
    }
    writeAllLinesInFile(fileName, outputLines, tagCount);

    return 0;
}