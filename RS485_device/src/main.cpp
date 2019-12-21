


#include <Arduino.h>

// this is a flag to add all debug code outputs in  <SystemKeeper.h> and <Device.h>
// it must be defined before including of systemkeeper
// or there will be no debug
#include <Protocol.h>


#include <SystemKeeper.h>
#include <OneGpioDevice.h>
#include <DHTDevice.h>

// software serial init
uint8_t rx_pin = 12;
uint8_t tx_pin = 13;
uint8_t ss_mode_pin = 3;

bool d_global_debug = false;
SoftwareSerial myserial = SoftwareSerial(rx_pin, tx_pin);

//devices init
BaseDevice dev1 = BaseDevice(0x0120, d_global_debug);

BaseDevice dev2 = BaseDevice(0x0121, d_global_debug);
BaseDevice dev3 = BaseDevice(0x0122, d_global_debug);
BaseDevice dev4 = BaseDevice(0x0123, d_global_debug);
OneGpioDevice dev5 = OneGpioDevice(4, 0x0124, d_global_debug);
OneGpioDevice dev6 = OneGpioDevice(5, 0x0125, d_global_debug);
DHTDevice dev7 = DHTDevice(DHT11, 6, 0x0126, d_global_debug);

const uint8_t device_len = 7;
BaseDevice* devarr[device_len] = {&dev1, &dev2, &dev3, &dev4, &dev5, &dev6, &dev7};
SystemKeeper sk = SystemKeeper(devarr, device_len, &myserial, ss_mode_pin, true);

/*BaseDevice* devarr[1] = {&dev1};
SystemKeeper sk = SystemKeeper(devarr, 1, &myserial, ss_mode_pin, true);
*/
void setup() 
{
  if(d_global_debug)
  {
    Serial.begin(9600);
    Serial.println(F("Start of init"));
  }
  
  myserial.begin(57600);
  pinMode(ss_mode_pin, OUTPUT);
  digitalWrite(ss_mode_pin, RS485Receive);
  //pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
}


void loop()
{
  sk.do_main_loop();
}