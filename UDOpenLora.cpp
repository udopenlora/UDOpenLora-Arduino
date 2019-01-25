#include "UDOpenLora.h"


UDOpenLora::UDOpenLora(SoftwareSerial* LoraSerial)
{
  this->LoraSerial = LoraSerial;
}

bool UDOpenLora::setDebugPort(HardwareSerial* debugPort)
{
  this->debugPort = debugPort;
}

bool UDOpenLora::setIOPin(uint8_t M0_PIN, uint8_t M1_PIN, uint8_t AUX_PIN)
{  
  this->M0_PIN = M0_PIN;
  this->M1_PIN = M1_PIN;
  this->AUX_PIN = AUX_PIN;    
  pinMode(this->M0_PIN, OUTPUT);
  pinMode(this->M1_PIN, OUTPUT);
  pinMode(this->AUX_PIN, INPUT_PULLUP); 
}

bool UDOpenLora::LoraBegin(uint8_t ADDR_H, uint8_t ADDR_L, UART_FORMAT_TYPE UART_FORMAT, UART_BPS_TYPE UART_BPS, AIR_BPS_TYPE AIR_BPS, uint8_t chanel, uint8_t trsm_mode, uint8_t drive_mode, WEAK_UP_TIME_TYPE WEAK_UP_TIME, uint8_t enFWC, TSMT_PWR_TYPE power)
{
  WaitAUX_L();
  WaitAUX_H();
  if(debugPort != NULL)
  { 
    this->debugPort->println("LoraBegin...");
  }
  this->ADDR_H = ADDR_H; 
  this->ADDR_L = ADDR_L;
  this->main_chanel = chanel;  
  RET_STATUS STATUS = RET_SUCCESS;
  struct CFGstruct pCFG;  
  pCFG.HEAD = W_CFG_PWR_DWN_SAVE;
  pCFG.ADDH = this->ADDR_H;
  pCFG.ADDL = this->ADDR_L;
  pCFG.SPED_bits.uart_fmt = UART_FORMAT;
  pCFG.SPED_bits.uart_bps = UART_BPS;
  pCFG.SPED_bits.air_bps = AIR_BPS;
  pCFG.CHAN = this->main_chanel;
  pCFG.OPTION_bits.trsm_mode = trsm_mode;
  pCFG.OPTION_bits.drive_mode = drive_mode;
  pCFG.OPTION_bits.wakeup_time = WEAK_UP_TIME;
  pCFG.OPTION_bits.enFWC = enFWC;
  pCFG.OPTION_bits.tsmt_pwr = power;
  STATUS = this->SleepModeCmd(W_CFG_PWR_DWN_SAVE, &pCFG);
  this->SleepModeCmd(W_RESET_MODULE, NULL);
  delay(300);
  STATUS = this->SleepModeCmd(R_CFG, &pCFG);
  return STATUS;  
}
    
bool UDOpenLora::LoraBegin(uint8_t ADDR_H, uint8_t ADDR_L, uint8_t chanel, TSMT_PWR_TYPE power)
{
  return LoraBegin(ADDR_H, ADDR_L, UART_FORMAT_8N1, UART_BPS_9600, AIR_BPS_2400, chanel, TRSM_FP_MODE, PP_DRIVE_MODE, WEAK_UP_TIME_250, ENABLE_FEC, power); 
}
    
bool UDOpenLora::LoraBegin(uint8_t ADDR_H, uint8_t ADDR_L, uint8_t chanel)
{
  return LoraBegin(ADDR_H, ADDR_L, chanel, TSMT_PWR_30DB); 
}

bool UDOpenLora::join_lora_network_request(uint8_t chanel, char* nameOfDevice)
{
  char mess[256];
  int lengthOfMess = sprintf (mess, "%c%c%c\"L_DT\":\"%s\"", 0xB0, 0xB1, 0xB0 , nameOfDevice);
  this->SendMessage(0x00, 0x00, chanel, mess);
}


bool UDOpenLora::ReadAUX()
{
  return digitalRead(this->AUX_PIN);
}


RET_STATUS UDOpenLora::WaitAUX_L()
{
  RET_STATUS STATUS = RET_SUCCESS;
  uint8_t cnt = 0;
  uint8_t data_buf[100], data_len;
  while((this->ReadAUX()==HIGH) && (cnt++<TIME_OUT_CNT))
  {
    if(debugPort != NULL)
    { 
      this->debugPort->print(".");
    }
    delay(25);
  }
  if(cnt==0)
  {
  }
  else if(cnt>=TIME_OUT_CNT)
  {
    STATUS = RET_TIMEOUT;
    if(debugPort != NULL)
    { 
      this->debugPort->println(" TimeOut");
    }
  }
  else
  {
    if(debugPort != NULL)
    { 
      this->debugPort->println("");
    }
  }
  return STATUS;
}

