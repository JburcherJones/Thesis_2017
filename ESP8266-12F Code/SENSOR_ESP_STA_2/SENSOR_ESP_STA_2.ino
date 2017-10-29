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
//const char* myssid = "CKSAP1"; //SSID AP MODE
//const char* mypassword = "22CkfH5032e!"; //Password AP MODE

WiFiClient client;
bool alreadyConnected = false;
const char* host = "192.168.4.1";
//const char* host = "192.168.121.137";
int index_TCP = 0;
char data_TCP[1500];
bool validMsgTCP = false;
bool msgCompleteTCP = false;

//*************************UART related variables**********************************//
bool uartRecived = false, validMsg = false, msgComplete = false;
char inChar;
int index_UART = 0;
char data_UART[1500];
//*************************************************************************************//

//*************************Protocal related variables**********************************//
char data_FINAL[2000];
char size_DATA[4];
char mac_ADDRESS[18];
char endChar[2];
//*************************************************************************************//

void setup()
{
  //turn on the LED when the setup begins
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  Serial.begin(57600);  //the baud rate that the loras work with
  Serial.setTimeout(2000); //default is 1000

  WiFi.mode(WIFI_STA); //set up the wifi as station

  WiFi.begin(myssid, mypassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
    delay(100);
  }
  delay(1000); //dont need this i think
}

void loop()
{
  //**************heartbeat of the program*********************************//
  //  digitalWrite(5, HIGH);
  //  delay(100);
  //  digitalWrite(5, LOW);
  //  delay(100);
  //  digitalWrite(5, HIGH);
  //  delay(100);
  //  digitalWrite(5, LOW);
  //  delay(1000);
  //**************heartbeat of the program*********************************//

  while (alreadyConnected == false)
  {
    if (client.connect(host, 8888) == 1)
    {
      alreadyConnected = true;
    }
  }
  if (!client.connected())//try to see if this actually works
  {
    alreadyConnected = true;
  }

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
      digitalWrite(5, HIGH);
      delay(2000);
      digitalWrite(5, LOW);
      delay(100);
      //do something with the message
      msgCompleteTCP = false;
    }
    memset(data_TCP, 0, sizeof(data_TCP));
    index_TCP = 0;
  }
  //*******************************Recieve tcp from server*******************************//
  //*********************************Wait for uart messages*****************************************//
  while (Serial.available() > 0)                            // Don't read unless
  {
    if (index_UART < 1498)                                  // One less than the size of the array, needed so as to not try and over fill the array due to its given size
    {
      inChar = Serial.read();                               // Read a character
      if (inChar == 0x02)                                  //siginals the start of the message
      {
        validMsg = true;
      }
      else if (inChar == 0x04)
      {
        msgComplete = true;
        sprintf(size_DATA, "%03d", index_UART);
      }
      else if (validMsg == true)
      {
        data_UART[index_UART] = inChar;
        index_UART++;
      }
      if (msgComplete == true)
      {
        validMsg = false;
      }
    }
  }
  //***********************************************************************************************//
  //*************************Do something with the uart message************************************//
  if (msgComplete == true)
  {

    data_FINAL[0] = 0x02;
    endChar[0] = 0x04;
    endChar[1] = '\0';
    WiFi.macAddress().toCharArray(mac_ADDRESS, 18);
    strcat(data_FINAL, mac_ADDRESS);
    strcat(data_FINAL, size_DATA);
    strcat(data_FINAL, data_UART);
    strcat(data_FINAL, endChar);
    //Serial.print(data_FINAL);
    client.print(data_FINAL);  //sends the uart information over tcp connection
    memset(data_FINAL, 0, sizeof(data_FINAL));
    msgComplete = false;
    index_UART = 0;
    memset(data_UART, 0, sizeof(data_UART));      //clear the uart data array
  }

  //**********************************************************************************************//

}

//***********************************Class Functios***********************************//

//*************************************************************************************//
