#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sqlite3.h>

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT "8888" // the port client will be connecting to
#define MAXDATASIZE 1000 // max number of bytes we can get at once




int client();

int clientListen();

void handleMsg(char msg[]);

void sendPackage(char package[10][150], int numPackets);

