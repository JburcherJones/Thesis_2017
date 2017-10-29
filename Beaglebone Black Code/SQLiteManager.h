#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sqlite3.h>

#define MAX_ENTRIES_PER_PACKET 4

sqlite3 *db;

char *sql;
char queryBuf[50];
int rc;

/*char macAdrs[18] = {'\0'};
char sizedata[4] = {'\0'};
char dataTimeStamp[13] = {'\0'};
char dataStatus[5] = {'\0'};*/

char strarr[4][18];
char devTable[3][18];
char mainTable[4][13];
char mainTableID_UPDATE[1][5];
char ID_LIST[100];
char SQLREQ[1000];

void connectToDB();
void createMainTable();
void createDeviceTable();
void createDeviceLocationTable();

void pushSQLite(char macAdrs[], char size_data[], char data_TimeStamp[], char data_Status[]);
int macID_search(char* mac_ADDRESS);
void add_MacAddres(char* mac);

void handleSQLREQ();
void packageREQ();
void updateSQlite();