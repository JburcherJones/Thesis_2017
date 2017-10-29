//#include "TCP_Server.h"
#include "EventManager.h"
#include "SQLiteManager.h"



void eventHandler(int eventType, char *eventMsg)
{
	if(eventType<=DEBUG)
	{
		char fileEntry[255] = "";
		char messageHeader[10] = "";
		switch (eventType)
		{
			case ERROR:
				strcpy(messageHeader, "ERROR:   ");
				break;

		    case WARNING:
		    	strcpy(messageHeader, "WARNING: ");
				break;

			case VERBOSE:
				strcpy(messageHeader, "VERBOSE: ");
				break;

			case INFO:
				strcpy(messageHeader, "INFO:    ");
				break;

		    default:
				strcpy(messageHeader, "UNKNOWN: ");
		}
		strcat(fileEntry, messageHeader);
		strcat(fileEntry, eventMsg);
		Logger_fp = fopen("EventLog.txt", "a+");
		fprintf(Logger_fp, fileEntry);
		fclose(Logger_fp);
	}
}