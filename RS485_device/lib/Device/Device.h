/* 
lib for device arduino wrappers
*/

#ifndef Device_h
#define Device_h

#include "Arduino.h"


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
    first byte is a command that was (or wasnt) executed
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

// commands
const uint8_t d_user_read = 0x01;
const uint8_t d_user_write = 0x02;
const uint8_t d_user_execute = 0x03;

const uint8_t d_admin_read = 0x31;
const uint8_t d_admin_write = 0x32;
const uint8_t d_admin_execute = 0x33;

//success codes
const uint8_t d_success = 0x00;

//error codes
const uint8_t d_access_denied = 0x10;
const uint8_t d_value_incorrect = 0x20;
const uint8_t d_no_such_register = 0x21;
const uint8_t d_crc_incorrect = 0x30;
const uint8_t d_incorrect_package = 0x31;
const uint8_t d_incorrect_command = 0x34;
const uint8_t d_unknown_error = 0x40;

// some default variables to construct messages
const uint8_t d_stub_value = 0x00;

// default register adresses
const uint16_t d_device_addr = 0x0001;
const uint16_t d_data_addr = 0x0002;


// some useful things

struct two_bytes  //disgusting
{
    uint8_t first;
    uint8_t last;
};

uint32_t d_unite4 (uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
uint16_t d_unite2 (uint8_t first, uint8_t last);
two_bytes d_split2 (uint16_t value);


// classes definitions

class MasterMeassage
{
    public:
    MasterMeassage(    
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
    public:
    SlaveMessage(
        uint8_t NET_ADDR_1_,
        uint8_t DEV_ADDR_2_,
        uint8_t VALUE_1_,
        uint8_t VALUE_2_,
        uint8_t VALUE_3_,
        uint8_t VALUE_4_,
        uint8_t STATUS_1_,
        uint8_t STATUS_2_
        );
        
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





class Protocol
{
    // this class realizes all methods to send and receive packages
    // also to create first checks of packages integrity 
    // and trying to find what device is a destination of this package
    
    public:
    int get_version();
    // int parse_package();
    bool send_master_message(MasterMeassage mm);
    bool send_slave_message(SlaveMessage sm);
    uint16_t parse_master_message(MasterMeassage mm);
    uint16_t parse_slave_message(SlaveMessage sm);
    Protocol();

    private:
    int _version;
};

class Reg
{
    /*
    This class comtain 4 bytes data_field, its own 2 byte address
    and a pointer to execution method, by default it is nullptr
    the method pointed to by the pointer should return uint32_t and take no arguments
    uint32_t (*exec_method_pointer)() = nullptr
    */

    public:
    Reg(
        uint16_t addr_ = 0x0000,
        uint32_t data_ = 0x00000000,
        bool readable_ = false,
        bool writable_ = false,
        bool executable_ = false,
        uint32_t (*exec_method_pointer_)() = nullptr
    );
    
    uint16_t addr;
    uint32_t data;
    bool readable;
    bool writable;
    bool executable;
    uint32_t (*exec_method_pointer)();
};

class BaseDevice
{
    /*
    Device is a passive state machine
    It have state registers 4 bytes long
    each register keeps some simple parameter of device working mode
    we can read and write to register
    also we can execute a function bound to the corresponding register
    all devices have function do_your_duty - it does current periodical work of this device 
    */

    // base device can only have uid and nothing more
    public:
    BaseDevice(uint16_t uid, Protocol *p_);
    SlaveMessage handle_mm(MasterMeassage mm); //returns error message or answer message
    SlaveMessage generate_error_sm(uint8_t command, uint8_t error_code); //generates default error sm
    SlaveMessage generate_success_sm(uint8_t command, uint32_t data);  // generates sm with this data
    void do_your_duty();  //tryes to do some uniq periodical routine 
    // register methods 
    SlaveMessage _read_register(Reg* r, bool isadmin = false);
    SlaveMessage _write_register(Reg* r, uint32_t new_data, bool isadmin = false);
    SlaveMessage _execute_register(Reg* r, bool isadmin = false);

    protected:
    // registers are private and we cant directly touch them from outside
    // we should use public functions
    // registers
    Reg _uid; // read-only
    int regs_len; // keeps number of registers
    Reg **regs; // keeps pointers for all registers, must be created dinamically in constructor
    Protocol *p;  // keeps pointer to current protocol

};

class CorrectTestDevice
{
    //public
};

#endif