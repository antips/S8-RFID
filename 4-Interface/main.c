#define WIN32_LEAN_AND_MEAN
#if defined(_WIN32)
#define WIN32
#endif
#if defined(_WIN64)
#define WIN64
#define _AMD64_
#undef _X86_
#else
#undef _AMD64_
//#define _X86_
#endif

#include <minwindef.h>
#include <string.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <omp.h>


//#include "NurApiConfig.h"
//#include "NurMicroApi.h"


#define SCREENWIDTH 900
#define SCREENHEIGHT 600

#define MAXCHAR 1000

#define MARGIN 10
#define ICON_SCALE 30
#define MARGIN_ICON 5

#define MAX_GLASSES 20
#define GLASS_ID_SIZE 129

#define NUM_MAX_PRINTED_CHAR 15

#define EMPTY 0
#define USED 1
#define REGISTERED 2

#define FILENAME "list_tags.txt"

//le glass -> 129 char
//Time stamp de lecture -> TODO
//RSSI -> signed int (a comfirmer)

typedef struct
{
	char id[GLASS_ID_SIZE];
	//char id[4];
	int RSSI;
	//int time_stamp;
	clock_t start_time;
	//int number;
	int list;
} Glass;

Glass NO_GLASS;

Glass emptyGlasses[MAX_GLASSES];
int numEmptyGlass = 0;

Glass usedGlasses[MAX_GLASSES];
int numUsedGlass = 0;

Glass registeredGlasses[MAX_GLASSES];
int numRegisteredGlass = 0;


Vector2 touchPosition = { 0, 0 };

void DrawGlass(/*int col, int line, */Color color, int list, int index)
{
	int col = 0;
	int line = 0;
	
	char text[GLASS_ID_SIZE];
	
	if (list == EMPTY)
	{
		col = MARGIN+MARGIN_ICON;
		line = 130+MARGIN_ICON+index*(MARGIN_ICON+ICON_SCALE);
		strcpy(text, emptyGlasses[index].id);
	}
	else if (list == USED)
	{
		col = MARGIN+MARGIN_ICON+(SCREENWIDTH/2);
		line = 130+MARGIN_ICON+index*(MARGIN_ICON+ICON_SCALE);
		strcpy(text, usedGlasses[index].id);
	}
	else	//REGISTERED
	{
		col = MARGIN+MARGIN_ICON;
		//TODO -> line
		//130+4*MARGIN_ICON+numEmptyGlass*(MARGIN_ICON+ICON_SCALE)
		line = 130+4*MARGIN_ICON+(numEmptyGlass+1)*(MARGIN_ICON+ICON_SCALE)+index*(MARGIN_ICON+ICON_SCALE);
		strcpy(text, registeredGlasses[index].id);
	}
	
	
	
	Rectangle rec = {col, line, ICON_SCALE, ICON_SCALE};
	DrawRectangleRec(rec, WHITE);
	touchPosition = GetTouchPosition(0);
	//printf("X : %i, Y : %i\n", (int)touchPosition.x, (int)touchPosition.y);
	//printf("%d %d\n", (int)touchPosition.x, (int)(touchPosition.x));
	if (CheckCollisionPointRec(touchPosition, rec))
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			printf("collision gauche, glass %i : x=%i, xmin=%i, xmax=%i\n", index, (int)touchPosition.x, col, col+ICON_SCALE);
			switch (list){
				case EMPTY :	//On bouge le verre vers plein
					addUsedGlass(emptyGlasses[index]);
					removeEmptyGlass(index);
				break;
				case USED :
					addEmptyGlass(usedGlasses[index]);
					removeUsedGlass(index);
				break;
				case REGISTERED :
					addEmptyGlass(registeredGlasses[index]);
					removeRegisteredGlass(index);
				break;
			}		
		}
		else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			printf("collision droite, glass %i : x=%i, xmin=%i, xmax=%i\n", index, (int)touchPosition.x, col, col+ICON_SCALE);
			switch (list){
				case EMPTY :
					addRegisteredGlass(emptyGlasses[index]);
					removeEmptyGlass(index);
				break;
				case USED :
					//Reset du timer ?
				break;
				case REGISTERED :
					// N/A
				break;
			}
		}
	}
	DrawText(text, col+ICON_SCALE+MARGIN_ICON, line, ICON_SCALE, BLACK);
	DrawLine(col+0.1*ICON_SCALE, line+0.1*ICON_SCALE, col+0.2*ICON_SCALE, line+0.9*ICON_SCALE, color);
	DrawLine(col+0.2*ICON_SCALE, line+0.9*ICON_SCALE, col+0.8*ICON_SCALE, line+0.9*ICON_SCALE, color);
	DrawLine(col+0.8*ICON_SCALE, line+0.9*ICON_SCALE, col+0.9*ICON_SCALE, line+0.1*ICON_SCALE, color);
	
	
}

