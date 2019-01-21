# UDOpenLora-Arduino ![alt text](https://camo.githubusercontent.com/cfcaf3a99103d61f387761e5fc445d9ba0203b01/68747470733a2f2f7472617669732d63692e6f72672f6477796c2f657374612e7376673f6272616e63683d6d6173746572)
This library create for UDOpenLora Arduino Shield from https://www.facebook.com/UDOpenLoraTeam or E32-TTL Lora UART module from EByte.
## Requirements
Arduino IDE 1.5
## Tested with:
UDOpenLora Arduino Shield.

[E32-433T20S1](http://www.ebyte.com/en/product-view-news.aspx?id=229).

[E32-433T30S](http://www.ebyte.com/en/product-view-news.aspx?id=253).

# Installation & Usage:
## Setup UART connection:
Default: (you can choice other pin)

| ARDUINO | E32 Module |
|------|-----|
|PIN 10 | TX |
|PIN 9  | RX |

```sh
HardwareSerial* debugSerial = &Serial;
SoftwareSerial LoraSerial(10,9);// RX, TX
```
## Setup Lora board:
DeviceLoraAddr: 2 bytes of Lora ID 
destADDR : 2 bytes ID of the lora node we want to send data

```sh
#define lora_power TSMT_PWR_30DB
byte DeviceLoraAddr_H = 0x35;
byte DeviceLoraAddr_L = 0x35;
byte LoraChanel = 0x19;
byte destADDR_H = 0x30;
byte destADDR_L = 0x30;
byte SourceAddr_H, SourceAddr_L;
char data_buf[128];
uint8_t data_len;
char Mess[128] = "Hello! I'm UDOpenLora";
```

## Setup other pins of E32-TTL:

| ARDUINO | E32 Module |
|------|-----|
|PIN 7 | PIN M0 |
|PIN 8  | PIN M1 |
|PIN 4  | PIN AUX |


## Setup buzzer and button pin of UDOpenLora Arduino Shield:

| ARDUINO | E32 Module |
|------|-----|
|PIN 2 | button Pin |
|PIN 6  | buzzer Pin |