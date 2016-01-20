/*
 * A simple single freq AD9850 Arduino test script
 * Original AD9851 DDS sketch by Andrew Smallbone at www.rocketnumbernine.com
 * Modified for testing the inexpensive AD9850 ebay DDS modules
 * Pictures and pinouts at nr8o.dhlpilotcentral.com
 * 9850 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9850.pdf
 * Use freely
 *
 * Modified for FR6989 LaunchPad to use LCD screen
 * Uses potentiometer to adjust frequency
 * Adjusts up to 4095 Hz
 * Frank Milburn  January 2016
 * Use freely
*/
 
#include "LCD_Launchpad.h"
 
#define   W_CLK     8       // connect to AD9850 module word load clock pin (CLK)
#define   FQ_UD     9       // connect to AD9850 freq update pin (FQ)
#define   DATA     10       // connect to AD9850 serial data load pin (DATA)
#define   RESET    11       // connect to AD9850 reset pin (RST).
#define   ADJ_FREQ  2       // analog pin tied to pot to adjust frequency
 
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
 
LCD_LAUNCHPAD lcd;          // remove if not using FR6989

void setup() {
  Serial.begin(115200);
  lcd.init();               // remove if not using FR6989
  
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
  
  analogReadResolution(12); // MCU dependent
 
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);         // this pulse enables serial mode - Datasheet page 12 figure 10
}
 
void loop() {
  int freq = analogRead(ADJ_FREQ);
  Serial.println(freq);
  lcd.clear();              // remove if not using FR6989
  lcd.println(freq);        // remove if not using FR6989
  delay(200);
  sendFrequency(freq);           
}
 
 // transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}
 
 // frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}
 