RET_STATUS UDOpenLora::WaitAUX_H()
{
  RET_STATUS STATUS = RET_SUCCESS;
  uint8_t cnt = 0;
  uint8_t data_buf[100], data_len;
  while((this->ReadAUX()==LOW) && (cnt++<TIME_OUT_CNT))
  {
    if(debugPort != NULL)
    { 
      this->debugPort->print(".");
    }
    delay(100);
  }
  if(cnt==0)
  {
  }
  else if(cnt>=TIME_OUT_CNT)
  {
    STATUS = RET_TIMEOUT;
    if(debugPort != NULL)
    { 
      this->debugPort->println(" TimeOut");
    }
  }
  else
  {
    if(debugPort != NULL)
    { 
      this->debugPort->println("");
    }
  }
  return STATUS;
}

bool UDOpenLora::chkModeSame(MODE_TYPE mode)
{
  static MODE_TYPE pre_mode = MODE_INIT;
  if(pre_mode == mode)
  {
    return true;
  }
  else
  {
    if(debugPort != NULL)
    { 
      this->debugPort->print("SwitchMode: from ");
      this->debugPort->print(pre_mode, HEX);
      this->debugPort->print(" to "); 
      this->debugPort->println(mode, HEX);
    }
    pre_mode = mode;
    return false;
  }
}


void UDOpenLora::SwitchMode(MODE_TYPE mode)
{
  if(!this->chkModeSame(mode))
  {
    WaitAUX_H();
    switch (mode)
    {
      case MODE_0_NORMAL:
        // Mode 0 | normal operation
        digitalWrite(M0_PIN, LOW);
        digitalWrite(M1_PIN, LOW);
        break;
      case MODE_1_WAKE_UP:
        digitalWrite(M0_PIN, HIGH);
        digitalWrite(M1_PIN, LOW);
        break;
      case MODE_2_POWER_SAVIN:
        digitalWrite(M0_PIN, LOW);
        digitalWrite(M1_PIN, HIGH);
        break;
      case MODE_3_SLEEP:
        // Mode 3 | Setting operation
        digitalWrite(M0_PIN, HIGH);
        digitalWrite(M1_PIN, HIGH);
        break;
      default:
        return ;
    }
    WaitAUX_H();
    delay(10);
  }
}

void UDOpenLora::cleanUARTBuf()
{
  bool IsNull = true;
  while (this->LoraSerial->available())
  {
    IsNull = false;
    this->LoraSerial->read();
  }
}

void UDOpenLora::triple_cmd(SLEEP_MODE_CMD_TYPE Tcmd)
{
  uint8_t CMD[3] = {Tcmd, Tcmd, Tcmd};
  this->LoraSerial->write(CMD, 3);
  delay(50); 
}


RET_STATUS UDOpenLora::Module_info(uint8_t* pReadbuf, uint8_t buf_len)
{
  RET_STATUS STATUS = RET_SUCCESS;
  uint8_t Readcnt, idx;
  Readcnt = this->LoraSerial->available();
  if (Readcnt == buf_len)
  {
    for(idx=0;idx<buf_len;idx++)
    {
      *(pReadbuf+idx) = this->LoraSerial->read();
      if(debugPort != NULL)
      { 
        this->debugPort->print(" 0x");
        this->debugPort->print(0xFF & *(pReadbuf+idx), HEX);
      } 
    } 
    if(debugPort != NULL)
    { 
      this->debugPort->println("");
    } 
  }
  else
  {
    STATUS = RET_DATA_SIZE_NOT_MATCH;
    if(debugPort != NULL)
    { 
      this->debugPort->print("  RET_DATA_SIZE_NOT_MATCH - Readcnt: ");
      this->debugPort->println(Readcnt);
    } 
    cleanUARTBuf();
  }
  return STATUS;
}

RET_STATUS UDOpenLora::Write_CFG_PDS(struct CFGstruct* pCFG)
{
  this->LoraSerial->write((uint8_t *)pCFG, 6);
  this->WaitAUX_H();
  delay(1200);
  return RET_SUCCESS;
}


RET_STATUS UDOpenLora::Read_CFG(struct CFGstruct* pCFG)
{
  RET_STATUS STATUS = RET_SUCCESS;
  this->cleanUARTBuf();
  this->triple_cmd(R_CFG);
  STATUS = this->Module_info((uint8_t *)pCFG, sizeof(CFGstruct));
  if(STATUS == RET_SUCCESS)
  {
    if(debugPort != NULL)
    { 
      this->debugPort->print("  HEAD:     ");
      this->debugPort->println(pCFG->HEAD, HEX);
      this->debugPort->print("  ADDH:     ");
      this->debugPort->println(pCFG->ADDH, HEX);
      this->debugPort->print("  ADDL:     ");
      this->debugPort->println(pCFG->ADDL, HEX);
      this->debugPort->print("  CHAN:     ");
      this->debugPort->println(pCFG->CHAN, HEX);
    } 
  }
  return STATUS;
}

