#include <SoftwareSerial.h>
#include <UDOpenLora.h>

HardwareSerial* debugSerial = &Serial;
SoftwareSerial LoraSerial(10,9);// RX, TX
UDOpenLora loraBoard(&LoraSerial);


#define lora_power TSMT_PWR_30DB
byte DeviceLoraAddr_H = 0x35;
byte DeviceLoraAddr_L = 0x35;
byte LoraChanel = 0x19;
byte destADDR_H = 0x30;
byte destADDR_L = 0x30;
byte SourceAddr_H, SourceAddr_L;
char data_buf[128];
uint8_t data_len;
char Mess[128] = "Hello! I'm UDOpenLora Sender";

#define buttonPin 2
#define buzzerPin 6
#define M0_PIN 7
#define M1_PIN 8
#define AUX_PIN 4

void setup()
{
  Serial.begin(9600);
  LoraSerial.begin(9600);  
  loraBoard.setDebugPort(debugSerial); 
  pinMode(buttonPin, INPUT);  
  pinMode(buzzerPin, OUTPUT);  
  Serial.print("Configure Lora Module: ");
  loraBoard.setIOPin(M0_PIN, M1_PIN, AUX_PIN);
  if (loraBoard.LoraBegin(DeviceLoraAddr_H, DeviceLoraAddr_L, LoraChanel, lora_power))
  {    
    Serial.println("Configure ERROR!");
  }
  Serial.println("Configure Lora Module DONE!");  
  loraBoard.SendMessage(destADDR_H, destADDR_L, Mess);
}

void loop() 
{   
  if(isPressButton())
  {
    delay(50);
    if(isPressButton())
    {
      while(isPressButton());
      turnOnBuzzer();
      loraBoard.SendMessage(destADDR_H, destADDR_L, Mess);
    }
  }
  else
  {
    turnOffBuzzer();
  }
  if(loraBoard.ReceiveMsg(&SourceAddr_H, &SourceAddr_L, data_buf, &data_len)==RET_SUCCESS) 
  {
    Serial.println();
    Serial.print("Msg from: 0x");
    Serial.print(SourceAddr_H, HEX);
    Serial.println(SourceAddr_L, HEX);
    Serial.print("Message length:");
    Serial.println(data_len);
    Serial.print("Receive Message:");
    for(int i = 0; i < data_len; i++)
    {
      Serial.print(data_buf[i]);
    }
    Serial.println();
    turnOnBuzzer();
    delay(1000);
    turnOffBuzzer();
  }   
}


bool isPressButton()
{
   return !digitalRead(buttonPin);
}

void turnOffBuzzer()
{
  digitalWrite(buzzerPin, LOW);
}

void turnOnBuzzer()
{
  digitalWrite(buzzerPin, HIGH);
}
