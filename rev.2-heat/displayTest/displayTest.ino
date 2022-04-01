/**
 * @file LCDemo7Segment.ino
 * @author Noa Sakurajin (noasakurajin@web.de)
 * @brief using the ledcontroller with 7-segment displays
 * @version 0.1
 * @date 2020-12-30
 *
 * @copyright Copyright (c) 2020
 *
 */

//TEST for thermostat PCB
//because this tests the buttons by outputting to serial the heat led is always on




//We always have to include the library
#include "LedController.hpp"

/*
 You might need to change the following 3 Variables depending on your board.
 pin 15 is connected to the DataIn
 pin 14 is connected to the CLK
 pin 13 is connected to LOAD/ChipSelect
*/
#define DIN 19
#define CS 5
#define CLK 18
#define OFF_LED 33
#define HEAT_LED 03
#define HEATING_LED 32
#define UP 25
#define DOWN 26
#define HEAT 23
#define RESET 27

/*
 Now we need a LedController Variable to work with.
 We have only a single MAX72XX so the Dimensions are 1,1.
 */
LedController<1,1> lc;

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void setup() {
  pinMode(OFF_LED, OUTPUT);
  pinMode(HEAT_LED, OUTPUT);
  pinMode(HEATING_LED, OUTPUT);
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(HEAT, INPUT);
  pinMode(RESET, INPUT);
  digitalWrite(OFF_LED, LOW);
  digitalWrite(HEAT_LED, LOW);
  digitalWrite(HEATING_LED, LOW);
  Serial.begin(115200);

  //Here a new LedController object is created without hardware SPI.
  lc=LedController<1,1>(DIN,CLK,CS);

  /* Set the brightness to a medium values */
  lc.setIntensity(12);
  /* and clear the display */
  lc.clearMatrix();
}


/*
 This method will display the characters for the
 word "Arduino" one after the other on digit 0.
 */
void writeArduinoOn7Segment() {
  lc.setChar(0,0,'a',false);
  delay(delaytime);
  lc.setRow(0,0,0x05);
  delay(delaytime);
  lc.setChar(0,0,'d',false);
  delay(delaytime);
  lc.setRow(0,0,0x1c);
  delay(delaytime);
  lc.setRow(0,0,B00010000);
  delay(delaytime);
  lc.setRow(0,0,0x15);
  delay(delaytime);
  lc.setRow(0,0,0x1D);
  delay(delaytime);
  lc.clearMatrix();
  delay(delaytime);
}

/*
  This method will scroll all the hexa-decimal
 numbers and letters on the display. You will need at least
 four 7-Segment digits. otherwise it won't really look that good.
 */
void scrollDigits() {
  for(int i=0; i<32; i++) {
    lc.setDigit(0,7,i,false);
    lc.setDigit(0,6,i+1,false);
    lc.setDigit(0,5,i+2,false);
    lc.setDigit(0,4,i+3,false);
    lc.setDigit(0,3,i+4,false);
    lc.setDigit(0,2,i+5,false);
    lc.setDigit(0,1,i+6,false);
    lc.setDigit(0,0,i+7,false);
    delay(delaytime);
  }
  lc.clearMatrix();
  delay(delaytime);
}

void loop() {
  writeArduinoOn7Segment();
  scrollDigits();
  
  digitalWrite(OFF_LED, HIGH);
  digitalWrite(HEAT_LED, HIGH);
  digitalWrite(HEATING_LED, HIGH);
  delay(500);
  digitalWrite(OFF_LED, LOW);
  digitalWrite(HEAT_LED, LOW);
  digitalWrite(HEATING_LED, LOW);
  delay(500);
  digitalWrite(OFF_LED, HIGH);
  digitalWrite(HEAT_LED, HIGH);
  digitalWrite(HEATING_LED, HIGH);
  delay(500);
  digitalWrite(OFF_LED, LOW);
  digitalWrite(HEAT_LED, LOW);
  digitalWrite(HEATING_LED, LOW);
  Serial.print(digitalRead(UP));
  Serial.print(digitalRead(DOWN));
  Serial.print(digitalRead(HEAT));
  Serial.println(digitalRead(RESET));
  
}
