/*
  Van Si Nguyen
  UDOpenLora.h
  nguyenvansi0610@gmail.com 
  source E32-TTL-100 
*/

#ifndef UDOpenLora_H_
#define UDOpenLora_H_

#include <SoftwareSerial.h>
#include <Arduino.h>


typedef enum {
  RET_SUCCESS = 0,
  RET_ERROR_UNKNOWN,  /* something shouldn't happened */
  RET_NOT_SUPPORT,
  RET_NOT_IMPLEMENT,
  RET_NOT_INITIAL,
  RET_INVALID_PARAM,
  RET_DATA_SIZE_NOT_MATCH,
  RET_BUF_TOO_SMALL,
  RET_TIMEOUT,
  RET_HW_ERROR,
} RET_STATUS;

enum MODE_TYPE
{
  MODE_0_NORMAL = 0,
  MODE_1_WAKE_UP,
  MODE_2_POWER_SAVIN,
  MODE_3_SLEEP,
  MODE_INIT = 0xFF
};

//SPED+
enum SLEEP_MODE_CMD_TYPE
{
  W_CFG_PWR_DWN_SAVE = 0xC0,
  R_CFG              = 0xC1,
  W_CFG_PWR_DWN_LOSE = 0xC2,
  R_MODULE_VERSION   = 0xC3,
  W_RESET_MODULE     = 0xC4
};

enum UART_FORMAT_TYPE
{
  UART_FORMAT_8N1 = 0x00,  /*no   parity bit one stop*/
  UART_FORMAT_8O1 = 0x01,  /*odd  parity bit one stop*/
  UART_FORMAT_8E1 = 0x02   /*even parity bitone stop*/
};

enum UART_BPS_TYPE
{
  UART_BPS_1200 = 0x00,
  UART_BPS_9600 = 0x03,
  UART_BPS_115200 = 0x07
};

enum AIR_BPS_TYPE
{
  AIR_BPS_300   = 0x00,
  AIR_BPS_2400  = 0x02,
  AIR_BPS_19200 = 0x05
};
//SPED-

//410~441MHz : 410M + CHAN*1M
enum AIR_CHAN_TYPE
{
  AIR_CHAN_410M = 0x00,
  AIR_CHAN_433M = 0x17,
  AIR_CHAN_441M = 0x1F
};

//OPTION+
#define TRSM_TT_MODE    0x00  /*Transparent Transmission*/
#define TRSM_FP_MODE    0x01  /*Fixed-point transmission mode*/

#define OD_DRIVE_MODE   0x00
#define PP_DRIVE_MODE   0x01

enum WEAK_UP_TIME_TYPE
{
  WEAK_UP_TIME_250  = 0x00,
  WEAK_UP_TIME_1000 = 0x03,
  WEAK_UP_TIME_2000 = 0x07
};

#define DISABLE_FEC     0x00
#define ENABLE_FEC      0x01

//Transmit power
enum TSMT_PWR_TYPE
{
  TSMT_PWR_30DB = 0x00,
  TSMT_PWR_27DB = 0x01,
  TSMT_PWR_24DB = 0x02,
  TSMT_PWR_21DB = 0x03
};
//OPTION-

#pragma pack(push, 1)
struct SPEDstruct {
  uint8_t air_bps : 3; //bit 0-2
  uint8_t uart_bps: 3; //bit 3-5
  uint8_t uart_fmt: 2; //bit 6-7
};
struct OPTIONstruct {
  uint8_t tsmt_pwr    : 2; //bit 0-1
  uint8_t enFWC       : 1; //bit 2
  uint8_t wakeup_time : 3; //bit 3-5
  uint8_t drive_mode  : 1; //bit 6
  uint8_t trsm_mode   : 1; //bit 7
};
struct CFGstruct {
  uint8_t HEAD;
  uint8_t ADDH;
  uint8_t ADDL;
  struct SPEDstruct   SPED_bits;
  uint8_t CHAN;
  struct OPTIONstruct OPTION_bits;
};
struct MVerstruct {
  uint8_t HEAD;
  uint8_t Model;
  uint8_t Version;
  uint8_t features;
};
#pragma pack(pop)

#define TIME_OUT_CNT  100
#define MAX_TX_SIZE   58

#define endMessSymbol 0xAB

class UDOpenLora
{
  public:
    UDOpenLora(SoftwareSerial* LoraSerial);
    bool setDebugPort(HardwareSerial* debugPort);    
    bool setIOPin(uint8_t M0_PIN, uint8_t M1_PIN, uint8_t AUX_PIN);
    bool LoraBegin(uint8_t ADDR_H, uint8_t ADDR_L, UART_FORMAT_TYPE UART_FORMAT, UART_BPS_TYPE UART_BPS, AIR_BPS_TYPE AIR_BPS, uint8_t chanel, uint8_t trsm_mode, uint8_t drive_mode, WEAK_UP_TIME_TYPE WEAK_UP_TIME, uint8_t enFWC, TSMT_PWR_TYPE power);
    bool LoraBegin(uint8_t ADDR_H, uint8_t ADDR_L, uint8_t chanel, TSMT_PWR_TYPE power);
    bool LoraBegin(uint8_t ADDR_H, uint8_t ADDR_L, uint8_t chanel);
    bool join_lora_network_request(uint8_t chanel, char* nameOfDevice);
    
    RET_STATUS ReceiveMsg(byte *ADDRH, byte *ADDRL, byte *pdatabuf, uint8_t *data_len);
    RET_STATUS SendMessage(byte ADDRH, byte ADDRL, byte chan, char* message);
    RET_STATUS SendMessage(byte ADDRH, byte ADDRL, char* message);
    
  private:    
    uint8_t ADDR_H; 
    uint8_t ADDR_L;
    uint8_t main_chanel;       
    uint8_t M0_PIN; 
    uint8_t M1_PIN;
    uint8_t AUX_PIN;
    HardwareSerial* debugPort;
    SoftwareSerial* LoraSerial;
    bool AUX_HL;
    bool ReadAUX();
    RET_STATUS WaitAUX_H();
    bool chkModeSame(MODE_TYPE mode);
    void SwitchMode(MODE_TYPE mode);
    void cleanUARTBuf();
    void triple_cmd(SLEEP_MODE_CMD_TYPE Tcmd);
    RET_STATUS Module_info(uint8_t* pReadbuf, uint8_t buf_len);
    RET_STATUS Write_CFG_PDS(struct CFGstruct* pCFG);
    RET_STATUS Read_CFG(struct CFGstruct* pCFG);
    RET_STATUS Read_module_version(struct MVerstruct* MVer);
    void Reset_module();
    RET_STATUS SleepModeCmd(uint8_t CMD, void* pBuff);
    

};



#endif /* UDOpenLora_H_ */
