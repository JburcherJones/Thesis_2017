/************************************************************************************/
//ESP wifi in station mode, looks to connect to an ESP access point
//Takes uart messages and relays them over a tcp connection with the access point
//A start character of 0x02 signifies the start of a message
//An end character of 0x04 signifies the end of the message
//When the Esp sends the sensor the command 0xA1, the sensor replies with 0xA2 and then the data(still to be confirmed)
/************************************************************************************/


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//***********************************Class Variables***********************************//
//*************************WIFI related variables**********************************//
const char* myssid = "JordanAP_TEST"; //SSID AP MODE
const char* mypassword = "123456789"; //Password AP MODE

WiFiClient client;
bool alreadyConnected = false;
const char* host = "192.168.4.1";
int index_TCP = 0;
char data_TCP[1500];
bool validMsgTCP = false;
bool msgCompleteTCP = false;
char myID[] = {0x02, 0x11, 0x31, 0x04};
bool serverReadyForData = false;

//*************************UART related variables**********************************//
bool uartRecived = false, validMsg = false, msgComplete = false;
char inChar;
int index_UART = 0;
char data_UART[1500];
//*************************************************************************************//

//*************************Protocal related variables**********************************//
char data_FINAL[2000];
char data_TEST[2000];
char size_DATA[4];
char mac_ADDRESS[18];
char endChar[2];
//*************************************************************************************//

void setup()
{
  Serial.begin(57600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(myssid, mypassword);
  delay(5);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5);
  }
  Serial.println("started");

  while (alreadyConnected == false)
  {
    delay(5);
    if (client.connect(host, 8888) == 1)
    {
      alreadyConnected = true;
    }
  }
  if (!client.connected())
  {
    alreadyConnected = false;
  }
  Serial.println("connected");
  /**/
  data_TEST[0] = 0x02;
  data_TEST[1] = 0x44;
  int i;
  for (i = 2; i < 18; i++)
  {
    data_TEST[i] = '5';
    data_TEST[i + 1] = 0x04;
    data_TEST[i + 2] = '\0';
  }
  Serial.println("data generated");
  delay(100);
  /**/
  //*******************************Recieve tcp from server*******************************//
  if (client.available() > 0)
  {
    char aChar;
    while (client.available())
    {
      aChar = client.read();
      if (aChar == 0x02)
      {
        validMsgTCP = true;
      }
      else if (aChar == 0x04)
      {
        Serial.println("received something");
        msgCompleteTCP = true;
      }
      if (validMsgTCP == true)
      {
        data_TCP[index_TCP] = aChar;
        index_TCP++;
        data_TCP[index_TCP] = '\0';
      }
      if (msgCompleteTCP == true)
      {
        validMsgTCP = false;
      }
    }
    client.flush();
    if (msgCompleteTCP == true)//test hex values at later stage
    {
      handleTCP(data_TCP);
      msgCompleteTCP = false;
    }
    memset(data_TCP, 0, sizeof(data_TCP));
    index_TCP = 0;
    delay(5);
  }
  //*******************************Recieve tcp from server*******************************//
  //*********************************Wait for uart messages*****************************************//
//  while (Serial.available() > 0)                            // Don't read unless
//  {
//    if (index_UART < 1498)                                  // One less than the size of the array, needed so as to not try and over fill the array due to its given size
//    {
//      inChar = Serial.read();                               // Read a character
//      if (inChar == 0x02)                                  //siginals the start of the message
//      {
//        validMsg = true;
//      }
//      else if (inChar == 0x04)
//      {
//        msgComplete = true;
//        sprintf(size_DATA, "%03d", index_UART);
//      }
//      else if (validMsg == true)
//      {
//        data_UART[index_UART] = inChar;
//        index_UART++;
//      }
//      if (msgComplete == true)
//      {
//        validMsg = false;
//      }
//    }
//  }
  //***********************************************************************************************//
  //*************************Do something with the uart message************************************//
//  if (msgComplete == true)
//  {
//    data_FINAL[0] = 0x02;
//    data_FINAL[1] = 0x44;
//    endChar[0] = 0x04;
//    endChar[1] = '\0';
//    WiFi.macAddress().toCharArray(mac_ADDRESS, 18);
//    strcat(data_FINAL, mac_ADDRESS);
//    strcat(data_FINAL, size_DATA);
//    strcat(data_FINAL, data_UART);
//    strcat(data_FINAL, endChar);
//    if (true == serverReadyForData)
//    {
//      client.print(data_FINAL);  //sends the uart information over tcp connection
//      serverReadyForData = false;
//      memset(data_FINAL, 0, sizeof(data_FINAL));
//      msgComplete = false;
//      index_UART = 0;
//      memset(data_UART, 0, sizeof(data_UART));
//    }
//    else
//    {
//      Serial.println("ERROR: Server not ready to receive data");
//    }
//  }
  client.print(data_TEST);
  ESP.deepSleep(20 * 1000000);
  //**********************************************************************************************//
  /**/
}
void loop()
{
}
//***********************************Class Functios***********************************//

void handleTCP(char tcp[])
{
  switch (tcp[1])
  {
    case 0x05:                                            //server asking what i am
      client.print(myID);
      break;
    case 0x06:                                            //server is ready for me to send some data
      serverReadyForData = true;
      break;
    default:
      Serial.println("ERROR: Unknown response from server");
      break;
  }
}

//*************************************************************************************//
