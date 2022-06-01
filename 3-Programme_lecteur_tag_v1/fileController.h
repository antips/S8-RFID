#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include "config.h"

int getNumberOfLinesInFile(const char *filename);
int readAllLinesOfFile(const char *fileName, char outputBuffer[][LINE_MAX_LENGTH]);
int writeAllLinesInFile(const char *fileName, char stringBuffer[][LINE_MAX_LENGTH], int tagCount);

#endif