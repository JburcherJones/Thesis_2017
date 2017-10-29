/*Command line compile: gcc -o app Jordan_Project_main.c TCP_Server.c SQLiteManager.c EventManager.c ClientManager.c -lsqlite3*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sqlite3.h>

//#include "TCP_Server.h"
#include "EventManager.h"
#include "SQLiteManager.h"
#include "ClientManager.h"


int main(void)
{
	Logger_fp = fopen("EventLog.txt", "a+"); //will have a value of NULL if something went wrong
	if(Logger_fp != NULL)
	{
	    time_t rawtime;
	    time (&rawtime);
  		fprintf(Logger_fp,"%s",ctime(&rawtime));
  		fclose(Logger_fp);
	}
	else
	{
		printf("Error opening file");
	}
	eventHandler(INFO,"Main method started\n");

	connectToDB();
	createMainTable();
	createDeviceTable();
	//createDeviceLocationTable();


	client();												//sets up the client connection

	if(clientListen() == 0)
	{
		eventHandler(ERROR,"Did not reply to server properly\n");
	}

	int isayso = 0;

		if(clientListen() == 5)									//ak received means that the bbb can be sent data now
		{
			do
			{
				eventHandler(INFO,"Valid comms, waiting for commands from server\n");
				clientListen();
				isayso++;
				printf("%d\n", isayso);
			}
			while(isayso<15);
		}

	sqlite3_close(db);
	eventHandler(INFO,"sqlite3_close(db)\n");
	eventHandler(INFO,"Main method ended\n\n\n");

	return 0;
}