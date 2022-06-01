#include <stdio.h>
#include <string.h>

#define LINE_MAX_LENGTH 25

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

int main()
{
    char test[][LINE_MAX_LENGTH] = {
        "e200001963020281201048f9",
        "e2806894000050042ea62095",
        "ad2360f7bbcc000ad23604c7",
    };

    writeAllLinesInFile("testOut.txt", test, 3);

    return 0;
}
