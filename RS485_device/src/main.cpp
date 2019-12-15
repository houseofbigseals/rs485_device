#include <Arduino.h>
#include <SystemKeeper.h>

BaseDevice dev1 = BaseDevice(0x0020);
BaseDevice dev2 = BaseDevice(0x0021);
BaseDevice dev3 = BaseDevice(0x0022);
BaseDevice* devarr[3] = {&dev1, &dev2, &dev3};
SystemKeeper p = SystemKeeper(devarr, 3, &Serial);

void setup() 
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
}


void loop()
{
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  //Serial.println("YOLO");
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
  p.do_main_loop();
   // wait for a second
  delay(1000);
}