/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where can do changes and look to the results     */
/****************************************************/

/**********************************************************/
/*                            S S W                       */
/*  a b c d e f g 6 5 4 3 2 1 2 1 h   Segments & Grids    */
/*  o o o o o o o o o o o o o o o o   Pins of display     */
/*                                                        */
/* Symbols 2, Symbols 1 and Wheels 3 pins at right side   */
/**********************************************************/
/*ATT:The model SC6928B is not equal to SC6928 or AP6928, */
/* the firstuse 16bits memory map and the second use      */
/*12bits to fill  memory map.                             */
/**********************************************************/
//set your clock speed
#define F_CPU 16000000UL
//these are the include files. They are outside the project folder
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Standard Input/Output functions 1284
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/atomic.h>
#include <stdbool.h>
#include "stdint.h"
#include <Arduino.h>

#define AP6928_in 7// If 0 write LCD, if 1 read of LCD
#define AP6928_clk 8 // if 0 is a command, if 1 is a data0
#define AP6928_stb 9 // Must be pulsed to LCD fetch data of bus
#define panelLED 10// pin to control the LED presente as side of Display.
#define out2 2 //Pin 2
#define out3 3 //Pin 3 
#define out4 4 //Pin 4

#define AdjustPins    PIND // before is C, but I'm use port C to VFC Controle signals

unsigned char DigitTo7SegEncoder(unsigned char digit, unsigned char common);

/*Global Variables Declarations*/
unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char minute = 0;
unsigned char secs=0;
unsigned char seconds=0;
unsigned char milisec = 0;

unsigned char memory_secs=0;
unsigned char memory_minutes=0;

unsigned char number;
unsigned char numberA0 =0x00;
unsigned char numberA1 =0x00;
unsigned char numberB0 =0x00;
unsigned char numberB1 =0x00;
unsigned char numberC0 =0x00;
unsigned char numberC1 =0x00;
unsigned char numberD0 =0x00;
unsigned char numberD1 =0x00;
unsigned char numberE0 =0x00;
unsigned char numberE1 =0x00;
unsigned char numberF0 =0x00;
unsigned char numberF1 =0x00;

unsigned char digit=0;
unsigned char grid=0;
unsigned char gridSegments = 0b00000011; // Here I define the number of GRIDs and Segments I'm using

boolean flag=true;
boolean flagSecs=false; // Used in case of using buttons to setting clock, set buttons to do it!


unsigned int segOR[14];  // 7*2 result as 14 positions of map memory to fill table(0b1100xxxx) where x is to address of one grid in normal use!!!
unsigned int wd0 =0x00;
unsigned int wd1 =0x00;
unsigned char segmWheels = 0x00;


