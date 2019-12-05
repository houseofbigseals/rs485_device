/* 
lib for device arduino wrappers
*/

#ifndef Device_h
#define Device_h

#include "Arduino.h"


class MasterMeassage
{
    public:
    MasterMeassage(    
        uint8_t DEV_ADDR_1_,
        uint8_t DEV_ADDR_2_,
        uint8_t VALUE_1_,
        uint8_t VALUE_2_,
        uint8_t VALUE_3_,
        uint8_t VALUE_4_,
        uint8_t REG_ADDR_1_,
        uint8_t REG_ADDR_2_,
        uint8_t COMMAND_,
        uint8_t CRC_16_1_,
        uint8_t CRC_16_2_
        );

    uint8_t DEV_ADDR_1;
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
    public:
    SlaveMessage(
        uint8_t DEV_ADDR_1_,
        uint8_t DEV_ADDR_2_,
        uint8_t VALUE_1_,
        uint8_t VALUE_2_,
        uint8_t VALUE_3_,
        uint8_t VALUE_4_,
        uint8_t STATUS_1_,
        uint8_t STATUS_2_,
        uint8_t CRC_16_1_,
        uint8_t CRC_16_2_
        );
        
    uint8_t DEV_ADDR_1;
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


class Protocol
{
    /*
    package from master to device - 11 bytes
    
    | DEV_ADDR_LOW | DEV_ADDR_HIGH | COMMAND | REG_ADDR_LOW | REG_ADDR_HIGH |
    VALUE_1 | VALUE_2 | VALUE_3 | VALUE_4 | CRC16_LOW | CRC16_HIGH |

    DEV_ADDR - 2 bytes - to what device
    COMMAND - 1 byte - what to do
    REG_ADDR - 2 bytes - to what register
    VALUE - 4 bytes - what to write in that register
    CRC16 - 2 bytes - crc16 sum of package

    package from device to master - 10 bytes

    | DEV_ADDR_LOW | DEV_ADDR_HIGH | STATUS_LOW | STATUS_HIGH |
    VALUE_1 | VALUE_2 | VALUE_3 | VALUE_4 | CRC16_LOW | CRC16_HIGH |

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
    first byte is a command that was executed
    second byte is error message or success status

    0x00 : 0x0F - success execution codes
    0x00 - absolutely win

    0x10 : 0x1F - access error codes
    0x10 - access to register denied

    0x20 : 0x2F - execution error codes
    0x20 - value is incorrect (cant execute this conmmand with that register with that value)
    0x21 - no such register
    0x22 - 

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
    0xAA - we are in big unknown trouble (use only as flag of real big problem, like fatal error)
    0xFF - reserved

    */
    public:
    // commands
    const uint8_t user_read_command = 0x01;
    const uint8_t user_write_command = 0x02;
    const uint8_t user_execute_command = 0x03;

    const uint8_t admin_read_command = 0x31;
    const uint8_t admin_write_command = 0x32;
    const uint8_t admin_execute_command = 0x33;

    //success codes
    const uint8_t success = 0x00;

    //error codes
    const uint8_t access_denied = 0x10;
    const uint8_t value_incorrect = 0x20;
    const uint8_t no_such_register = 0x21;
    const uint8_t crc_incorrect = 0x30;
    const uint8_t incorrect_package = 0x31;
    const uint8_t incorrect_command = 0x34;
    const uint8_t unknown_error = 0x40;

    int get_version();
    int parse_package();
    bool send_master_message(MasterMeassage mm);
    bool send_slave_message(SlaveMessage sm);
    MasterMeassage parse_master_message(MasterMeassage mm);
    SlaveMessage parse_slave_message(SlaveMessage sm);
    
    Protocol();

    private:
    int _version;



};

class BaseDevice
{
    /*
    Device is a passive state machine
    It have state registers 2 bytes long
    each register keeps some simple parameter of device working mode
    we can read and write to register
    UID
    */
    public:
    BaseDevice(int uid);
    int get_uid();

    //int set_uid(); 

    private:
    int _uid;

};

#endif