RET_STATUS UDOpenLora::Read_module_version(struct MVerstruct* MVer)
{
  RET_STATUS STATUS = RET_SUCCESS;
  this->cleanUARTBuf();
  this->triple_cmd(R_MODULE_VERSION);
  STATUS = this->Module_info((uint8_t *)MVer, sizeof(MVerstruct));
  if(STATUS == RET_SUCCESS)
  {
    if(debugPort != NULL)
    { 
      this->debugPort->print("  HEAD:     0x");
      this->debugPort->println(MVer->HEAD, HEX);
      this->debugPort->print("  Model:    0x");
      this->debugPort->println(MVer->Model, HEX);
      this->debugPort->print("  Version:  0x");
      this->debugPort->println(MVer->Version, HEX);
      this->debugPort->print("  features: 0x");
      this->debugPort->println(MVer->features, HEX);
    } 
  }
  return RET_SUCCESS;
}

void UDOpenLora::Reset_module()
{
  this->triple_cmd(W_RESET_MODULE);
  this->WaitAUX_H();
  delay(1000);
}


RET_STATUS UDOpenLora::SleepModeCmd(uint8_t CMD, void* pBuff)
{
  RET_STATUS STATUS = RET_SUCCESS;
  if(debugPort != NULL)
  { 
    this->debugPort->print("SleepModeCmd: 0x");
    this->debugPort->println(CMD, HEX);
  } 
  this->WaitAUX_H();
  this->SwitchMode(MODE_3_SLEEP);
  switch (CMD)
  {
    case W_CFG_PWR_DWN_SAVE:
      STATUS = this->Write_CFG_PDS((struct CFGstruct* )pBuff);
      break;
    case R_CFG:
      STATUS = this->Read_CFG((struct CFGstruct* )pBuff);
      break;
    case W_CFG_PWR_DWN_LOSE:

      break;
    case R_MODULE_VERSION:
      this->Read_module_version((struct MVerstruct* )pBuff);
      break;
    case W_RESET_MODULE:
      this->Reset_module();
      break;
    default:
      return RET_INVALID_PARAM;
  }
  this->WaitAUX_H();
  return STATUS;
}


RET_STATUS UDOpenLora::ReceiveMsg(byte *ADDRH, byte *ADDRL, byte *pdatabuf, uint8_t *data_len)
{
  RET_STATUS STATUS = RET_NOT_IMPLEMENT;
  uint8_t idx;
  this->SwitchMode(MODE_0_NORMAL);
  if(this->LoraSerial->available())
  {
    delay(100);
    *data_len = this->LoraSerial->available()- 2;  
    *ADDRH = this->LoraSerial->read();
    *ADDRL = this->LoraSerial->read();
    if (*data_len > 0)
    {
      for(idx=0;idx<*data_len;idx++)
        *(pdatabuf+idx) = this->LoraSerial->read();
      if(*(pdatabuf+(*data_len-1)) == 0xAB)
      {
        *(pdatabuf+(*data_len-1)) = '\0';
        *data_len = *data_len - 1 ;
      }
      else
      {
        STATUS = RET_NOT_IMPLEMENT;
      }
      STATUS = RET_SUCCESS;
    }
    else
    {
      STATUS = RET_NOT_IMPLEMENT;
    }
  }
  return STATUS;
}


RET_STATUS UDOpenLora::SendMessage(byte ADDRH, byte ADDRL, byte chan, char* message)
{
  RET_STATUS STATUS = RET_SUCCESS;
  this->SwitchMode(MODE_0_NORMAL);
  if(this->ReadAUX()!=HIGH)
  {
    return RET_NOT_IMPLEMENT;
  }
  delay(10);
  if(this->ReadAUX()!=HIGH)
  {
    return RET_NOT_IMPLEMENT;
  }  
  char SendBuf[128] ;
  int lengthMess = sprintf (SendBuf, "%c%c%c%c%c%s%c", ADDRH, ADDRL, chan, this->ADDR_H, this->ADDR_L, message, (byte)(endMessSymbol));
  SendBuf[lengthMess] = '\0';
  if(debugPort != NULL)
  {     
    this->debugPort->print("Send to: 0x");
    this->debugPort->print(ADDRH, HEX);
    this->debugPort->println(ADDRL, HEX);
    this->debugPort->print("Mess: ");
    this->debugPort->println(message);
    this->debugPort->print("Length: ");
    this->debugPort->println(lengthMess - 6);
  } 
  this->LoraSerial->write(SendBuf, lengthMess);
  return STATUS;
}


RET_STATUS UDOpenLora::SendMessage(byte ADDRH, byte ADDRL, char* message)
{
  return SendMessage(ADDRH, ADDRL, main_chanel, message);
}
