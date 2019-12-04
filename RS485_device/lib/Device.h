/* 
lib for device arduino wrappers
*/

#ifndef Device_h
#define Device_h

#include "Arduino.h"


class MasterMeassage
{
    public:
    uint16_t DEV_ADDR;
    uint32_t VALUE;
    uint16_t REG_ADDR;
    uint8_t COMMAND;


};

class SlaveMessage
{

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
    int get_version();
    int parse_package();
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