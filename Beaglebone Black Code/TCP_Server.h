#include <stdio.h>					//Typically included in all c files, necessary for all input an output commands
#include <sys/types.h>				//Contains definitions for data types regarding system calls
#include <sys/socket.h>				//Neccessary to use sockets
#include <netinet/in.h>				//Holds constants neccessary for internet domain addresses
#include <string.h>					//Neccessary for manipulating arrays of characters
#include <stdlib.h>					//Neccessary to perform general functions
#include <sqlite3.h>				//Needed to use functions pertaining the transient internal database of the BBB

static const int BACKLOG_QUEUE_SIZE = 5;	//The number of connections that can be waiting while a process is handling a particular connection
static const int true = 1;						//just to ensure that these values are correct in default code
static const int false = 0;

void Server(int x);
int createListenerSocket();
void bindListeningSocketfd(int listeningfd);
void listenToSocket(int listeningfd);

int error(char *msg);			//Function called when when the system fails
int sqlStateMachine(char *buffer);
static int callback(void *unused, int count, char **data, char **columns);