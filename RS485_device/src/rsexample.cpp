#include <Arduino.h>
#include <SoftwareSerial.h>

/*
 * NOTE
 * use only software serial with rs485 scheme because cheap china clones of arduino
 * may have problems with connecting max485 to real rx-tx pins (0 and 1)
 * or may not, but i got such problem, so now you are warned
 */

#define SerialTxControl 3
#define RS485Transmit HIGH
#define RS485Receive LOW
const int command_maxlen = 32;
unsigned long previousMillis = 0;
const unsigned long interval = 100; //timer interval in ms
SoftwareSerial myserial(12, 13); // RX, TX

void check_rs485();

void setup()
{
  Serial.begin(9600);
  //Serial.println("Start of init");
  
  myserial.begin(9600);
  pinMode(SerialTxControl, OUTPUT);
  digitalWrite(SerialTxControl, RS485Receive);
}

void loop()
{
    // it is the main loop
  check_rs485();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    // we have to check flags
   /* if(show_time)
    {
      do_show_time();
    }
    if(show_temp)
    {
      do_show_temp();
    }*/
}

void check_rs485()
{
  if( myserial.available() )
    {
        byte ans = 0xBB;
        byte common_sign = 0xAA;
        byte reqv_start_byte = 0x45;
        byte resp_start_byte = 0x54;
        byte stop_byte = 0xFA;
        byte my_addr = 0x05;
        byte byte_com = 0x00;
        byte ok_byte = 0x01;
        byte rs485_command[command_maxlen];
        int answer_len = 8;
        delay(5);
        int i = 0;
        while( myserial.available() )
        {
            rs485_command[i] = myserial.read();
            i++;
        }
        byte_com = rs485_command[3];
        byte byte_addr = rs485_command[2];
        /*if (byte_addr == my_addr)
        {
          switch (byte_com)
          {
            case 0x00:
              manual_mode = false;
              ans = ok_byte;
              break;
  
            case 0x01:
              manual_mode = true;
              ans = ok_byte;
              break;
  
            case 0x03:
              led = true;
              ans = ok_byte;
              digitalWrite(13, true);
              break;
  
            case 0x04:
              led = false;
              ans = ok_byte;
              digitalWrite(13, false);
              break;
              
            case 0x05:
              pump = true;
              ans = ok_byte;
              break;
  
            case 0x06:
              pump = false;
              ans = ok_byte;
              break;
  
            case 0x07:
              fans = true;
              ans = ok_byte;
              break;
  
            case 0x08:
              fans = false;
              ans = ok_byte;
              break;
  
            case 0x11:
              ans = 0xFA;
              break;
  
            default:
              ans = 0xBB;
              break;
          }*/
      byte crc_low = 0x06;
      byte crc_high = 0x06;
      int answer_len = 8;
      digitalWrite(SerialTxControl, RS485Transmit);
      byte out_command[answer_len] = {0xAA, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, crc_low, crc_high};
      int i = 0;
      while (i < answer_len)
      {
        myserial.write(out_command[i]);
        i++;
        delayMicroseconds(5);
      }
      myserial.flush();
      //Serial.write(out_command, 8);
      delay(5);
      digitalWrite(SerialTxControl, RS485Receive);
      }
    }
}