#include "fileController.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            count = count + 1;

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
        outputBuffer[i][strcspn(outputBuffer[i], "\n")] = 0;
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
        printf("Erreur : impossible d'ouvrir le fichier %s en mode ‚criture\n", fileName);
        return -1;
    }

    for (int i = 0; i < tagCount; i++)
        fprintf(fptr, "%s\n", stringBuffer[i]);

    fclose(fptr);

    return 0;
}