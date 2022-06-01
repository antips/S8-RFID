#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_LENGTH 129

int main()
{
    const int numStrings = 3;
    char textData[][MAX_STRING_LENGTH] = {"Test1", "Test-2", "Test 3"};

    printf("Les donn‚es de test :\n");
    for (int i = 0; i < numStrings; i++)
        printf("\t%s\n", textData[i]);

    char tagsNames[numStrings][MAX_STRING_LENGTH];
    // for (int i = 0; i < numStrings; i++)
    //     tagsNames[i] = (char *)malloc(MAX_STRING_LENGTH * sizeof(char));

    for (int i = 0; i < numStrings; i++)
        strcpy(tagsNames[i], textData[i]);

    printf("Les donn‚es enregistr‚es :\n");
    for (int i = 0; i < numStrings; i++)
        printf("\t%s\n", tagsNames[i]);
}