//#include "TCP_Server.h"
#include "EventManager.h"
#include "SQLiteManager.h"
#include "ClientManager.h"


char *zErrMsg = 0;

/*static int callback(void *data, int argc, char **argv, char **azColName)
{
   int i;
   fprintf(stderr, "%s", (const char*)data);

   for(i = 0; i<argc; i++)
   {
      sprintf(queryBuf, "%s\n", argv[i] ? argv[i] : "NULL");
   }
   return 0;
}*/

static int callback(void *data, int argc, char **argv, char **azColName)
{
   int i;
   fprintf(stderr, "%s", (const char*)data);

   for(i = 0; i<argc; i++)
   {
      sprintf(strarr[i],"%s", argv[i] ? argv[i] : "NULL");
   }

   return 0;
}

static int callback_macID(void *data, int argc, char **argv, char **azColName)
{
   sprintf(devTable[0],"%s", argv[0] ? argv[0] : "NULL");
   sprintf(devTable[1],"%s", argv[1] ? argv[1] : "NULL");
   sprintf(devTable[2],"%s", argv[2] ? argv[2] : "NULL");


   return 0;
}

static int callback_MainTable(void *data, int argc, char **argv, char **azColName)
{
	eventHandler(INFO,"callback_MainTable() started\n");
	char comma[] = ",";
	char colon[] = ":";

	sprintf(mainTable[0],"%s", argv[0] ? argv[0] : "NULL");
	strcat(SQLREQ, mainTable[0]);
	strcat(SQLREQ, colon);
	sprintf(mainTable[1],"%s", argv[1] ? argv[1] : "NULL");
	strcat(SQLREQ, mainTable[1]);
	strcat(SQLREQ, colon);
	sprintf(mainTable[2],"%s", argv[2] ? argv[2] : "NULL");
	strcat(SQLREQ, mainTable[2]);
	strcat(SQLREQ, colon);
	sprintf(mainTable[3],"%s", argv[3] ? argv[3] : "NULL");
	strcat(SQLREQ, mainTable[3]);
	strcat(SQLREQ, comma);

	eventHandler(INFO,"callback_MainTable() ended\n");
	return 0;
}

static int callback_UpdateMainTable(void *data, int argc, char **argv, char **azColName)
{
	eventHandler(INFO,"callback_UpdateMainTable() started\n");
	char colon[] = ":";

	sprintf(mainTableID_UPDATE[0],"%s", argv[0] ? argv[0] : "NULL");
	strcat(ID_LIST, mainTableID_UPDATE[0]);


	eventHandler(INFO,"callback_UpdateMainTable() ended\n");
	return 0;
}

void connectToDB()
{
	eventHandler(INFO,"connectToDB() started\n");

	   rc = sqlite3_open("test.db", &db);

	   if( rc )
	   {
	      eventHandler(ERROR,"connectToDB() Can't open database\n");
	      exit(1);
	   }
	   else
	   {
	      eventHandler(VERBOSE,"connectToDB() Opened database successfully\n");
	   }
   eventHandler(INFO,"connectToDB() ended\n");
}

void createMainTable()
{
	eventHandler(INFO,"createMainTable() started\n");
	sql = "CREATE TABLE MainTable("\
		  "ID INTEGER PRIMARY KEY AUTOINCREMENT,"\
		  "DEVICE INT NOT NULL,"\
		  "DATA_TIME CHAR(20) NOT NULL,"\
		  "DATA_STATE CHAR(4) NOT NULL,"\
		  "HAS_SENT INT NOT NULL);";
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);	//Executes the sql command
	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"createMainTable() SQL error:\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(VERBOSE,"createMainTable() Table created successfully\n");
		fprintf(stdout, "Table created successfully\n");
	}

	eventHandler(INFO,"createMainTable() ended\n");
}