//Array to run the Wheels
unsigned int wheels[]={'f', 'e', 'c', 'd', 'a', 'b'}; // Att: The sequence is already like the wheels is contructed!
//Arrays of bits to Digits
unsigned int segNum0[] ={
//0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "g" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000001), // "f" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000001), // "e" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000001), // "d" of all digits  // Symbol PBC
         (0b0000000000000001), // "c" of all digits  // Symbol Play
         (0b0000000000000001), // "b" of all digits  // Symbol Pause
         (0b0000000000000001)  // "a" of all digits  // Symbol Wheel
};
unsigned int segNum1[] ={
//1        (hgfedcba) // Position of GRIDs
         (0b0000000000000000), // "g" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000000), // "f" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000000), // "e" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000000), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "c" of all digits// Symbol Play
         (0b0000000000000001), // "b" of all digits// Symbol Pause
         (0b0000000000000000)  // "a" of all digits // Symbol Wheel
};
unsigned int segNum2[] ={         
//2        (7654321) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000000), // "e" of all digits// Symbol Play
         (0b0000000000000001), // "f" of all digits// Symbol Pause
         (0b0000000000000001) // "g" of all digits // Symbol Wheel
};
unsigned int segNum3[] ={
//3        (57643210) // Position of GRIDs
         (0b0000000000000001), // "g" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000000), // "f" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000000), // "e" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "c" of all digits// Symbol Play
         (0b0000000000000001), // "b" of all digits// Symbol Pause
         (0b0000000000000001)  // "a" of all digits // Symbol Wheel
};
unsigned int segNum4[] ={
//4        (57643210) // Position of GRIDs
         (0b0000000000000001), // "g" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000001), // "f" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000000), // "e" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000000), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "c" of all digits// Symbol Play
         (0b0000000000000001), // "b" of all digits// Symbol Pause
         (0b0000000000000000)  // "a" of all digits // Symbol Wheel
};
unsigned int segNum5[] ={
//5        (57643210) // Position of GRIDs
         (0b0000000000000001), // "g" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000001), // "f" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000000), // "e" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "c" of all digits// Symbol Play
         (0b0000000000000000), // "b" of all digits// Symbol Pause
         (0b0000000000000001)  // "a" of all digits // Symbol Wheel
};
unsigned int segNum6[] ={
//6        (57643210) // Position of GRIDs
         (0b0000000000000001), // "g" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000001), // "f" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "e" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "c" of all digits// Symbol Play
         (0b0000000000000000), // "b" of all digits// Symbol Pause
         (0b0000000000000001)  // "a" of all digits // Symbol Wheel
};
unsigned int segNum7[] ={
//7        (57643210) // Position of GRIDs
         (0b0000000000000000), // "g" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000000), // "f" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000000), // "e" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000000), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "c" of all digits// Symbol Play
         (0b0000000000000001), // "b" of all digits// Symbol Pause
         (0b0000000000000001)  // "a" of all digits // Symbol Wheel
};
unsigned int segNum8[] ={
//8        (57643210) // Position of GRIDs
         (0b0000000000000001), // "g" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000001), // "f" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000001), // "e" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000001), // "d" of all digits  // Symbol PBC
         (0b0000000000000001), // "c" of all digits  // Symbol Play
         (0b0000000000000001), // "b" of all digits  // Symbol Pause
         (0b0000000000000001)  // "a" of all digits  // Symbol Wheel
};
unsigned int segNum9[] ={
//9        (57643210) // Position of GRIDs
         (0b0000000000000001), // "g" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000001), // "f" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "e" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000001), // "c" of all digits  // Symbol Play
         (0b0000000000000001), // "b" of all digits  // Symbol Pause
         (0b0000000000000001)  // "a" of all digits  // Symbol Wheel
 };
