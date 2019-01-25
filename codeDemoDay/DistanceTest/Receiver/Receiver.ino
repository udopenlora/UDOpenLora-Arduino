#include <SoftwareSerial.h>
#include <UDOpenLora.h>

HardwareSerial* debugSerial = &Serial;
SoftwareSerial LoraSerial(10,9);// RX, TX
UDOpenLora loraBoard(&LoraSerial);


#define lora_power TSMT_PWR_30DB
byte DeviceLoraAddr_H = 0x30;
byte DeviceLoraAddr_L = 0x30;
byte LoraChanel = 0x19;
byte destADDR_H = 0x35;
byte destADDR_L = 0x35;
byte SourceAddr_H, SourceAddr_L;
char data_buf[128];
uint8_t data_len;
String Mess = "UDLR";

#define buttonPin 2
#define buzzerPin 6
#define M0_PIN 7
#define M1_PIN 8
#define AUX_PIN 4
#define ledDebug1 A1
#define ledDebug2 A2

void setup()
{
  Serial.begin(9600);
  LoraSerial.begin(9600);  
  loraBoard.setDebugPort(debugSerial); 
  pinMode(buttonPin, INPUT_PULLUP);  
  pinMode(buzzerPin, OUTPUT);  
  pinMode(ledDebug1, OUTPUT);  
  pinMode(ledDebug2, OUTPUT);  
  Serial.print("Configure Lora Module: ");
  loraBoard.setIOPin(M0_PIN, M1_PIN, AUX_PIN);
  if (loraBoard.LoraBegin(DeviceLoraAddr_H, DeviceLoraAddr_L, LoraChanel, lora_power))
  {    
    Serial.println("Configure ERROR!");
  }
  Serial.println("Configure Lora Module DONE!");  
}

void loop() 
{   
  if(loraBoard.ReceiveMsg(&SourceAddr_H, &SourceAddr_L, data_buf, &data_len)==RET_SUCCESS) 
  {
    Serial.println();
    Serial.print("Msg from: 0x");
    Serial.print(SourceAddr_H, HEX);
    Serial.println(SourceAddr_L, HEX);
    Serial.print("Message length:");
    Serial.println(data_len);
    Serial.print("Receive Message:");
    String mess = String(data_buf);    
    Serial.println(mess);
    if(mess == Mess)
    {
      Serial.println("mess == Mess");
//      turnOnBuzzer();
//      delay(500);
//      turnOffBuzzer();
      loraBoard.SendMessage(destADDR_H, destADDR_L, "MOK");
    }
  }   
}


bool isPressButton()
{
   return !digitalRead(buttonPin);
}

void turnOffBuzzer()
{
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledDebug1, LOW);
}

void turnOnBuzzer()
{
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(ledDebug1, HIGH);
}
