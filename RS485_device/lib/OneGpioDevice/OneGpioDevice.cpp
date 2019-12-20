#include "OneGpioDevice.h"


SlaveMessage OneGpioDevice::__private_execute(Reg* r, MasterMeassage* mmptr, bool isadmin)
{
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->print(F("one_gpio_device: We start __private_execute register: "));
    _debug_s->println(r->addr);
    }
    // we will read only 4-th bute from all message and put it to digitalWrite
    digitalWrite(_pin, mmptr->VALUE_4);
    // put new value to r->data
    r->data = (uint32_t)(mmptr->VALUE_4);
    //anyway return mmptr->VALUE_4
    return generate_success_sm(mmptr->COMMAND, (uint32_t)(mmptr->VALUE_4));
}

OneGpioDevice::OneGpioDevice(uint8_t pin, uint16_t uid, bool debug) : BaseDevice(uid, debug)
{
    //pinMode pin as out
    pinMode(pin, OUTPUT);
    // set up default relay state
    digitalWrite(pin, _d_default_relay_state);
    // save pin as secret internal value 
    _pin = pin;
    // init _uid register read-only
    /*_uid = Reg(
        d_device_data_addr,  //default addr of _uid register
        uid,
        true,
        false,
        false
    );*/
    _gpio_pin = Reg(
        d_relay_data_addr,  //default addr of relay register
        _d_default_relay_state, // default relay state
        true,
        false,
        true
    );
    // now lets recreate regs array
    regs_len = 2;
    // remove firstly-created array 
    // yeah it disgusting
    // we have to purified code somehow
    delete [] regs;
    //  and create it again with new size
    regs = new Reg*[regs_len];
    regs[0] = &_uid;  // put here pointer for _uid
    regs[1] = &_gpio_pin;  // put here pointer for _gpio_pin
    // thats all folks
    //D_RS485_DEBUG_OUTPUT_ALLOWED = debug;
}