void addEmptyGlass(Glass glass)
{
	if(numEmptyGlass == MAX_GLASSES-1)
	{
		printf("Cannot add anymore empty Glasses ...\n");
		return;
	}
	
	/*emptyGlasses[numEmptyGlass]->id = glass->id;
	emptyGlasses[numEmptyGlass]->list = glass->list;
	emptyGlasses[numEmptyGlass]->number = glass->number;
	emptyGlasses[numEmptyGlass]->RSSI = glass->RSSI;
	emptyGlasses[numEmptyGlass]->start_time = glass->start_time;
	emptyGlasses[numEmptyGlass]->time_stamp = glass->time_stamp;*/
	
	emptyGlasses[numEmptyGlass] = glass;
	
	emptyGlasses[numEmptyGlass].list = EMPTY;
	
	numEmptyGlass++;
}

void addUsedGlass(Glass glass)
{
	if(numUsedGlass == MAX_GLASSES-1)
	{
		printf("Cannot add anymore used Glasses ...\n");
		return;
	}
	
	usedGlasses[numUsedGlass] = glass;
	
	usedGlasses[numUsedGlass].list = USED;
	
	numUsedGlass++;
}

void addRegisteredGlass(Glass glass)
{
	if(numRegisteredGlass == MAX_GLASSES-1)
	{
		printf("Cannot add anymore registered Glasses ...\n");
		return;
	}
	
	registeredGlasses[numRegisteredGlass] = glass;
	
	registeredGlasses[numRegisteredGlass].list = REGISTERED;
	
	numRegisteredGlass++;
}

void removeEmptyGlass(int index)
{
	if (index > numEmptyGlass || numEmptyGlass == 0)
	{
		printf("Cannot remove this empty glass ..., index = %i\n", index);
		return;
	}
//	else
//	{
//		printf("removing index %i\n", index);
//	}
	
	for (int i = index; i < numEmptyGlass-1; i++)
	{
		//printf("Putting %i into %i !\n", i+1, i);
		emptyGlasses[i] = emptyGlasses[i+1];
	}
	
	//TODO -> S'occuper du dernier verre
	emptyGlasses[numEmptyGlass-1] = NO_GLASS;

	numEmptyGlass--;
}

void removeUsedGlass(int index)
{
	if (index > numUsedGlass || numUsedGlass == 0)
	{
		printf("Cannot remove this used glass ...\n");
		return;
	}
	
	for (int i = index; i < numUsedGlass-1; i++)
	{
		usedGlasses[i] = usedGlasses[i+1];
	}
	
	//TODO -> S'occuper du dernier verre
	usedGlasses[numUsedGlass-1] = NO_GLASS;
	
	numUsedGlass--;
}

void removeRegisteredGlass(int index)
{
	if (index > numRegisteredGlass || numRegisteredGlass == 0)
	{
		printf("Cannot remove this registered glass ...\n");
		return;
	}
	
	for (int i = index; i < numRegisteredGlass-1; i++)
	{
		registeredGlasses[i] = registeredGlasses[i+1];
	}
	
	//TODO -> S'occuper du dernier verre
	registeredGlasses[numRegisteredGlass-1] = NO_GLASS;
	
	numRegisteredGlass--;
}

int initGlassesIDs(char *filename)
{
    FILE *fptr;
    int count = 0;
    char c;
    int count_name_length = 0;
    char glassId[GLASS_ID_SIZE];

    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s", filename);
        return -1;
    }

    for (c = getc(fptr); c != EOF; c = getc(fptr))
    {
    	if (c == '\n')
        {
        	if (count_name_length > 0)
        	{	//On créé le tag
        		Glass newGlass;// = malloc(sizeof(Glass));
		    	strcpy(newGlass.id, glassId);
		    	//testGlass->id
		    	newGlass.list = EMPTY;
		    	newGlass.RSSI = 0;
		    	newGlass.start_time = clock();
		    	
		    	addRegisteredGlass(newGlass);
			}
			
        	//On reinitialise le nom du tag :
        	for (int i = 0; i < count_name_length; i++)
        	{
        		glassId[i] = ' ';
			}
			count_name_length = 0;
        	count++;
		}
        else
        {
        	if(count_name_length < NUM_MAX_PRINTED_CHAR)
				glassId[count_name_length] = c;
			else if (count_name_length == NUM_MAX_PRINTED_CHAR)
			{
				glassId[NUM_MAX_PRINTED_CHAR-1] = '.';
				glassId[NUM_MAX_PRINTED_CHAR-2] = '.';
				glassId[NUM_MAX_PRINTED_CHAR-3] = '.';
			}
        	count_name_length++;
		}
	}
	//On enregistre le dernier tag
	if (count_name_length > 0)
	{	//On créé le tag
		Glass newGlass;// = malloc(sizeof(Glass));
    	strcpy(newGlass.id, glassId);
    	//testGlass->id
    	newGlass.list = EMPTY;
    	newGlass.RSSI = 0;
    	newGlass.start_time = clock();
    	
    	addRegisteredGlass(newGlass);
	}
	
    fclose(fptr);

    return count;
}

