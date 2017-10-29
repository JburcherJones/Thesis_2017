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
  Serial.setTimeout(500); //default is 1000     debuging

  //WiFi.mode(WIFI_AP); //set up the wifi as an access point
  //WiFi.softAP(myssid, mypassword);
  //server.begin(); // start the server

  pinMode(4, OUTPUT);
  setupLoRa();
  digitalWrite(5, LOW); // trn off LED when setup is complete
  digitalWrite(2, HIGH);

}

void loop()
{
  receiveLoRa();
}

//***********************************Class Functios***********************************//
void setupLoRa()
{
  // reset the module-synchronise serial communications
  digitalWrite(4, LOW);
  delay(500);
  digitalWrite(4, HIGH);
  delay(2000);                                      //investigae the shortest time you can have here

  str = Serial.readStringUntil('\n');
  Serial.println("radio set wdt 0");
  str = Serial.readStringUntil('\n');
  Serial.println("mac pause");
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
        loraListen = false;
      }
    }
  }
}
