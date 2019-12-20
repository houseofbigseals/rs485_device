
#ifndef ONE_GPIO_DEVICE_H
#define ONE_GPIO_DEVICE_H

#include <Device.h>



class OneGpioDevice : public BaseDevice
{
    // simple device to set on-off one gpio pin
    // do we need all of this reg things to do it? - no
    // but we __want__ all this things
    public:
    OneGpioDevice(uint8_t pin, uint16_t uid, bool debug);
    //bool _d_set_pin(uint32_t _in_data, uint32_t * _out_data); // function to execute as reg function
    SlaveMessage __private_execute(Reg* r, MasterMeassage* mmptr, bool isadmin = false);
    protected:
    uint8_t _pin;
    uint8_t _d_default_relay_state = 0x01;
    Reg _gpio_pin;  // additional reg to keep gpio pin state
};

#endif