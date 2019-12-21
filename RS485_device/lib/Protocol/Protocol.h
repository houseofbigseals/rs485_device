#ifndef Protocol_h
#define Protocol_h

#include <stdint.h>

// part for arduino serial debug
//#include <Arduino.h>
//const bool D_RS485_DEBUG_OUTPUT_ALLOWED = true;
//HardwareSerial * _debug_s = &Serial;


// ========================= pseudo documentation =============================

/*
    package from master to device - 12 bytes
    
    | START_BYTE | DEV_ADDR_LOW | DEV_ADDR_HIGH | COMMAND | REG_ADDR_LOW | REG_ADDR_HIGH |
    VALUE_1 | VALUE_2 | VALUE_3 | VALUE_4 | CRC16_LOW | CRC16_HIGH |

    START_BYTE - 1 byte - everytime is 0xCA
    DEV_ADDR - 2 bytes - to what device
    COMMAND - 1 byte - what to do
    REG_ADDR - 2 bytes - to what register
    VALUE - 4 bytes - what to write in that register
    CRC16 - 2 bytes - crc16 sum of package

    package from device to master - 11 bytes

    | START_BYTE | DEV_ADDR_LOW | DEV_ADDR_HIGH | STATUS_LOW | STATUS_HIGH |
    VALUE_1 | VALUE_2 | VALUE_3 | VALUE_4 | CRC16_LOW | CRC16_HIGH |

    START_BYTE - 1 byte - everytime is 0xCA
    DEV_ADDR - 2 bytes - to what device
    STATUS - 2 bytes - status of command execution
    VALUE - 4 bytes - result of command execution
    CRC16 - 2 bytes - crc16 sum of package


    user commands:
    0x01 - read from register
    0x02 - write to register
    0x03 - execute register

    debug commands:
    0x31 - read as admin
    0x32 - write as admin
    0x33 - execute as admin

    statuses:
    first byte is a command that was (or wasnt) executed
    second byte is error message or success status

    0x00 : 0x0F - success execution codes
    0xAA - absolutely win

    0x10 : 0x1F - access error codes
    0x10 - access to register denied

    0x20 : 0x2F - execution error codes
    0x20 - value is incorrect (cant execute this conmmand with that register with that value)
    0x21 - no such register
    0x22 - unknown error of register execution
    0x23 - hardware error of register execution

    0x30 : 0x3F - CRC sum and package format error codes
    0x30 - incorrect CRC sum
    0x31 - incorrect format of package, but we dont know where is error
    0x32 - package is too long
    0x33 - package is too short
    0x34 - incorrect command
    0x35 - 

    0x40 : 0x4F - unknown error codes
    0x40 - something went wrong

    other error codes:
    0xBB - we are in big unknown trouble (use only as flag of real big problem, like fatal error)
    0xFF - reserved

    */


// ========================= constants declaration =============================
// constants

const uint16_t d_system_version = 0x0001;

// commands
const uint8_t d_user_read = 0x01;
const uint8_t d_user_write = 0x02;
const uint8_t d_user_execute = 0x03;

const uint8_t d_admin_read = 0x31;  // can be used, but for tests and debug only
const uint8_t d_admin_write = 0x32;  // deprecated, dont use it without big need in tests and debug
const uint8_t d_admin_execute = 0x33;  // deprecated, never use it, this can lead to idiotic mistakes

//success codes
const uint8_t d_success = 0xAA;

//error codes
const uint8_t d_access_denied = 0x10;
const uint8_t d_value_incorrect = 0x20;
const uint8_t d_no_such_register = 0x21;
const uint8_t d_register_execution_error = 0x22;
const uint8_t d_hardware_register_execution_error = 0x23;
const uint8_t d_crc_incorrect = 0x30;
const uint8_t d_incorrect_package = 0x31;
const uint8_t d_incorrect_command = 0x34;
const uint8_t d_unknown_error = 0x40;

// some default variables to construct messages
const uint8_t d_stub_value = 0x00;
const uint8_t d_start_byte = 0xCA;
const uint8_t d_master_message_len = 12;
const uint8_t d_slave_message_len = 11;

// default register adresses
// yeah, disgusting
const uint16_t d_device_data_addr = 0x0010;  // 10-19
const uint16_t d_sensor_data_addr = 0x0020;  // 20-29
const uint16_t d_relay_data_addr = 0x0030;  // 30-39


// ========================= useful methods declaration =============================
// some useful methods and structs
struct two_bytes  //disgusting_s->write(sm.STATUS_1);
{
    uint8_t first;
    uint8_t last;
};

two_bytes d_split2 (uint16_t value);
uint32_t d_unite4 (uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
uint16_t d_unite2 (uint8_t first, uint8_t last);
uint16_t d_crc16 (uint8_t * pcBlock, uint16_t len);


// ========================= message classes declaration =============================
// classes

class MasterMeassage
{
    // class that simply keep all needed fields for master message and constructor
    public:
    MasterMeassage(
        // we have to fill all values in every construction of message
        // because there is no any default behavior
        // except start_byte - its always 0xCA
        // crc16 fields we have to not fill
        // they will we calculated inside of that object by internal method
        // from this library (uint16_t d_crc16 (uint8_t* start_ptr, int len))
        uint8_t NET_ADDR_1_,
        uint8_t DEV_ADDR_2_,
        uint8_t VALUE_1_,
        uint8_t VALUE_2_,
        uint8_t VALUE_3_,
        uint8_t VALUE_4_,
        uint8_t REG_ADDR_1_,
        uint8_t REG_ADDR_2_,
        uint8_t COMMAND_
        );

    uint8_t START_BYTE;
    uint8_t NET_ADDR_1;
    uint8_t DEV_ADDR_2;
    uint8_t VALUE_1;
    uint8_t VALUE_2;
    uint8_t VALUE_3;
    uint8_t VALUE_4;
    uint8_t REG_ADDR_1;
    uint8_t REG_ADDR_2;
    uint8_t COMMAND;
    uint8_t CRC_16_1;
    uint8_t CRC_16_2;
};

class SlaveMessage
{
    // class that simply keeps all needed fields for slave message and constructor
    public:
    SlaveMessage(
        // we have to fill all values in every construction of message
        // because there is no any default behavior
        // except start_byte - its always 0xCA
        // crc16 fields we have to not fill
        // they will be calculated inside of that object by some internal method

        uint8_t NET_ADDR_1_,
        uint8_t DEV_ADDR_2_,
        uint8_t VALUE_1_,
        uint8_t VALUE_2_,
        uint8_t VALUE_3_,
        uint8_t VALUE_4_,
        uint8_t STATUS_1_,
        uint8_t STATUS_2_
        );
    
    uint8_t START_BYTE;
    uint8_t NET_ADDR_1;
    uint8_t DEV_ADDR_2;
    uint8_t VALUE_1;
    uint8_t VALUE_2;
    uint8_t VALUE_3;
    uint8_t VALUE_4;
    uint8_t STATUS_1;
    uint8_t STATUS_2;
    uint8_t CRC_16_1;
    uint8_t CRC_16_2;

};

#endif