void createDeviceTable()
{
	eventHandler(INFO,"createDeviceTable() started\n");

	sql = "CREATE TABLE DeviceTable("\
		  "ID INTEGER PRIMARY KEY AUTOINCREMENT,"\
		  "MacADDRESS CHAR(18) NOT NULL,"\
		  "LOCATION INT NULL);";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);	//Executes the sql command
	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"createDeviceTable() SQL error:\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(VERBOSE,"createDeviceTable() Table created successfully\n");
		fprintf(stdout, "Table created successfully\n");
	}


	eventHandler(INFO,"createDeviceTable() ended\n");
}

void createDeviceLocationTable()
{
	eventHandler(INFO,"createDeviceLocationTable() started\n");

	sql = "CREATE TABLE LocationTable("\
		  "ID INTEGER PRIMARY KEY AUTOINCREMENT,"\
		  "COUNTRY CHAR(20) NOT NULL,"\
		  "PROVINCE_STATE CHAR(20) NOT NULL,"\
		  "TOWN CHAR(20) NOT NULL,"\
		  "STREET CHAR(20) NOT NULL,"\
		  "HOUSE_NUMBER CHAR(5) NOT NULL);";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);	//Executes the sql command
	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"createDeviceLocationTable() SQL error:\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(VERBOSE,"createDeviceLocationTable() Table created successfully\n");
		fprintf(stdout, "Table created successfully\n");
	}

	eventHandler(INFO,"createDeviceLocationTable() ended\n");
}

void pushSQLite(char* macAdrs, char* sizedata, char* dataTimeStamp, char* dataStatus)
{
	eventHandler(INFO,"pushSQLite() started\n");

	//we want to put the data in the main table
	//first have to look and see what the mac id is for the main table
	//if there is no mac id then we need to create that in the Dev table


	int numID = macID_search(macAdrs);
	if(numID == 0)
	{
		numID = macID_search(macAdrs);
	}
	char SQLMAIN[150] = {'\0'};

	char *sql1 = "INSERT INTO MainTable(DEVICE,DATA_TIME,DATA_STATE,HAS_SENT) "  \
		  		 "VALUES(";
	char thing[3];
	sprintf(thing, "%d", numID);
	char *sql2 = ",'";
	char *sql3 = "','";
	char *sql4 = "',0);";

	strcat(SQLMAIN, sql1);
	strcat(SQLMAIN, thing);
	strcat(SQLMAIN, sql2);
	strcat(SQLMAIN, dataTimeStamp);
	strcat(SQLMAIN, sql3);
	strcat(SQLMAIN, dataStatus);
	strcat(SQLMAIN, sql4);

	rc = sqlite3_exec(db, SQLMAIN, callback, 0, &zErrMsg);

	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"pushSQLite() ERROR\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(VERBOSE,"pushSQLite() added to main table successfully\n");
	}

	eventHandler(INFO,"pushSQLite() ended\n");
}

int macID_search(char* mac_ADDRESS)
{
	devTable[0][0] = '\0';

	char SQLDevTable[100] = {'\0'};
	char* sql1 = "SELECT * from DeviceTable WHERE MacADDRESS = '";
	char* sql2 = "';";

	strcat(SQLDevTable, sql1);
	strcat(SQLDevTable, mac_ADDRESS);
	strcat(SQLDevTable, sql2);

	rc = sqlite3_exec(db, SQLDevTable, callback_macID, 0, &zErrMsg);

	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"macID_search() error\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(VERBOSE,"macID_search() macID_search successful\n");
	}

	if(devTable[0][0] == '\0')				/*this means that the address does not exist and will thus be added to the table*/
	{
		add_MacAddres(mac_ADDRESS);
		eventHandler(INFO,"macID_search() ADDED mac address\n");
		return 0;
	}

	eventHandler(INFO,"macID_search() returned mac ID\n");
	return atoi(devTable[0]);
}

void add_MacAddres(char* mac)
{
	char SQLADD[100] = {'\0'};

	char *sql1 = "INSERT INTO DeviceTable(MacADDRESS) "  \
		  		 "VALUES('";
	char *sql2 = "'";
	//char *sql3 = "1";
	char *sql4 = ");";

	strcat(SQLADD, sql1);
	strcat(SQLADD, mac);
	strcat(SQLADD, sql2);
	//strcat(SQLADD, sql3);
	strcat(SQLADD, sql4);

	rc = sqlite3_exec(db, SQLADD, callback, 0, &zErrMsg);

	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"add_MacAddres() error\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(VERBOSE,"add_MacAddres() added successful\n");
	}
}