int main(int argc, char *argv[]) {
	
	//Initialisation du nombre de verre :
	initGlassesIDs(FILENAME);
	
	FILE *fp;
    char row[MAXCHAR];
	
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "Projet RFID");

    SetTargetFPS(60);
    
    clock_t before = clock();
    
    //Initialisation des verres (pour le démarrage)
    
    while(!WindowShouldClose())
    {
    	//DEBUG - ajout / soustraction de verre
    
	    if(IsKeyPressed(KEY_P))
	    {
	    	char c[10];
	    	printf("Adding empty Glass !\n");
	    	Glass testGlass;// = malloc(sizeof(Glass));
	    	strcpy(testGlass.id, itoa(numEmptyGlass, c, 10));
	    	//testGlass->id
	    	testGlass.list = EMPTY;
	    	testGlass.RSSI = 0;
	    	testGlass.start_time = clock();
	    	
	    	addEmptyGlass(testGlass);
	    	
	    	//free(testGlass);
		}
	    
	    if(IsKeyPressed(KEY_O))
		{
			printf("Removing empty Glass !\n");
			removeEmptyGlass(numEmptyGlass);
		}
		
		if(IsKeyPressed(KEY_I))
	    {
	    	char c[10];
	    	printf("Adding used Glass !\n");
	    	Glass testGlass;// = malloc(sizeof(Glass));
	    	strcpy(testGlass.id, itoa(numUsedGlass, c, 10));
	    	//testGlass->id
	    	testGlass.list = USED;
	    	testGlass.RSSI = 1;
	    	testGlass.start_time = clock();
	    	
	    	addUsedGlass(testGlass);
		}
	    
	    if(IsKeyPressed(KEY_U))
		{
			printf("Removing used Glass !\n");
			removeUsedGlass(numUsedGlass);
		}
    	
    	BeginDrawing();
    	
    	//Interface de base :
    		
    	DrawText("Le verre connecté !", 10, 10, 50, BLACK);
    	
    	clock_t diff = clock()-before;
    	
    	char num[10];
    	int sec = diff/1000;
    	
    	//DrawText(itoa(sec, num, 10), 40, 40, 50, BLACK);
    	
    	//Affichage des verres vides
    	
    	DrawRectangle(MARGIN, 100, SCREENWIDTH/2-2*MARGIN, 30+MARGIN_ICON+numEmptyGlass*(ICON_SCALE+MARGIN_ICON), DARKGRAY);
    	DrawText("Verres pret a partir :", 2*MARGIN, 100+MARGIN, 20, WHITE);
    	
    	for (int i = 0; i < numEmptyGlass; i++)
    	{
    		DrawGlass(BLACK, EMPTY, i);
		}
		
		
		//Affichage des verres utilisés
		
    	DrawRectangle(MARGIN+SCREENWIDTH/2, 100, SCREENWIDTH/2-2*MARGIN, 30+MARGIN_ICON+numUsedGlass*(ICON_SCALE+MARGIN_ICON), DARKGRAY);
    	DrawText("Verres partis :", 2*MARGIN+SCREENWIDTH/2, 100+MARGIN, 20, WHITE);
    	
    	for (int i = 0; i < numUsedGlass; i++)
    	{
    		DrawGlass(BLUE, USED, i);
		}
		
		//Affichage des veres enregistrés
		
		DrawRectangle(MARGIN, 130+4*MARGIN_ICON+numEmptyGlass*(MARGIN_ICON+ICON_SCALE), SCREENWIDTH/2-2*MARGIN, 30+MARGIN_ICON+numRegisteredGlass*(ICON_SCALE+MARGIN_ICON), DARKGRAY);
		DrawText("Verres enregistrés :", 2*MARGIN, 130+MARGIN+4*MARGIN_ICON+numEmptyGlass*(MARGIN_ICON+ICON_SCALE), 20, WHITE);
    	
    	
    	for (int i = 0; i < numRegisteredGlass; i++)
    	{
    		DrawGlass(GREEN, REGISTERED, i);
		}
    	
    	ClearBackground(RAYWHITE);
    	
    	EndDrawing();
    	
	}
	
	return 0;
}