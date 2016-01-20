/*
 * A simple single freq AD9850 Arduino test script
 * Original AD9851 DDS sketch by Andrew Smallbone at www.rocketnumbernine.com
 * Modified for testing the inexpensive AD9850 ebay DDS modules
 * Pictures and pinouts at nr8o.dhlpilotcentral.com
 * 9850 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9850.pdf
 * Use freely
 * 
 * Modified to display on FR6989 LaunchPad LCD
 * Frank Milburn  January 2016
 * Use freely
*/
#include "energia.h"

#if defined (__MSP430FR6989) 
  #include "LCD_Launchpad.h"
  LCD_LAUNCHPAD lcd;
#endif
 
#define   FREQ   4000       //     <-----  CHANGE THE FREQUENCY HERE
 
#define   W_CLK     3       // connect to AD9850 module word load clock pin (CLK)
#define   FQ_UD     4       // connect to AD9850 freq update pin (FQ)
#define   DATA      5       // connect to AD9850 serial data load pin (DATA)
#define   RESET     6       // connect to AD9850 reset pin (RST)
 
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

void setup() {
  Serial.begin(115200);
  
  #if defined (__MSP430FR6989)
    lcd.init();
  #endif    
  
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
  double freq = FREQ;
  
  Serial.println(freq);
  sendFrequency(freq); 
    
  
  #if defined (__MSP430FR6989)        // allows display on the LCD if using a FR6989 LaunchPad
    int single_freq = (int) freq;     
    lcd.clear();              
    if (freq >= 10000000) {
      single_freq = int (freq / 1000000);   // Displays MHz if 1,000,000 Hz or above
    }
    else if (freq >= 10000) {
      single_freq = (int) (freq / 1000);    // Displays KHz between 10,000 Hz and 1,000,000 Hz
    }
    else {                                  // Displays Hz if below 10,000
      single_freq = (int) freq;
    }
    lcd.println(single_freq);        
  #endif
  while (1){};  
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
