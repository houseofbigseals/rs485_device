#include "SystemKeeper.h"


// ========================= SystemKeeper methods implementation =============================
SystemKeeper::SystemKeeper(
    BaseDevice **device_pointer_,
    uint16_t devices_number_,
    SoftwareSerial * s,
    uint16_t sspin,
    bool debug
    )
{
    // hm
    _device_pointer = device_pointer_;
    _devices_number = devices_number_;
    _s = s;
    _debug_s = &Serial;
    software_serial_pin = sspin;
    D_RS485_DEBUG_OUTPUT_ALLOWED = debug;

}

void SystemKeeper::parse_package(uint8_t * package, uint16_t len)
{
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->println("we got smth");
    }

    // at first lets check if it has correct length
    if(len != d_master_message_len)
    {
        // do nothing
        // its incorrect
        if(D_RS485_DEBUG_OUTPUT_ALLOWED)
        {
        _debug_s->print("but len is incorrect: ");
        _debug_s->println(len);
        }
    }
    else
    {
        // then check start byte
        if(package[0] != d_start_byte)
        {
            // do nothing
            // its incorrect
            if(D_RS485_DEBUG_OUTPUT_ALLOWED)
            {
            _debug_s->print("start byte incorrect: ");
            _debug_s->print(package[0], HEX);
            _debug_s->println(" ");
            }
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
                if(D_RS485_DEBUG_OUTPUT_ALLOWED)
                {
                _debug_s->print("crc incorrect: ");
                _debug_s->print(package[len-2], HEX);
                _debug_s->print(" ");
                _debug_s->print(package[len-1], HEX);
                _debug_s->println(" ");
                }
            }
            else
            {
                if(D_RS485_DEBUG_OUTPUT_ALLOWED)
                {
                    // print message we got
                    for(uint8_t i = 0; i<len; i++)
                    {
                    _debug_s->println(package[i], HEX);
                    delayMicroseconds(5);
                    }
                }
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
                       // | START_BYTE | DEV_ADDR_LOW | DEV_ADDR_HIGH | COMMAND | REG_ADDR_LOW | REG_ADDR_HIGH |
   // VALUE_1 | VALUE_2 | VALUE_3 | VALUE_4 | CRC16_LOW | CRC16_HIGH |
                        MasterMeassage message = MasterMeassage(
                            package[1], //NET_ADDR_1_,
                            package[2], //DEV_ADDR_2_,
                            package[6], //VALUE_1_,
                            package[7], //VALUE_2_,
                            package[8], //VALUE_3_,
                            package[9], //VALUE_4_,
                            package[4], //REG_ADDR_1_
                            package[5], //REG_ADDR_2_
                            package[3] //COMMAND_
                        );
                        // then put it to found device to get resulted slave message 
                        if(D_RS485_DEBUG_OUTPUT_ALLOWED)
                        {
                            _debug_s->println("we found adressed device");
                        }
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
        delay(5);  // strategical delay - awaiting until all bytes will be sent
        while(_s->available())
        {
            message[len] = _s->read();
            len++;
            delay(1);
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
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->println("start writing answer");
    }
    // disgusting
    // go to send mode
    digitalWrite(software_serial_pin, RS485Transmit);
    // then send bytes
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

    // go back to read mode
    digitalWrite(software_serial_pin, RS485Receive);
    // clean
    _s->flush();

    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    // do the same to debug channel
    _debug_s->println((uint8_t)sm.START_BYTE, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.NET_ADDR_1, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.DEV_ADDR_2, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.STATUS_1, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.STATUS_2, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.VALUE_1, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.VALUE_2, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.VALUE_3, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.VALUE_4, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.CRC_16_1, HEX);
    delayMicroseconds(5);
    _debug_s->println((uint8_t)sm.CRC_16_2, HEX);
    delayMicroseconds(5);

    delay(5);
    }
}