/************************************************************************************/
//ESP wifi in access point mode, looks to receive connection from an ESP station
//Controls the LORA device, sends data from tcp over LORA connnection
//Primes the LORA to recieve from the other side which there is no tcp string to transmit
//more stuff to be put here when dealing with the CP and the bbb sqlite stories
//All LEDs are onl used for debugging purposes and visual entertainment
/************************************************************************************/


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define MAX_NO_CLIENTS 5           //number of sensors in a given house

//***********************************Class Variables***********************************//
//***************************WIFI related variables************************************//
//needed to set up the wifi connection as  server
//const char* myssid = "JordanAP_TEST"; //SSID AP MODE
//const char* mypassword = "123456789"; //Password AP MODE
const char* myssid = "LORARECEIVER"; //SSID AP MODE
const char* mypassword = "123456789"; //Password AP MODE

WiFiServer server(8888);
WiFiClient *clients[MAX_NO_CLIENTS] = { NULL };
bool alreadyConnected = false; //used to see if there are any wifi connections
int index_TCP = 0;
char data_TCP[1500];

bool validMsg = false;
bool msgComplete = false;

char askClientID[3] = {0x02, 0x05, 0x04};
char readyToReceive[3] = {0x02, 0x06, 0x04};
char reqSQL[3] = {0x02, 0x52, 0x04};
char NAK_SQL_REQ[4] = {0x02, 0x52, 0x15, 0x04};
int ID[3] = {0};

bool bbbConnected = false;
bool cpConnected = false;
int bbbLocation = -1;           //has no location
int cpLocation = -1;


//****************************LoRa related variables***********************************//
String str; //LORA reply saved in in this variable
String strHold;
bool loraListen = false;

int index_LORA = 0;
char data_LORA[1500];
//*************************************************************************************//

void setup()
{
  //turn on the LED when the setup begins
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  delay(1000);

  Serial.begin(57600);  //the baud rate that the loras work with
  //Serial.setTimeout(2000); //default is 1000
  Serial.setTimeout(500); //default is 1000     debuging

  WiFi.mode(WIFI_AP); //set up the wifi as an access point
  WiFi.softAP(myssid, mypassword);
  //Serial.println(WiFi.softAPIP());
  server.begin(); // start the server

  pinMode(4, OUTPUT);
  setupLoRa();
  digitalWrite(5, LOW); // trn off LED when setup is complete
  digitalWrite(2, HIGH);
}

void loop()
{
  //***************************Connect to a client if you can*************************//
  WiFiClient newClient = server.available();
  if (newClient)
  {
    for (int i = 0 ; i < MAX_NO_CLIENTS ; ++i)             // Find the first unused space
    {
      if (NULL == clients[i])                   //use NULL== instead of ==NULL to avoid silly assignment if u forget double =
      {
        clients[i] = new WiFiClient(newClient);
        clients[i]->print(askClientID);
        break;
      }
    }
  }
  //***************************Connect to a client if you can*************************//

  //*********************Check to see if a client has disconnected*********************//

  for (int i = 0 ; i < MAX_NO_CLIENTS ; ++i)
  {
    if (NULL != clients[i])
    {
      int connectionStatus = clients[i]->connected();
      if (0 == connectionStatus)
      {
        clients[i]->stop();     //deletes the client
        delete clients[i];
        clients[i] = NULL;
        ID[i] = 0;
        if (bbbLocation == i)
        {
          bbbLocation = -1;
          bbbConnected = false;

        }
        if (cpLocation == i)
        {
          cpLocation = -1;
          cpConnected = false;
        }
      }
    }
  }

  //*********************Check to see if a client has disconnected*********************//

  //*****************************Read tcp if it is there******************************//

  for (int i = 0 ; i < MAX_NO_CLIENTS ; ++i)
  {
    if ((NULL != clients[i]) && (clients[i]->available() ))           // If the client is in use, and has some data...
    {
      char aChar = clients[i]->read();
      if (aChar == 0x02)
      {
        validMsg = true;
      }
      else if (aChar == 0x04)
      {
        msgComplete = true;
      }

      if (validMsg == true)
      {
        data_TCP[index_TCP] = aChar;
        index_TCP++;
        data_TCP[index_TCP] = '\0';
      }
      if (msgComplete == true)
      {
        validMsg = false;
      }

      if (msgComplete == true)//test hex values at later stage
      {
        clients[i]->flush();
        digitalWrite(5, HIGH);
        delay(200);
        digitalWrite(5, LOW);
        delay(100);

        if ((data_TCP[1] == 0x11) && (data_TCP[2] == 0x31))
        {
          ID[i] = 1;
          clients[i]->print(readyToReceive);
        }
        else if ((data_TCP[1] == 0x12) && (data_TCP[2] == 0x31))
        {
          ID[i] = 2;
          bbbLocation = i;
          clients[i]->print(readyToReceive);
        }
        else if ((data_TCP[1] == 0x13) && (data_TCP[2] == 0x31))
        {
          ID[i] = 3;
          cpLocation = i;
          clients[i]->print(readyToReceive);
        }

        handleTCP(data_TCP, ID[i], i);
        
        msgComplete = false;
        memset(data_TCP, 0, sizeof(data_TCP));
        index_TCP = 0;
      }
    }
  }
  //*****************************Read tcp if it is there******************************//
  receiveLoRa();      //by deafult a listening device
}

