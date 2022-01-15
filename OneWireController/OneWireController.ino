#include <OneWire.h>

#include "MAX14661.h"
/* MAX31850 Temperature chip i/o */
// From Amir : By pressing Switch S1 you can let the Arduino to scan through all channels and print out the data.
//             By pressing Switch S2 you can ask Arduino to scan one by one as you press it.
// From Amir : Note: the LED numbers do not match the binary code. So, D4 is LSB. if you see D4 is ON, that means code/channel 0x01 not 0x08
OneWire  ds(2);  // on pin 10
MAX14661 mux1(0x4C);  // 0x4C..0x4F
MAX14661 mux2(0x4D);
MAX14661 mux3(0x4E);
MAX14661 mux4(0x4F);
bool AutoScan = 0;

void setup(void) {
  Serial.begin(9600);
  
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  digitalWrite(A5,HIGH);    // set pullup for switches
  digitalWrite(A4,HIGH);    // set pullup for switches
  
  if (mux1.begin() == false)
    Serial.println("Could not find MAX14661 - 1");
  else
    mux1.openAllChannels();    

  if (mux2.begin() == false)
    Serial.println("Could not find MAX14661 - 2");
  else
    mux2.openAllChannels();
        
  if (mux3.begin() == false)
    Serial.println("Could not find MAX14661 - 3");
  else
    mux3.openAllChannels();
    
  if (mux4.begin() == false)
    Serial.println("Could not find MAX14661 - 4");
  else
    mux4.openAllChannels();      
}

void loop(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];  

  if(!digitalRead(A5))
  {
    if(AutoScan == 0) AutoScan = 1;
    else if(AutoScan == 1) AutoScan = 0;   
    delay(500); 
  }
  if(AutoScan || (!digitalRead(A4)))
  {
    if ( !ds.search(addr)) {
    Serial.print("No more addresses.\n");
    ds.reset_search();
    delay(250);
    return;
  }
  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }
  
  if ( addr[0] != 0x3B) {
      Serial.print("Device is not a MAX31850 family device.\n");
      return;
  }

  // The DallasTemperature library can do all this work for you!

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(700);               // From Amir : this was almost 1 second. I lowered to 700 ms.
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("P=");
  //Serial.print(present,HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print( OneWire::crc8( data, 8), HEX);
  //Serial.print(" ~~ ");
  
  Serial.print(" ~~ Temp ( ");  
  LED_LIGHTUP(data[4]&0x0f);      // From Amir : just writing the obtained sensor address to LEDs
  Serial.print(data[4]&0x0f);
  Serial.print(" )=");
  Serial.print(data[1]);          // From Amir : just showing the MSB byte for my own tests
  Serial.println();
  }
}

void LED_LIGHTUP(int code)
{
  PORTC = code;


}