void handleSQLREQ()
{
	eventHandler(INFO,"handleSQLREQ() started\n");
	memset(&SQLREQ, 0, sizeof(SQLREQ));

	const char* data = "";
	char *sql1 = "SELECT ID,DEVICE,DATA_TIME,DATA_STATE from MainTable WHERE HAS_SENT = 0;";
	//char *sql1 = "SELECT ID,DEVICE,DATA_TIME,DATA_STATE from MainTable WHERE HAS_SENT = 1;";

	rc = sqlite3_exec(db, sql1, callback_MainTable, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"handleSQLREQ() error\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(INFO,"handleSQLREQ() Operation done successfully\n");
	}

	//break sqlreq into packets?
	packageREQ();
	eventHandler(INFO,"handleSQLREQ() ended\n");
}

void packageREQ()
{
	eventHandler(INFO,"packageREQ() started\n");
	//make the packages array and then send it
	//make the next package and then send it untill there are no more packages and then send end of package char
	//we have to cut up SQLREQ array
	char packet[150];
	char packets[10][150];										//first parameter can be quite large depeinging how much data is to be sent at any 1 time

	memset(&packet, 0, sizeof(packet));
	memset(&packets, 0, sizeof(packets));

	int i=0;//counting commas
	int j=0;//counting position in sqlreq
	int k = 0;//counting position in packets... ie the number of packets to be sent
	int z = 0;//counting the position in packet

	while(j<strlen(SQLREQ))												//the amount of lines to be sent from the db at a time
	{
		packet[z] = SQLREQ[j];

		if(SQLREQ[j] == ',')
		{
			i++;
		}
		if((i==MAX_ENTRIES_PER_PACKET)||(j == (strlen(SQLREQ)-1)))
		{
			sprintf(packets[k],"%s", packet);
			memset(&packet, 0, sizeof(packet));
			k++;
			i=0;
			z=-1;
		}
		z++;
		j++;
	}
	sendPackage(packets, k);
	eventHandler(INFO,"packageREQ() ended\n");
}

void updateSQlite()
{
	//need to update the db that we have successfully sent a number of packets
	//can only update what we know we have a ak for
	eventHandler(INFO,"updateSQlite() started\n");

	memset(&mainTableID_UPDATE, 0, sizeof(mainTableID_UPDATE));
	memset(&ID_LIST, 0, sizeof(ID_LIST));


	const char* data = "";
	char *sql1 = "SELECT ID from MainTable WHERE HAS_SENT = 0 LIMIT 1;";

	rc = sqlite3_exec(db, sql1, callback_UpdateMainTable, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"updateSQlite() error\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(INFO,"updateSQlite() Operation done successfully\n");
	}

	printf("we are here\n");
	char *sql2 = "UPDATE MainTable SET HAS_SENT = 1 WHERE ID >= ";
	//char *sql2 = "UPDATE MainTable SET HAS_SENT = 0 WHERE ID >= ";
	char *sql3 = ID_LIST;
	char *sql4 = " AND ID <= ";
	char sql5[10];
	sprintf(sql5, "%d", (atoi(ID_LIST) + MAX_ENTRIES_PER_PACKET - 1));
	char *sql6 = ";";

	char SQLUPDATE[100];
	memset(&SQLUPDATE, 0, sizeof(SQLUPDATE));

	strcat(SQLUPDATE, sql2);
	strcat(SQLUPDATE, sql3);
	strcat(SQLUPDATE, sql4);
	strcat(SQLUPDATE, sql5);
	strcat(SQLUPDATE, sql6);

	rc = sqlite3_exec(db, SQLUPDATE, callback_UpdateMainTable, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK )
	{
		eventHandler(ERROR,"updateSQlite() error updating\n");
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		eventHandler(INFO,"updateSQlite() Operation done successfully\n");
	}

	eventHandler(INFO,"updateSQlite() ended\n");

}


