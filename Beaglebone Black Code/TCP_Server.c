/*Command line compile: gcc -o app Jordan_Project_main.c TCP_Server.c EventManager.c -lsqlite3*/

#include "TCP_Server.h"
#include "EventManager.h"
#include "SQLiteManager.h"

/**/
// get sockaddr, IPv4 or IPv6:		 get_in_addr			/////DO LIT STUDY ON THIS MAYBE
void *clientDevice_address(struct sockaddr *address)
{
 if (address->sa_family == AF_INET)
 {
 return &(((struct sockaddr_in*)address)->sin_addr);
 }
 return &(((struct sockaddr_in6*)address)->sin6_addr);
}
/**/


int newsockfd;
struct sockaddr_in serv_addr, cli_addr;
/* A server in the using TCP. The port number is passed as an argument */
void Server(int x)
{
	/*Variables for TCP network*/
	int portno;									//Port on which the server accepts connections
	int clilen;									//Value for the length of the client address, needed in order to accept system call
	int RWreturn;								//Read Write return value used to catch errors
	char buffer[1024];							//Server saves characters read from the socket, size 1024 for recievinf sql strings
	char clientIP[INET6_ADDRSTRLEN];
	//struct sockaddr_in serv_addr, cli_addr;		//Defined in #include <netinet/in.h>, holds address of server and client
	struct sockaddr_storage their_addr; 		// connector's address information
	socklen_t sin_size;
	pid_t childpid;
	/***************************/
	/***Initialize variables***/
	bzero((char *)&serv_addr, sizeof(serv_addr));				//Initialize serv_addr to 0's
	portno = x;													//Sets the port number that the server will listen for connections
	serv_addr.sin_family = AF_INET;								//Code for the address family, it should always be set to the symbolic constant AF_INET.
	serv_addr.sin_addr.s_addr = INADDR_ANY;						//IP address of the machine that the server is running on
	serv_addr.sin_port = htons(portno);							//Port number converted to Network Byte order,  sorting bytes that is independent of specific machine architecture
	clilen = sizeof(cli_addr);
	/***************************/

	int listeningSockfd = createListenerSocket();
	bindListeningSocketfd(listeningSockfd);
	listenToSocket(listeningSockfd);

	printf("Listening...\n");

	int n;
	/**/
	for ( ; ; )
	{
	  clilen = sizeof(cli_addr);
	  newsockfd = accept (listeningSockfd, (struct sockaddr *) &cli_addr, &clilen);
	  printf("Received request...\n");
	  if ( (childpid = fork ()) == 0 )
	  {
	  	close (listeningSockfd);
		memset(&buffer[0], 0, sizeof(buffer));
	  	while ( (n = recv(newsockfd, buffer, 1024,0)) > 0)
	  	{
	   		printf("%s","String received\n");
	  		puts(buffer);
	  		if(buffer[0]=='~' && buffer[1] == '`' && buffer[2]=='~')
	  		{
				exit(1);
			}
	   		memset(&buffer[0], 0, sizeof(buffer));
	  	}
	  exit(0);
	  printf("ooooooooooooooooooooooooooooooooo");
	 }
	 //close socket of the server
	 close(newsockfd);
	 printf("=====================================");
}
	/**/


	/*	char *msg = "Beej was here!";											//Writes to the client
		int len, bytes_sent;
		len = strlen(msg);
		bytes_sent = send(newsockfd, msg, len, 0);
		printf("bytes_sent = %d, Length of msg = %d\n", bytes_sent, len);
*/


}

//AF_INET - domain for processes for hosts on the internet
//SOCK_STREAM - Type of Socket streamable vs datagram sockets messages read in clumps(SOCK_DGRAM)
//0 - protocal, chooses TCP for stream sockets and UDP for datagram sockets.
//Checks less than 0 because all negative returned numbers are errors
int createListenerSocket()
{
	int socketfd;		//fd stands for file descripter, integer used to uniquily represents an opened file or socket
	int attemptCount = 0;
	do
	{	eventHandler(INFO, "Attempting to create socket listener\n");
		socketfd = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int));	//This is needed so that the assigned port can be reused without powering off the BBB
		if(socketfd > -1)
		{
			eventHandler(VERBOSE, "Socket listener created successfully\n");
			return socketfd;
		}
		attemptCount++;
	}while(socketfd<0 || attemptCount<10);

	attemptCount = 0;
	eventHandler(ERROR, "ERROR opening socket\n");
	error("ERROR on creating socket");					//will exit the system as a persistant error here will break all the code
}

void bindListeningSocketfd(int listeningfd)
{
	int bindFlag = 0;
	int attemptCount = 0;
	do
	{
		eventHandler(INFO, "Attempting to bind to listening socket\n");
		bindFlag = bind(listeningfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); //bind() binds the socket to an address ie current host and port number that the server will run,
		if(bindFlag==0)
		{
			eventHandler(VERBOSE, "Binding successfull\n");
			return;
		}
		attemptCount++;
	}while(bindFlag<0 || attemptCount<10);

	eventHandler(ERROR, "Could not on bind to socket listener");
	error("ERROR on binding");								//Fails if the socket is already in use on the machine
}

void listenToSocket(int listeningfd)
{
	int listenFlag = 0;
	int attemptCount = 0;
	do
	{
		eventHandler(INFO, "Listening to socket\n");
		listenFlag = listen(listeningfd, BACKLOG_QUEUE_SIZE);									//Listens to the socket for connections (cannot fail)
		if(listenFlag == 0)																		//Processess will go in sleep mode and await incomming connection
		{
			eventHandler(VERBOSE, "Listening Successfull\n");
			return;
		}
		attemptCount++;
	}while(listenFlag<0 || attemptCount<10);

	eventHandler(ERROR, "Listening failed\n");
	error("listening failed");
}



int error(char *msg)				//Function called when when the system fails
{
	perror(msg);
	exit(1);
}

static int callback(void *unused, int count, char **data, char **columns)
{
    int idx;

    printf("There are %d column(s)\n", count);

    for (idx = 0; idx < count; idx++) {
        printf("The data in column \"%s\" is: %s\n", columns[idx], data[idx]);
    }
    printf("\n");
}

