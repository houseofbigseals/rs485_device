
#include "DHTDevice.h"

void DHTDevice::do_your_duty()
{
    //as we can see in documentation to dht11
    // the min time between measures is 2 sec 
    // (otherwise the accuracy of the sensor will decrease significantly)
    // _dht handles this and if there is less then 2 sec from last measure
    // it returns last status of measure if there is not the time for new measure

    // update stored data in regs
    // if there is nan - dont touch last result
    // _dht keeps that nan

    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
       // _debug_s->println(F("DHTDevice: we start do duty"));
    }

    float ftemp = _dht.readTemperature(); // its not simple function
                                            // look at its implementation in lib
    if(!isnan(ftemp))
    {
        // i am not good at type conversion
        uint32_t t_temp = 0;
        // type casting
        *(float*)(&t_temp) = ftemp;
        _temp.data = t_temp;
    }
    float fhum = _dht.readHumidity();
    if(!isnan(fhum))
    {
        // i am not good at type conversion
        uint32_t t_hum = 0;
        // type casting
        *(float*)(&t_hum) = fhum;
        _hum.data = t_hum;
    }
}
DHTDevice::DHTDevice(uint8_t type, uint8_t pin, uint16_t uid, bool debug) : BaseDevice(uid, debug), _dht(pin, type)
{

    // start our dht object
    _dht.begin();
    // save some params
    _pin = pin;
    _type = type;

    // create regs
    _temp = Reg(
        d_sensor_data_addr,  //default addr of sensor register
        0x00000000, // default sensor state
        true,
        false,
        true
    );

    _hum = Reg(
        d_sensor_data_addr+1,  //next to default addr of sensor register
        0x00000000, // default sensor state
        true,
        false,
        true
    );
    // now lets recreate regs array
    regs_len = 3;
    // remove firstly-created array 
    delete [] regs;
    //  and create it again with new size
    regs = new Reg*[regs_len];
    regs[0] = &_uid;  // put here pointer for _uid
    regs[1] = &_temp;  // put here pointer for _temp
    regs[2] = &_hum;  // put here pointer for _hum
}


SlaveMessage DHTDevice::__private_execute(Reg* r, MasterMeassage* mmptr, bool isadmin)
{
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
        _debug_s->println(F("DHTDevice: we start __private_execute"));
    }
    // we will ignore admin flag for some time
    if(r->addr == d_device_data_addr)
    {
        return generate_success_sm(mmptr->COMMAND, r->data);   
    }
    if(r->addr == d_sensor_data_addr)
    {
        //r->data
        if(D_RS485_DEBUG_OUTPUT_ALLOWED)
        {
            _debug_s->println(F("DHTDevice: read temp"));
        }

        float ftemp = _dht.readTemperature(); // its not simple function
                                            // look at its implementation in lib

        if(isnan(ftemp))
        {
            if(D_RS485_DEBUG_OUTPUT_ALLOWED)
            {
                _debug_s->println(F("DHTDevice: error while read temp"));
            }
            // return hardware error
            return generate_error_sm(mmptr->COMMAND, d_hardware_register_execution_error);
        }
        else
        {
            if(D_RS485_DEBUG_OUTPUT_ALLOWED)
            {
                _debug_s->print(F("DHTDevice: temp is: "));
                _debug_s->println(ftemp);
            }
            // i am not good at type conversion
            uint32_t t_temp = 0;
            // type casting
            *(float*)(&t_temp) = ftemp;
            if(D_RS485_DEBUG_OUTPUT_ALLOWED)
            {
                _debug_s->print(F("DHTDevice: after conversion temp is: "));
                _debug_s->println(t_temp, HEX);
            }
            r->data = t_temp;
            // and send it back
            return generate_success_sm(mmptr->COMMAND, r->data);
        }
    }
    if(r->addr == d_sensor_data_addr + 1)
    {
        if(D_RS485_DEBUG_OUTPUT_ALLOWED)
        {
            _debug_s->println(F("DHTDevice: read hum"));
        }
        //r->data
        float fhum = _dht.readHumidity(); // its not simple function
                                            // look at its implementation in lib
        if(isnan(fhum))
        {
            if(D_RS485_DEBUG_OUTPUT_ALLOWED)
            {
                _debug_s->println(F("DHTDevice: error while read hum"));
            }
            // return hardware error
            return generate_error_sm(mmptr->COMMAND, d_hardware_register_execution_error);
        }
        else
        {
            if(D_RS485_DEBUG_OUTPUT_ALLOWED)
            {
                _debug_s->print(F("DHTDevice: hum is: "));
                _debug_s->println(fhum);
            }
            // i am not good at type conversion
            uint32_t t_hum = 0;
            // type casting
            *(float*)(&t_hum) = fhum;
            r->data = t_hum;
            // and send it back
            return generate_success_sm(mmptr->COMMAND, r->data);
        }
    }

}
