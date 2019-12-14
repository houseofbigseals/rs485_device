/* 
lib for device arduino wrappers
*/

#ifndef Device_h
#define Device_h

//#include <Arduino.h>
#include <Protocol.h>

// classes definitions

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
    BaseDevice(uint16_t uid);
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
    //
    int regs_len; // keeps number of registers
    Reg **regs; // keeps pointers for all registers, must be created dinamically in constructor

};

class CorrectTestDevice
{
    //public
};

#endif