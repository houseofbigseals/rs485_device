
#ifndef D_DHT_DEVICE_H
#define D_DHT_DEVICE_H

#include <Device.h>
#include <DHT.h>

class DHTDevice : public BaseDevice
{
    // simple device to set on-off one gpio pin
    // do we need all of this reg things to do it? - no
    // but we __want__ all this things
    public:
    DHTDevice(uint8_t type, uint8_t pin, uint16_t uid, bool debug = false);
    void do_your_duty();
    //bool _d_set_pin(uint32_t _in_data, uint32_t * _out_data); // function to execute as reg function
    SlaveMessage __private_execute(Reg* r, MasterMeassage* mmptr, bool isadmin = false);
    protected:
    DHT _dht;
    uint8_t _pin;
    uint8_t _type;
    Reg _temp;  // additional reg to keep last temp measure result
    Reg _hum;  // additional reg to keep last hum measure result
    
};

#endif