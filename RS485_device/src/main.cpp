


#include <Arduino.h>

// this is a flag to add all debug code outputs in  <SystemKeeper.h> and <Device.h>
// it must be defined before including of systemkeeper
// or there will be no debug
#include <Protocol.h>


#include <SystemKeeper.h>

// software serial init
uint8_t rx_pin = 12;
uint8_t tx_pin = 13;
uint8_t ss_mode_pin = 3;


SoftwareSerial myserial = SoftwareSerial(rx_pin, tx_pin);

//devices init
BaseDevice dev1 = BaseDevice(0x0020, true);
BaseDevice dev2 = BaseDevice(0x0021, true);
BaseDevice dev3 = BaseDevice(0x0022, true);
BaseDevice dev4 = BaseDevice(0x0023, true);
BaseDevice dev5 = BaseDevice(0x0024, true);
BaseDevice dev6 = BaseDevice(0x0025, true);
BaseDevice* devarr[6] = {&dev1, &dev2, &dev3, &dev4, &dev5, &dev6};
SystemKeeper sk = SystemKeeper(devarr, 6, &myserial, ss_mode_pin, true);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Start of init");
  
  myserial.begin(9600);
  pinMode(ss_mode_pin, OUTPUT);
  digitalWrite(ss_mode_pin, RS485Receive);
  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
}


void loop()
{
  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, HIGH);
  //Serial.println("YOLO");
  // wait for a second
  //delay(1000);
  // turn the LED off by making the voltage LOW
  //digitalWrite(LED_BUILTIN, LOW);
  sk.do_main_loop();
   // wait for a second
  //delay(1000);
}