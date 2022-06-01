#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EPC_MAX_LENGTH 129

int getNumberOfLinesInFile(char *filename)
{
    FILE *fptr;
    int count = 0;
    char c;

    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s", filename);
        return -1;
    }

    for (c = getc(fptr); c != EOF; c = getc(fptr))
        if (c == '\n')
            count = count + 1;

    fclose(fptr);

    return count;
}

int getAllRegisteredTagsEPCs(char *fileName, char tagsNames[][EPC_MAX_LENGTH])
{
    FILE *fptr;

    fptr = fopen(fileName, "r");
    if (fptr == NULL)
    {
        printf("Erreur : impossible d'ouvrir le fichier %s", fileName);
        return -1;
    }

    printf("Les noms lus dans le fichier :\n");

    int i = 0;
    char line[EPC_MAX_LENGTH];
    while (fgets(line, EPC_MAX_LENGTH, fptr))
    {
        strcpy(tagsNames[i], line);
        tagsNames[i][strcspn(tagsNames[i], "\n")] = 0;
        printf("\tLigne %d du fichier : %s\n", i, tagsNames[i++]);
    }

    fclose(fptr);

    return 0;
}

int main()
{
    // R‚cup‚rer les noms des tags sous forme de liste de strings
    char fileName[] = "EPC_list.txt";
    const int numTagsToRead = getNumberOfLinesInFile(fileName);
    char tagsNames[numTagsToRead][EPC_MAX_LENGTH];

    int error = getAllRegisteredTagsEPCs(fileName, tagsNames);
    if (error)
        return error;

    printf("\nLes tags enregistr‚s :\n");
    for (int i = 0; i < sizeof(tagsNames) / sizeof(tagsNames[0]); i++)
        printf("\t%s\n", tagsNames[i]);

    return 0;
}