//Array of bits to Wheels
unsigned int WheelsA[] ={
//0         (7654321) // Position of GRIDs
         (0b0000000001000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsB[] ={
//0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000001000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsC[] ={
//0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000001000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsD[] ={
//0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000001000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsE[] ={
//0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000001000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsF[] ={
//0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000001000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
//
void setup() {
// put your setup code here, to run once:

// initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  seconds = 0x00;
  minutes =0x00;
  hours = 0x00;

  /*CS12  CS11 CS10 DESCRIPTION
  0        0     0  Timer/Counter1 Disabled 
  0        0     1  No Prescaling
  0        1     0  Clock / 8
  0        1     1  Clock / 64
  1        0     0  Clock / 256
  1        0     1  Clock / 1024
  1        1     0  External clock source on T1 pin, Clock on Falling edge
  1        1     1  External clock source on T1 pin, Clock on rising edge
 */
  // initialize timer1 
  cli();           // disable all interrupts
  //initialize timer1 
  //noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;// This initialisations is very important, to have sure the trigger take place!!!
  
  TCNT1  = 0;
  
  // Use 62499 to generate a cycle of 1 sex 2 X 0.5 Secs (16MHz / (2*256*(1+62449) = 0.5
  OCR1A = 62498;  // compare match register 16MHz/256/2Hz
  //OCR1A = 1200; // only to use in test, increment seconds to fast!
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= ((1 << CS12) | (0 << CS11) | (0 << CS10));    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

// Note: this counts is done to a Arduino 1 with Atmega 328... Is possible you need adjust
// a little the value 62499 upper or lower if the clock have a delay or advance on hours.
   
//  a=0x33;
//  b=0x01;

CLKPR=(0x80);
//Set PORT
DDRD = 0xFF;  // IMPORTANT: from pin 0 to 7 is port D, from pin 8 to 13 is port B
PORTD=0x00;
DDRB =0xFF;
PORTB =0x00;

AP6928_init();

clear_AP6928();

//only here I active the enable of interrupts to allow run the test of AP6928
//interrupts();             // enable all interrupts
sei();
}
void AP6928_init(void){
  delayMicroseconds(200); //power_up delay
  // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!

  // Configure AP6928 display (grids)
  cmd_with_stb(gridSegments); // cmd 1 // AP6928 is driver until 7 grids
  delayMicroseconds(1);
  // Write to memory display, increment address, normal operation
  cmd_with_stb(0b01000000);//(BIN(01000000));
  delayMicroseconds(1);
  // Address 00H - 15H ( total of 11*2Bytes=176 Bits)
  cmd_with_stb(0b11000000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // set DIMM/PWM to value
  cmd_with_stb((0b10001000) | 7);//0 min - 7 max  )(0b01010000)
  delayMicroseconds(1);
}
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char transmit = 7; //define our transmit pin
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
         for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
           digitalWrite(AP6928_clk, LOW);
                 if (data & mask){ // if bitwise AND resolves to true
                    digitalWrite(AP6928_in, HIGH);
                 }
                 else{ //if bitwise and resolves to false
                   digitalWrite(AP6928_in, LOW);
                 }
          delayMicroseconds(5);
          digitalWrite(AP6928_clk, HIGH);
          delayMicroseconds(5);
         }
   //digitalWrite(AP6928_clk, LOW);
}
void cmd_with_stb(unsigned char a){
  // send with stb
  unsigned char transmit = 7; //define our transmit pin
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(AP6928_stb, LOW);
  delayMicroseconds(1);
         for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
           digitalWrite(AP6928_clk, LOW);
           delayMicroseconds(1);
                 if (data & mask){ // if bitwise AND resolves to true
                    digitalWrite(AP6928_in, HIGH);
                 }
                 else{ //if bitwise and resolves to false
                   digitalWrite(AP6928_in, LOW);
                 }
          digitalWrite(AP6928_clk, HIGH);
          delayMicroseconds(1);
         }
   digitalWrite(AP6928_stb, HIGH);
   delayMicroseconds(1);
}
void test_WheelsOFF(void){
//where: 
//Command 1: Display Mode Setting Command //With Strobe impulse
//Command 2: Data Setting Command         //With Strobe impulse
//Command 3: Address Setting Command      //Without Strobe impulse
  digitalWrite(AP6928_stb, LOW);
  delayMicroseconds(1);
  cmd_with_stb(gridSegments); // cmd 1 // AP6928 is a drive of 7 grids
  cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
             //
                          digitalWrite(AP6928_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                            //
                            cmd_without_stb(0b00000000); // "a" of all digits and symbol repeat
                            cmd_without_stb(0b00000000); // Symbol DVD
                            
                            cmd_without_stb(0b00000000); // "b" of all digits and symbol wheel 
                            cmd_without_stb(0b00000000); // Symbol VCD
                            
                            cmd_without_stb(0b00000000); // "c" of all digits and symbol wheel
                            cmd_without_stb(0b00000000); // Symbol MP3
                            
                            cmd_without_stb(0b00000000); // "d" of all digits 
                            cmd_without_stb(0b00000000); // Symbol PBC
                            
                            cmd_without_stb(0b00000000); // "e" of all digits
                            cmd_without_stb(0b00000000); // Symbol Play
                            
                            cmd_without_stb(0b00000000); // "f" of all digits
                            cmd_without_stb(0b00000000); // Symbol Pause
                            
                            cmd_without_stb(0b00000000); // "g" of all digits
                            cmd_without_stb(0b00000000); // Symbol Wheel
                            digitalWrite(AP6928_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(200);
}
void test_WheelsON(void){
//where: 
//Command 1: Display Mode Setting Command //With Strobe impulse
//Command 2: Data Setting Command         //With Strobe impulse
//Command 3: Address Setting Command      //Without Strobe impulse
  digitalWrite(AP6928_stb, LOW);
  delayMicroseconds(1);
  cmd_with_stb(gridSegments); // cmd 1 // AP6928 is a drive of 7 grids
  cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
             //
                          digitalWrite(AP6928_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                            //
                            cmd_without_stb(0b01000000); // "a" of all digits and symbol repeat
                            cmd_without_stb(0b00000000); // Symbol DVD
                            
                            cmd_without_stb(0b01000000); // "b" of all digits and symbol wheel 
                            cmd_without_stb(0b00000000); // Symbol VCD
                            
                            cmd_without_stb(0b01000000); // "c" of all digits and symbol wheel
                            cmd_without_stb(0b00000000); // Symbol MP3
                            
                            cmd_without_stb(0b01000000); // "d" of all digits 
                            cmd_without_stb(0b00000000); // Symbol PBC
                            
                            cmd_without_stb(0b01000000); // "e" of all digits
                            cmd_without_stb(0b00000000); // Symbol Play
                            
                            cmd_without_stb(0b01000000); // "f" of all digits
                            cmd_without_stb(0b00000000); // Symbol Pause
                            
                            cmd_without_stb(0b01000000); // "g" of all digits
                            cmd_without_stb(0b00000000); // Symbol Wheel
                            digitalWrite(AP6928_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(100);
}
void test_AllON(void){
//where: 
//Command 1: Display Mode Setting Command //With Strobe impulse
//Command 2: Data Setting Command         //With Strobe impulse
//Command 3: Address Setting Command      //Without Strobe impulse
  digitalWrite(AP6928_stb, LOW);
  delayMicroseconds(1);
  cmd_with_stb(gridSegments); // cmd 1 // AP6928 is a drive of 7 grids
  cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
             //
                          digitalWrite(AP6928_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                            //
                            cmd_without_stb(0b11111110); // "a" of all digits and symbol repeat
                            cmd_without_stb(0b00000111); // Symbol DVD
                            
                            cmd_without_stb(0b11111110); // "b" of all digits and symbol wheel 
                            cmd_without_stb(0b00000111); // Symbol VCD
                            
                            cmd_without_stb(0b11111110); // "c" of all digits and symbol wheel
                            cmd_without_stb(0b00000011); // Symbol MP3
                            
                            cmd_without_stb(0b11111110); // "d" of all digits 
                            cmd_without_stb(0b00000011); // Symbol PBC
                            
                            cmd_without_stb(0b11111110); // "e" of all digits
                            cmd_without_stb(0b00000011); // Symbol Play
                            
                            cmd_without_stb(0b11111110); // "f" of all digits
                            cmd_without_stb(0b00000011); // Symbol Pause
                            
                            cmd_without_stb(0b11111110); // "g" of all digits
                            cmd_without_stb(0b00000011); // Symbol Wheel
                            digitalWrite(AP6928_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(100);
}
void test_GRID(void){
//where: 
//Command 1: Display Mode Setting Command //With Strobe impulse
//Command 2: Data Setting Command         //With Strobe impulse
//Command 3: Address Setting Command      //Without Strobe impulse
unsigned char word1 =0x00;
unsigned char word0 =0x00;
              for (unsigned int i=512; i>0; i=(i>>1)){
                //Serial.println(i, HEX);
                digitalWrite(AP6928_stb, LOW);
                delayMicroseconds(1);
                cmd_with_stb(gridSegments); // cmd 1 // AP6928 is a drive of 7 grids
                cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
                           //                          
                          digitalWrite(AP6928_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this output pins with segments
                            //               (..gfedcba)  // Position of GRIDs swapped to gain more digits on a IC with only 7 digits, segments give more of 7 digits
                             word1 = (i & 0xff00UL) >>  8;
                             word0 = (i & 0x00ffUL) >>  0;
                             Serial.print("WD0= "); Serial.print(word0, HEX);
                             Serial.print(", WD1= ");Serial.println(word1, HEX);
                            cmd_without_stb(word0); // Group Low of bits of Digit
                            cmd_without_stb(word1); // Group High of bits of Digit
                            digitalWrite(AP6928_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(800);
             }
}
void test_panel_DVD(void){
  for (int x=0; x< 10; x++){
    digit=x;
          //
          for (int i=0; i< 10;i++){
              delayMicroseconds(1);
              cmd_with_stb(gridSegments); // cmd 1 // AP6928 is a driver of 7 grids
              cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
              digitalWrite(AP6928_stb, LOW);
              delayMicroseconds(1);
              cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
            //                    
                                switch (i) {
                                  case 0: 
                                        for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum0[v]<<digit);
                                        }break;
                                  case 1: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum1[v]<<digit);
                                        }break;
                                  case 2:
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum2[v]<<digit);
                                        } break;
                                  case 3: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum3[v]<<digit);
                                        }break;
                                  case 4: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum4[v]<<digit);
                                        }break;
                                  case 5:
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum5[v]<<digit);
                                        }break;
                                  case 6: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum6[v]<<digit);
                                        }break;
                                  case 7: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum7[v]<<digit);
                                        }break;
                                  case 8: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum8[v]<<digit);
                                        }break;
                                  case 9:
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum9[v]<<digit);
                                        } break;
                                  default:break;
                                }  
                  digitalWrite(AP6928_stb, HIGH);
                  cmd_with_stb((0b10001000) | 7); //cmd 4
                 delay(100);
          }
  }
}
void write_panel_DVD(unsigned char digit, unsigned char grid){         
                                switch (digit) {
                                  case 0: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum0[v]<<grid));
                                        }break;
                                  case 1: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum1[v]<<grid));
                                        }break;
                                  case 2:
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum2[v]<<grid));
                                        } break;
                                  case 3: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum3[v]<<grid));
                                        }break;
                                  case 4: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum4[v]<<grid));
                                        }break;
                                  case 5:
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum5[v]<<grid));
                                        }break;
                                  case 6: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum6[v]<<grid));
                                        }break;
                                  case 7: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum7[v]<<grid));
                                        }break;
                                  case 8: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum8[v]<<grid));
                                        }break;
                                  case 9:
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum9[v]<<grid));
                                        } break;
                                  default:break;
                                } 
                                if (grid == 1){  //This define wich RUN only one time by update, can be any number of grid(segmente on this solution) 0~9.
                                  //Serial.print(grid); Serial.println(" Grelha");
                                  delayMicroseconds(1);
                                  cmd_with_stb(gridSegments); // cmd 1 // AP6928 is a driver of 7 grids
                                  cmd_with_stb(0b10000000); // cmd 2 //Normal operation; Set pulse as 1/16
                                  digitalWrite(AP6928_stb, LOW);
                                  delayMicroseconds(1);
                                  cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                                        for(int v=0; v<7; v++){
                                           wd0 = (segOR[v] & 0x00ffUL) >>  0;
                                           wd1 = (segOR[v] & 0xff00UL) >>  8;
                                           //Serial.print("Wd0: "), Serial.print(wd0, BIN);Serial.print(", Wd1: "), Serial.println(wd1, BIN);
                                        cmd_without_stb(wd0); //Send all the table of data to the map of driver (00~0D)
                                        cmd_without_stb(wd1); //Send all the table of data to the map of driver (00~0D)
                                        //Serial.println(segOR[v], BIN);
                                        }
                                  digitalWrite(AP6928_stb, HIGH);
                                  cmd_with_stb((0b10001000) | 7); //cmd 4
                                  delay(10);
                                        for(int v=0; v<14; v++){
                                        segOR[v]=0b00000000;  //Clear of contents of array table
                                        }
                                }
}
void clear_AP6928(void){
  /*
  Here I clean all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      for (int n=0; n < 14; n++){  // important be 10, if not, bright the half of wells./this on the AP6928 of 6 grids)
        //cmd_with_stb(gridSegments); // cmd 1 // AP6928 is fixed to 5 grids
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(AP6928_stb, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | n); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b00000000); // Data to fill table of 7 displays of 7segments, 
            //
            //cmd_with_stb((0b10001000) | 7); //cmd 4 set the bright//On SM1628 let the segments "a" of some dsp ON, check!!!
            digitalWrite(AP6928_stb, HIGH);
            delayMicroseconds(1);
     }
}
/******************************************************************/
/************************** Update Clock **************************/
/******************************************************************/
void send_update_clock(void){
    if (secs >=60){
      secs =0;
      minutes++;
      minute++;
    }
    if (minutes >=60){
      minutes =0;
      minute =0;
      hours++;
    }
    if (hours >=24){
      hours =0;
    }
  
    //*************************************************************
    numberA0=DigitTo7SegEncoder(secs%10);
    numberB0=DigitTo7SegEncoder(secs/10);
    //*************************************************************
    numberC0=DigitTo7SegEncoder(minute%10);
    numberD0=DigitTo7SegEncoder(minute/10);
    //**************************************************************
    numberE0=DigitTo7SegEncoder(hours%10);
    numberF0=DigitTo7SegEncoder(hours/10);
    //**************************************************************
}
void updateDisplay(void){
  //Note: This sequence is to a panel with AP6928 and 6 DSP de 7segm e 1 de digito 1
  //wheels and 8 predefined symbols. ("1.30.2 DP903002R" on PCB panel)
    grid=3;
    write_panel_DVD( numberA0, grid);// 
    grid=2;
    write_panel_DVD( numberB0, grid);// 
    grid=1;
    write_panel_DVD( numberC0, grid);// 
    grid=7;
    write_panel_DVD( numberD0, grid);// 
    grid=8;
    write_panel_DVD( numberE0, grid);// 
    grid=9;
    write_panel_DVD( numberF0, grid);// 
}
unsigned char DigitTo7SegEncoder( unsigned char digit){
  // Note the array (segments[]) to draw the numbers is with 2 bytes!!! 20 chars, extract 2 by 2 the number you need. 
  switch(digit){
    case 0:   number=0;      break;  // if remove the LongX, need put here the segments[x]
    case 1:   number=1;      break;
    case 2:   number=2;      break;
    case 3:   number=3;      break;
    case 4:   number=4;      break;
    case 5:   number=5;      break;
    case 6:   number=6;      break;
    case 7:   number=7;      break;
    case 8:   number=8;      break;
    case 9:   number=9;      break;
  }
  return number;
} 
void adjustHMS(){
 // Important is necessary put a pull-up resistor to the VCC(+5VDC) to this pins (3, 4, 5)
 // if dont want adjust of the time comment the call of function on the loop
  /* Reset Seconds to 00 Pin number 3 Switch to GND*/
    if((AdjustPins & 0x08) == 0 )
    {
      _delay_ms(200);
      secs=00;
    }
    
    /* Set Minutes when SegCntrl Pin 4 Switch is Pressed*/
    if((AdjustPins & 0x10) == 0 )
    {
      _delay_ms(200);
      if(minutes < 59)
      minutes++;
      else
      minutes = 0;
    }
    /* Set Hours when SegCntrl Pin 5 Switch is Pressed*/
    if((AdjustPins & 0x20) == 0 )
    {
      _delay_ms(200);
      if(hours < 23)
      hours++;
      else
      hours = 0;
    }
}
void readButtons(){
//Take special attention to the initialize digital pin LED_BUILTIN as an output.
//
int ledPin = 13;   // LED connected to digital pin 13
unsigned int inPin = 7;     // pushbutton connected to digital pin 7
unsigned int val = 0;       // variable to store the read value
unsigned int dataIn=0;

byte array[8] = {0,0,0,0,0,0,0,0};
byte together = 0;

unsigned char receive = 7; //define our transmit pin
unsigned char data = 0; //value to transmit, binary 10101010
unsigned char mask = 1; //our bitmask

array[0] = 1;

unsigned char btn1 = 0x41;

      digitalWrite(AP6928_stb, LOW);
        delayMicroseconds(2);
      cmd_without_stb(0b01000010); // cmd 2 //10=Read Keys; 00=Wr DSP;
      delayMicroseconds(2);
      // cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
      // send without stb
  
  pinMode(7, INPUT_PULLUP);  // Important this point! Here I'm changing the direction of the pin to INPUT data.
  delayMicroseconds(2);
  //PORTD != B01010100; // this will set only the pins you want and leave the rest alone at
  //their current value (0 or 1), be careful setting an input pin though as you may turn 
  //on or off the pull up resistor  
  //This don't send the strobe signal, to be used in burst data send
         for (int z = 0; z < 4; z++){
             //for (mask=00000001; mask > 0; mask <<= 1) { //iterate through bit mask
                   for (int h =8; h > 0; h--) {
                      digitalWrite(AP6928_clk, HIGH);  // Remember wich the read data happen when the clk go from LOW to HIGH! Reverse from write data to out.
                      delayMicroseconds(2);
                     val = digitalRead(inPin);
                      //digitalWrite(ledPin, val);    // sets the LED to the button's value
                           if (val & mask){ // if bitwise AND resolves to true
                             //Serial.print(val);
                            //data =data | (1 << mask);
                            array[h] = 1;
                           }
                           else{ //if bitwise and resolves to false
                            //Serial.print(val);
                           // data = data | (1 << mask);
                           array[h] = 0;
                           }
                    digitalWrite(AP6928_clk, LOW);
                    delayMicroseconds(2);
                   } 
             
              Serial.print(z);  // All the lines of print is only used to debug, comment it, please!
              Serial.print(" - " );
                        
                                  for (int bits = 7 ; bits > -1; bits--) {
                                      Serial.print(array[bits]);
                                   }
                        
                        if (z==0){
                           if(array[4] == 1){
                             flagSecs = !flagSecs;  // This change the app to hours or seconds
                           }
                        }
                        
                        if (z==1){
                           if(array[7] == 1){
                              digitalWrite(2, !digitalRead(2));
                          }
                        }
                        
                        if (z==1){
                           if(array[4] == 1){
                             digitalWrite(3, !digitalRead(3));
                             //digitalWrite(AP6928_onGreen, !digitalRead(AP6928_onGreen));
                            }
                        } 
                          
                        if (z==2){
                           if(array[7] == 1){
                             digitalWrite(4, !digitalRead(4));
                             //digitalWrite(AP6928_onGreen, !digitalRead(AP6928_onGreen));
                            }
                        }                                      
                  Serial.println();
          }  // End of "for" of "z"
      Serial.println();  // This line is only used to debug, please comment it!

 digitalWrite(AP6928_stb, HIGH);
 delayMicroseconds(2);
 cmd_with_stb((0b10001000) | 7); //cmd 4
 delayMicroseconds(2);
 pinMode(7, OUTPUT);  // Important this point! Here I'm changing the direction of the pin to OUTPUT data.
 delay(1); 
}
void led(void){
      for(int i=0;i<15; i++){
      digitalWrite(panelLED,  !digitalRead(panelLED));
      delay(50);
      }
}
void write_Wheels(unsigned char segm, unsigned char grid){
//Only to design the Wheels segments move!
                          switch (segm) {
                                  case 'a': 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (WheelsA[v]<<grid));
                                        }break;
                                  case 'b': 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (WheelsB[v]<<grid));
                                        }break;
                                  case 'c':
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (WheelsC[v]<<grid));
                                        } break;
                                  case 'd': 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (WheelsD[v]<<grid));
                                        }break;
                                  case 'e': 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (WheelsE[v]<<grid));
                                        }break;
                                  case 'f':
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (WheelsF[v]<<grid));
                                        }break;
                                  default:break;
                                }
                                 
                                 if (grid == 1){  //This define wich RUN only one time by update, can be any number of grid(segmente on this solution) 0~9.
                                  cmd_with_stb(gridSegments); // cmd 1 // AP6928 is a driver of 7 grids
                                  cmd_with_stb(0b10000000); // cmd 2 //Normal operation; Set pulse as 1/16
                                  digitalWrite(AP6928_stb, LOW);
                                  delayMicroseconds(1);
                                  cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                                        for(int v=0; v<7; v++){
                                           wd0 = (segOR[v] & 0x00ffUL) >>  0;
                                           wd1 = (segOR[v] & 0xff00UL) >>  8;
                                           //Serial.print("Wd0: "), Serial.print(wd0, BIN);Serial.print(", Wd1: "), Serial.println(wd1, BIN);
                                        cmd_without_stb(wd0); //Send all the table of data to the map of driver (00~0D)
                                        cmd_without_stb(wd1); //Send all the table of data to the map of driver (00~0D)
                                        //Serial.println(segOR[v], BIN);
                                        }
                                  digitalWrite(AP6928_stb, HIGH);
                                  cmd_with_stb((0b10001000) | 7); //cmd 4
                                  delay(10);
                                        for(int v=0; v<14; v++){
                                        segOR[v]=0b00000000;  //Clear of contents of array table
                                        }
                  }
}
void runWeels(void){
// Note the bits is already at segments position correct, by this reazon I move "0" the shift register!
    for(int w=0; w<6; w++){
      write_Wheels(wheels[w], 0);
      delay(10);
    }
}
void loop() {
//You can comment untill while cycle to avoid the test running.
for (int n=0; n<6; n++){
  number=0;
  write_panel_DVD( '3', 4);
  delay(500);
}
    for (int i=0; i<4; i++){
    led(); // This function is only to blinking the panel LED
    test_WheelsON();
    led();
    test_WheelsOFF();
    clear_AP6928();
    test_AllON();
    led();
    }

// I decide not use the dinamic refreshing, but you can do it!
       while(1){
              if (flag=!flag){
                //digitalWrite(panelLED,  !digitalRead(panelLED));//Only to control the LED is at left of Display. Comment if not use it!!!
                send_update_clock();
                delay(100);
                readButtons();
                delay(100);
                updateDisplay();
                segmWheels++;
                if(segmWheels ==6){
                segmWheels = 0;
                }
              write_Wheels(wheels[segmWheels], 0);
             }
       }
}

ISR(TIMER1_COMPA_vect)   {  //This is the interrupt request
// https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
      secs++;
      flag = !flag;   
} 
