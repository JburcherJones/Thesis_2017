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

//***********************************Class Variables***********************************//
//***************************WIFI related variables************************************//
//needed to set up the wifi connection as  server
const char* myssid = "LORARECEIVER"; //SSID AP MODE
const char* mypassword = "123456789"; //Password AP MODE
WiFiServer server(8888);
WiFiClient client;
bool alreadyConnected = false; //used to see if there are any wifi connections
int index_TCP = 0;
char data_TCP[1500];

bool validMsg = false;
bool msgComplete = false;

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
  delay(1000);

  Serial.begin(57600);  //the baud rate that the loras work with
  Serial.setTimeout(2000); //default is 1000

  WiFi.mode(WIFI_AP); //set up the wifi as an access point
  WiFi.softAP(myssid, mypassword);
  //Serial.println(WiFi.softAPIP());
  server.begin(); // start the server

  setupLoRa();
  digitalWrite(2, HIGH); // trn off LED when setup is complete

}

void loop()
{
  //**************heartbeat of the wifi connection*********************************//
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(1000);
  //**************************heartbeat of the wifi connection************************//
  //***************************Connect to a client if you can*************************//
  if (!client.connected())
  {
    if (alreadyConnected == true)
    {
      //Serial.println("\nClient Disconected"); //Previous client must have been disconnected
      alreadyConnected = false;                 //must flag to look for a connection
    }
    client = server.available(); //try to connect to a new client
  }
  else
  {
    if (!alreadyConnected)
    {
      //Serial.println("Client Connected"); //Must have a new client now
      //client.print("Connected ");
      int count = 0;
      do
      {
        digitalWrite(5, HIGH);
        delay(100);
        digitalWrite(5, LOW);
        delay(100);
        count++;
      } while (count < 10);
      alreadyConnected = true;
    }
    //***************************Connect to a client if you can*************************//
    //*****************************Read tcp if it is there******************************//
    if (client.available() > 0)
    {
      char aChar;
      while (client.available())
      {
        aChar = client.read();
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
      }
      client.flush();
      if (msgComplete == true)//test hex values at later stage
      {
        digitalWrite(5, HIGH);
        delay(2000);
        digitalWrite(5, LOW);
        delay(100);
        //transmitLoRa(data_TCP);
        msgComplete = false;
      }
      memset(data_TCP, 0, sizeof(data_TCP));
      index_TCP = 0;
    }
    //*****************************Read tcp if it is there******************************//
  }
  receiveLoRa();
}

//***********************************Class Functios***********************************//
void setupLoRa()
{
  // reset the module-synchronise serial communications
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  delay(500);
  digitalWrite(4, HIGH);
  delay(2000);

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

  for (int i = 0; i < strlen(tcp); i++)
  {
    sprintf(data, "%s%02X", data, tcp[i]);
  }

  sprintf(msg, "%s%s", awe, data);
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
        
        //Serial.println((char)loraMsgHex[count]);
        hexArr[count] = (char)loraMsgHex[count];
        count++;
        hexArr[count] = '\0';
      }
      /**/
      //Serial.print(hexArr);
      client.print(hexArr);            //should be sent over tcp
      loraListen = false;
      memset(hexArr, 0, sizeof(hexArr));
    }
  }
}

//*************************************************************************************//
