/* ADPL_complete.ino
 *
 * This code includes all components - temp probes, level sensor, and relays
 * General overview of goals: Run pump based on tank level, operate burner based
 * on temp3 
 *
 * LICENSE: MIT (see LICENSE file)
 * 
 * Copyright (c) Aaron Forbis-Stokes and Mark Palmeri
 *
 */

#include "Valve.h"
// instantiate valve object on digital pin #4
Valve valve(4); 

#include "Pump.h"
// instatiate pump object on digital pin #7
Pump pump(7);

#include "TempProbe.h"
// instantiate temperature probe objects on analog pins A0-A4
TempProbe tempProbe1(A0);
TempProbe tempProbe2(A1);
TempProbe tempProbe3(A2); // might want to rename this object to tie it to ignitor func
TempProbe tempProbe4(A3);
TempProbe tempProbe5(A4);

//Code for SD card
//#include <SPI.h>
//#include "SD.h"
//#include <Wire.h>
//#include <OneWire.h>
//
////for SD card
//const int chipSelect = 10;   //pin 10 for SD card
//#define WAIT_TO_START 0      //start readings immediately

// ignitor variables
#define IGNITOR_PIN 2   // digital output pin #2
#define INCINERATE_LOW_TEMP 25
#define INCINERATE_HIGH_TEMP 28
const unsigned long ignite_delay = 900000;  // ms (15min); time between ignitor fires 
                                            // when valve is open
unsigned long last_ignite_time;             // time (ms) that will be returned from millis()
                                            // when ignitor last fired

// water level variables
#define LEVEL_PIN A5
#define LEVEL_MIN 400
#define LEVEL_MAX 1200
#define KEEP_PUMP_ON_TIME 30000     // ms; keep pump on for 5 min for intermediate level
#define KEEP_PUMP_OFF_TIME 330000   // ms; keep pump off for 55 min after 5 min on time

unsigned long current_time;
//
//File dataFile;  //SD card file name

void setup() {
    Serial.begin(9600);
    pinMode(LEVEL_PIN, INPUT);
    pinMode(IGNITOR_PIN, OUTPUT);
    // pinMode(10, OUTPUT); //SD card pin
    // dataFile = SD.open("datalog.txt", FILE_WRITE);

    // need to define the reference for the analog input pins (3.3 V), which is more 
    // stable than the default 5 V since we're not using any Wheatstone bridges
    // for each thermistor
    analogReference(EXTERNAL); 
}

void loop() { 
   
    // read probe temperatures
    tempProbe1.read();
    tempProbe2.read(); 
    tempProbe3.read(); 
    tempProbe4.read(); 
    tempProbe5.read(); 

    // temporary debugging serial print statements
    Serial.print(tempProbe1.temp);
    Serial.print(", ");
    Serial.print(tempProbe2.temp);
    Serial.print(", ");
    Serial.print(tempProbe3.temp);
    Serial.print(", ");
    Serial.print(tempProbe4.temp);
    Serial.print(", ");
    Serial.print(tempProbe5.temp);
    Serial.print(", ");
    Serial.print(valve.gasOn);
    Serial.print(", ");
    // Serial.print(IgnitorOn);
    // Serial.print(", ");
    Serial.println(pump.pumping);
 
    /* ==== Valve / Ignitor ====
    Gas valve open when the temperature is < 68 (INCINERATE_LOW_TEMP) and
    closed when the temperature > 72 (INCINERATE_HIGH_TEMP).  When the
    temperature drops below 72, valve will not open until below 68.  The
    ignitor will spark for 5 seconds every 15 minutes while the valve is open.
    */

    if (tempProbe3.temp <= INCINERATE_LOW_TEMP) {       
        valve.open();
        delay(10);
        fireIgnitor();
    }

    if(valve.gasOn == true) {

        current_time = millis();

        if (tempProbe3.temp >= INCINERATE_HIGH_TEMP) {
            valve.close();
        }
        // if 15 min have elapsed since last ignitor fire, then fire again
        else if(current_time > (last_ignite_time + ignite_delay)) {     
            fireIgnitor();
            last_ignite_time = millis();
        }
    }    
 
    /* ==== Water Level Pump ====
    Pump off when the level is <2", Remain on when >24". When the level is
    2"<x<24", the pump should be on for 5 minutes, off 55 minutes.  
    
    Sensor output is 4 mA at bottom (4") and 20 mA at top (24").
    Resistor is 237 Ohm

    TO DO: Values for comparison need to be updated for conversion to inches
    */
    float waterLevel = analogRead(LEVEL_PIN);
    Serial.println(waterLevel);

    if (waterLevel > LEVEL_MAX && !pump.pumping) {
        pump.turnOn();
    }
    else if(waterLevel > LEVEL_MIN && waterLevel <= LEVEL_MAX) {

        current_time = millis();

        if (!pump.pumping && (current_time - pump.offTime) > KEEP_PUMP_OFF_TIME) {
            pump.turnOn();
        }
        else if (pump.pumping) {
            if ((current_time - pump.onTime) > KEEP_PUMP_ON_TIME) {
                pump.turnOff();
            }
        }
    }   
 
    // SD card data collection commented out b/c connected to damaged Arduino
    // writeSDcard(TempProbe[0]);

} // end loop()


void fireIgnitor() {
    const int IGNITOR_ON_TIME = 5000;   // ms
    digitalWrite(IGNITOR_PIN, HIGH);
    delay(IGNITOR_ON_TIME);
    digitalWrite(IGNITOR_PIN, LOW);
}

/*
void writeSDcard(float TempProbe) {
    Serial.print(TempProbe);
    Serial.print(", ");
    dataFile.print(TempProbe);
    dataFile.print(", ");
}
*/
