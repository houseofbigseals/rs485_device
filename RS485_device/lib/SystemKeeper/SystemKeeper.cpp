#include "SystemKeeper.h"


// ========================= SystemKeeper methods implementation =============================
SystemKeeper::SystemKeeper(BaseDevice **device_pointer_, uint16_t devices_number_, HardwareSerial *s)
{
    // hm
    _device_pointer = device_pointer_;
    _devices_number = devices_number_;
    _s = s;
}

void SystemKeeper::parse_package(uint8_t * package, uint16_t len)
{

    // at first lets check if it has correct length
    if(len != d_master_message_len)
    {
        // do nothing
        // its incorrect
    }
    else
    {
        // then check start byte
        if(package[0] != d_start_byte)
        {
            // do nothing
            // its incorrect
        }
        else
        {
            // check crc sum 
            // just put pointer of array and len -2 to d_crc16 function
            uint16_t crc16_check = d_crc16(package, len-2);
            // split it to half
            uint8_t crc1 = (uint8_t)((crc16_check & 0xFF00) >> 8);
            uint8_t crc2 = (uint8_t)(crc16_check & 0x00FF);
            if (crc1 != package[len-2] || crc2 != package[len-1])
            {
                // do nothing
                // its incorrect
            }
            else
            {
                // check if it is for one of our devices
                for (uint16_t d = 0; d < _devices_number; d++)
                {
                    // go through all devices and compare to their _uid
                    uint16_t did = _device_pointer[d]->get_uid();
                    // unite uid from message to uint16 for simplicity
                    uint16_t mid = d_unite2(package[1], package[2]);
                    if (did == mid)
                    {
                        // we found the device to which the message is addressed
                        // then lets create MM object from byte array
                        MasterMeassage message = MasterMeassage(
                            package[1],
                            package[2],
                            package[3],
                            package[4],
                            package[5],
                            package[6],
                            package[7],
                            package[8],
                            package[9]
                        );
                        // then put it to found device to get resulted slave message 
                        SlaveMessage sm = _device_pointer[d]->handle_mm(message);
                        // then we have to send it back to master pc
                        send_slave_message(sm);
                    }
                }
            }
        }   
    }
}

void SystemKeeper::do_main_loop()
{
    // that function must be used as fast as we can
    // so no any sleeping
    //SoftwareSerial()
    // at first lets check if we have incoming messages through uart
    if(_s->available())
    {
        // if we have any lets get them
        uint8_t message[30]; // we dont want read more than 30 characters //TODO fix
        uint16_t len = 0;
        while(_s->available())
        {
            message[len] = _s->read();
            len++;
        }
        // then lets parse it
        parse_package(message, len);
    }
    // if we got message or not we have to do routine for all devices
    for(uint16_t d = 0; d < _devices_number; d++)
    {
        // we cant control the time of every device work
        // so we have to wait or use timer interruptions
        // or use rtos and tasks abstraction
        _device_pointer[d]->do_your_duty();
    }

}

void SystemKeeper::send_slave_message(SlaveMessage sm)
{
    /*
    old example
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
    */

    // disgusting
    // go to send mode
    _s->write(sm.START_BYTE);
    delayMicroseconds(5);
    _s->write(sm.NET_ADDR_1);
    delayMicroseconds(5);
    _s->write(sm.DEV_ADDR_2);
    delayMicroseconds(5);
    _s->write(sm.STATUS_1);
    delayMicroseconds(5);
    _s->write(sm.STATUS_2);
    delayMicroseconds(5);
    _s->write(sm.VALUE_1);
    delayMicroseconds(5);
    _s->write(sm.VALUE_2);
    delayMicroseconds(5);
    _s->write(sm.VALUE_3);
    delayMicroseconds(5);
    _s->write(sm.VALUE_4);
    delayMicroseconds(5);
    _s->write(sm.CRC_16_1);
    delayMicroseconds(5);
    _s->write(sm.CRC_16_2);
    delayMicroseconds(5);

    _s->flush();
    delay(5);
    // go back to receive mode
}