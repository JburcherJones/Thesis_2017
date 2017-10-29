//#include "TCP_Server.h"
#include "EventManager.h"
#include "SQLiteManager.h"
#include "ClientManager.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
 }
 return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int sockfd, numbytes;

 	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char myID[5] = {0x02, 0x12, 0x31, 0x04};
	char packageDone[5] = {0x02, 0x53, 0x17, 0x04};

	int len;
	int bytes_sent;

	int vMsg = 0;

int client()
{
	eventHandler(INFO,"client() started\n");


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo("192.168.4.1", PORT, &hints, &servinfo)) != 0)
	{
		eventHandler(ERROR,"client() getaddrinfo\n");
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
 	}

 	for(p = servinfo; p != NULL; p = p->ai_next)
 	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			eventHandler(ERROR,"client() client socket\n");
	 	 	perror("client: socket");
	 	 	continue;
 	 	}
	 	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
	 	{
			close(sockfd);
		 	eventHandler(ERROR,"client() client connect\n");
		 	perror("client: connect");
		 	continue;
		 }
		 break;
	}

	if (p == NULL)
	{
		eventHandler(ERROR,"client() failed to connect\n");
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	//close(sockfd);//dont need this, we dont want to close the connection
	eventHandler(INFO,"client() ended\n");

	return 0;
}
///
//returns 1 if client connected successfully
int clientListen()
{
	char buf[MAXDATASIZE];
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
	{
		eventHandler(ERROR,"clientListen() Receiving\n");
	}

	buf[numbytes] = '\0';
	//printf("~~~buf~~~%X%X%X\n", buf[0],buf[1],buf[2]);

	if((buf[0] == 0x02)&&(buf[numbytes-1] == 0x04))
	{
		vMsg = 1;
	}

	if(vMsg == 1)
	{
		if(buf[1] == 0x05)
		{
			len = strlen(myID);
			bytes_sent = send(sockfd, myID, len, 0);
			if(bytes_sent != len)
			{
				eventHandler(ERROR,"clientListen() ERROR sending bytes \n");
				vMsg = 0;
				return 0;														//tell main loop something went wrong
			}
			vMsg = 0;
			return 1;
		}
		else if(buf[1] == 0x06)				//This ak from server
		{
			printf("got AK\n\n");
			vMsg = 0;
			return 5;
		}
		else if(buf[1] == 0x44)
		{
			eventHandler(INFO,"clientListen() data received\n");
			handleMsg(buf);
			vMsg = 0;
			return 1;
		}
		else if(buf[1] == 0x52)
		{
			eventHandler(INFO,"clientListen() SQL REQ\n");
			handleSQLREQ();
			vMsg = 0;
			return 1;
		}
		else
		{
			printf("NOT a valid message\n");
			vMsg = 0;
			return 0;
		}
		vMsg = 0;
	}
	vMsg = 0;
	return 0;
}

void handleMsg(char msg[])
{
	char mac_Adrs[18];
	int i;
	for(i=0; i<17; i++)
	{
		mac_Adrs[i] = msg[i+2];
	}
	mac_Adrs[17] = '\0';

	char size_data[4];
	for(i=0; i<3; i++)
	{
		size_data[i] = msg[i+19];
	}
	size_data[3] = '\0';

	char data_TimeStamp[13];
	for(i=0; i<12; i++)
	{
		data_TimeStamp[i] = msg[i+22];
	}
	data_TimeStamp[12] = '\0';

	char data_Status[5];
	for(i=0; i<4; i++)
	{
		data_Status[i] = msg[i+34];
	}
	data_Status[4] = '\0';

	pushSQLite(mac_Adrs, size_data, data_TimeStamp, data_Status);		//this should be handle sql, another ufnction to push the right data in is needed
}

void sendPackage(char package[10][150], int numPackets)
{
	//send first line of package
	//wait for ak from cp through server
	//update sql table with true in HAS_SENT
	//send the next line and repeat ak stuffs
	//ak returns 5 in client listen
	eventHandler(INFO,"sendPackage() started \n");

	char packetToSend[200];

	char endChar[2];
	endChar[0]= 0x04;
	endChar[1]= '\0';


	int i=0;

	/***************************send one line**************************/
	SendFunc:memset(&packetToSend, 0, sizeof(packetToSend));
	if(i<numPackets)
	{
		packetToSend[0] = 0x02;
		packetToSend[1] = 0x53;
		strcat(packetToSend, package[i]);
		strcat(packetToSend, endChar);

		len = strlen(packetToSend);
		bytes_sent = send(sockfd, packetToSend, len, 0);
		if(bytes_sent != len)
		{
			eventHandler(ERROR,"sendPackage() ERROR sending bytes \n");
		}
		/***************************send one line**************************/
		i++;
		if(clientListen() == 5)
		{
			printf("packet at [%d] successfully sent\n", (i-1));
			updateSQlite();

			if(i<numPackets)
			{
				goto SendFunc;
			}
			else
			{
				len = strlen(packetToSend);
				bytes_sent = send(sockfd, packageDone, len, 0);
				if(bytes_sent != len)
				{
					eventHandler(ERROR,"sendPackage() ERROR sending bytes \n");
				}
			}
		}
	}

	eventHandler(INFO,"sendPackage() ended \n");
}

