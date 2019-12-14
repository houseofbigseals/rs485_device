#ifndef SystemKeeper_h
#define SystemKeeper_h

//#include <Protocol.h>
#include <Device.h>

// ========================= SystemKeeper class declaration =============================

class SystemKeeper
{
    // this class realizes all methods to check and verify packages
    //
    // also it contains pointers to all devices
    //
    // it can create first checks of packages integrity 
    // and trying to find what device is a destination of this package
    // if it cant find desired device or package is broken it generates correspond error message
    // if it can, it creates master message and put it to correspond device
    // then it takes back slave message from device and sends it to master
    
    public:
    int get_version();
    // int parse_package();
    //bool send_master_message(MasterMeassage mm);
    //bool send_slave_message(SlaveMessage sm);
    uint16_t parse_master_message(MasterMeassage mm);
    uint16_t parse_slave_message(SlaveMessage sm);
    SystemKeeper();

    private:
    int _version;
};
 #endif