/*
  Using UDOpenLora_ArduinoShield for reading temperature and humdimity.
  
  Turns on an buzzer on for half past second when humdimity exceeds over 60%, then off.
  
  Most UDOpenLora_ArduinoShield have an on-board Buzzer you can control. On the Uno, 
  it is attached to digital pin 6. If you're unsure what
  pin the on-board buzzer is connected to on your UDOpenLora_ArduinoShield model, check
  the documentation at https://github.com/udopenlora

  This example code is in the public domain.

  modified 25 January 2019
  by Dao Minh Canh & Nguyen Ta Quang
 */
#include <UDOpenLora.h>
#include "DHT.h"

#define DHTPIN 3    
#define DHTTYPE DHT11   
#define M0_PIN 7
#define M1_PIN 8
#define AUX_PIN 4
#define lora_power TSMT_PWR_30DB

DHT dht(DHTPIN, DHTTYPE);
HardwareSerial* debugSerial = &Serial;;
SoftwareSerial LoraSerial(10,9);// RX, TX
UDOpenLora loraBoard(&LoraSerial);
/*--------------SENDER------------*/

int networkAddrH = 0x35;
int networkAddrL = 0x35;
int networkChanel = 0x19;
int deviceAddrH = 0x30; // 1 device : 1 addrH & addrL : receiver device address
int deviceAddrL = 0x30;
byte ADDR_H, ADDR_L;
char temp_t[10];
char humd_t[10];
char msg[16];
int length_t = 0;
float h = 0;
float t = 0;

void setup() {
  Serial.begin(9600); 
  dht.begin();
  LoraSerial.begin(9600);  
  loraBoard.setDebugPort(debugSerial);
  Serial.print("Configure Lora Module: ");
  loraBoard.setIOPin(M0_PIN, M1_PIN, AUX_PIN);
  delay(1000);
  loraBoard.LoraBegin((byte)(networkAddrH), (byte)(networkAddrL), (byte)(networkChanel), lora_power);
  Serial.println("GATE DONE");
}

void loop() {
  h = dht.readHumidity();             //read data
  t = dht.readTemperature();          //read data
  length_t = 0;
  length_t = String(h).length();
  String(h).toCharArray(humd_t,length_t);
  length_t=String(t).length();
  String(t).toCharArray(temp_t,length_t);
  strcpy(msg,"T");
  strcat(msg,temp_t);
  strcat(msg," H");
  strcat(msg,humd_t);
  loraBoard.SendMessage((byte)(deviceAddrH), (byte)(deviceAddrL),msg); //send message
  memset(msg,'\0',16);
  memset(temp_t,'\0',10);
  memset(humd_t,'\0',10);
  delay(5000);
}
