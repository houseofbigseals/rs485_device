#ifndef SystemKeeper_h
#define SystemKeeper_h

#include <Device.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// ========================= SystemKeeper class declaration =============================

class SystemKeeper
{
    // this class realizes all methods to check and verify packages
    //
    // also it contains pointers to all devices
    //
    // it can create first checks of packages integrity
    // and trying to find what device is a destination of this package
    // if it cant find desired device or package is broken it does nothing
    // if it can, it creates master message and put it to correspond device
    // then it takes back slave message from device and sends it to master
    
    public:
    void parse_package(uint8_t * package, uint16_t len);
    //void send_master_message(MasterMeassage mm);
    void send_slave_message(SlaveMessage sm);
    SlaveMessage parse_master_message(MasterMeassage mm);
    //void parse_slave_message(SlaveMessage sm);
    SystemKeeper(BaseDevice **device_pointer_, uint16_t devices_number_, HardwareSerial *s);
    void do_main_loop();

    protected:
    BaseDevice **_device_pointer; // pointer to array of pointers to devices 
    uint16_t _devices_number; // number of devices
    HardwareSerial * _s; // pointer to serial object
};
 #endif