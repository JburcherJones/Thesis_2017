#include <string.h>					//Neccessary for manipulating arrays of characters
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

#define ERROR 0
#define WARNING 1
#define VERBOSE 2
#define INFO 3
#define DEBUG 3	//choose level of information that you want to write to the file

FILE *Logger_fp;

void eventHandler(int eventType, char *eventMsg);
void fileManager(char *eventMsgg);