//***********************************Class Functios***********************************//
void setupLoRa()
{
  // reset the module-synchronise serial communications
  digitalWrite(4, LOW);
  delay(500);
  digitalWrite(4, HIGH);
  delay(2000);                          //investigae the shortest time you can have here

  str = Serial.readStringUntil('\n');
  Serial.println("radio set wdt 105000");
  str = Serial.readStringUntil('\n');
  Serial.println("radio set pwr 14");
  str = Serial.readStringUntil('\n');
  Serial.println("mac pause");
  str = Serial.readStringUntil('\n');
}

void transmitLoRa(char tcp[])
{
  char  msg[1000] = {0};
  char  data[1000] = {0};
  char awe[] = "radio tx ";

  Serial.println("sys reset");            //reset lora because it is listening by default
  str = Serial.readStringUntil('\n');
  Serial.println("radio set wdt 105000");
  str = Serial.readStringUntil('\n');
  Serial.println("radio set pwr 14");
  str = Serial.readStringUntil('\n');
  Serial.println("mac pause");
  str = Serial.readStringUntil('\n');
  loraListen = false;

  for (int i = 0; i < strlen(tcp); i++)
  {
    sprintf(data, "%s%02X", data, tcp[i]);
  }
  strcat(msg, awe);
  strcat(msg, data);
  Serial.println(msg);
  str = Serial.readStringUntil('\n');
}

void receiveLoRa()//in time make this return a value
{
  if (!loraListen)
  {
    loraListen = true;
    Serial.println("radio rx 0");
    str = Serial.readStringUntil('\n');
  }
  else
  {
    if (Serial.available() > 0)
    {
      str = Serial.readStringUntil('\r');
      strHold = str;
      strHold.remove(8);

      if (str == "radio_err")
      {
        loraListen = false;
      }
      else if (str == "ok")
      {
        //do nothing
      }
      else if (strHold == "radio_rx")
      {
        str.remove(0, 10);
        long loraMsgHex[1000] = {0};
        char buf[1000];
        char hexArr[1000];
        str.toCharArray(buf, 1000);
        char hexNum[3];
        char *ptr;
        long ret;

        int count = 0;
        /**/
        for (int i = 0; i < str.length(); i += 2)
        {
          hexNum[0] = buf[i];
          hexNum[1] = buf[i + 1];
          ret = strtol(hexNum, &ptr, 16);
          loraMsgHex[count] = ret;

          hexArr[count] = (char)loraMsgHex[count];
          count++;
          hexArr[count] = '\0';
        }
        if (true == bbbConnected)
        {
          clients[bbbLocation]->print(hexArr);
        }
        else
        {
          //reply to cp that there is no bbb connected
        }
        loraListen = false;
        memset(hexArr, 0, sizeof(hexArr));
      }
    }
  }
}

void  handleTCP(char tcp[], int ID, int i)
{
  switch (ID)
  {
    case 1:                                                                             //I am a sensor
      if (tcp[1] == 0x44)                                                               //we have data from the sensor of known format
      {
        if (false == bbbConnected)                                                      //if there is no bbb connected then obviously we need to attempt to transmit the data over lora
        {
          transmitLoRa(tcp);
        }
        else                                                                            //if we have a bbb on the network, then we can give the data to it over tcp connection
        {
          clients[bbbLocation]->print(tcp);
        }
      }
      break;



    case 2:                                                                              //I am a BBB
      bbbConnected = true;
      if (tcp[1] == 0x53)
      {
        clients[cpLocation]->print(tcp);
      }
      //more conversations stuff will go in here
      break;




    case 3:                                                                             //I am a CP
      cpConnected = true;
      if ((tcp[1] == 0x52) && (bbbLocation > -1))                                                             //app req latest sql
      {
        clients[bbbLocation]->print(tcp);                                               //pass req on to bbb
      }
      else if(bbbLocation < 0)
      {
        clients[cpLocation]->print(NAK_SQL_REQ);                                         //else the Cp that there is no bbb connected
      }
      else if(tcp[1] == 0x06)                                     //cp ak the sql data
      {
        clients[bbbLocation]->print(tcp); 
      }
      //more conversations stuff will go in here
      break;




    default:
      //unknown reply from client
      break;
  }
}

//*************************************************************************************//
