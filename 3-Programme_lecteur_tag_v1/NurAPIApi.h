#ifndef NURAPIAPI_H
#define NURAPIAPI_H

#include "include/NurAPI.h"
#include "include/NurAPIErrors.h"

int simpleInventory(HANDLE hapi);
int fetchInventoriedTagsData(HANDLE hapi, struct NUR_TAG_DATA tagsData[], int tagCount);
void getEpcStrOfTagData(struct NUR_TAG_DATA tagData, char *output);

#endif