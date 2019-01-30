 // Project: xtm1638 usage examples, nr. 4 
 // Author : codebeat - Erwin Haantjes - http://codebeat.nl
 // Source : https://github.com/codebeat-nl/xtm1638
 // Dev Date : 17 may 2017
 // Modified : 30 jan 2019 (new scores by using updates)
 // -------------------------------------------------------
 // WHAT IT DOES
 // Sample program to show you how to work with the object, this example includes:
 // o Handle compatible mode (so you can use the library on any 'Arduino')
 // o Read and use buttons
 // o Performance test (between libs)
 // o Learn that compiled size, performance and memory usage matters
 //
 // BUTTON ASSIGMENTS (LEFT TO RIGHT) 
 // o Button #1 - Start test with leds
 // o Button #2 - Start test without leds
 //
 // To do the same with different libs:
 // On an Arduino Nano (ATMega328), this sketch with use of:
 //
 // Batista TM1638 library            : size 4992b  462b mem
 // ----------------------------
 // This library,
 // o With normal devide or multiply:
 //   - Portmode (AVR only)           : size 2812b  104b mem  (AVR direct port manipulation (=much faster))
 //   - Arduino compatible mode       : size 3104b  104b mem  (with digitalwrite etc)
 //   - Arduino compatible mode  \     
 //     without PROGMEM font table    : size 3098b  174b mem  
 // o With ShiftDivide mode:
 //   - Portmode (AVR only)           : size 3018b  116b mem  (AVR direct port manipulation (=much faster))
 //   - Arduino compatible mode       : size 3310b  116b mem  (with digitalwrite etc)
 //   - Arduino compatible mode  \     
 //     without PROGMEM font table    : size 3304b  186b mem  
 //
 // NOTICE:
 // o See also "ARDUINO COMPATIBLE MODE EXPLAINED" in ino file for more info.
 // o Compile sizes and memory usage may vary between different versions of the Arduino IDE and library.  

 //  Stress test with Arduino Nano (ATMega328), count-up to 100.000 test):
 //  ---------------------------------------------------------------------
 //  Mode:                        Score:        Minutes:           no-leds score:    no-leds minutes:
 //  Batista TM1638 library        448676ms     7.48 minutes(!)     363178ms          6.05 minutes(!)       
 //  ---------- xtm1638 library:
 //  Register+Multiply             94177ms      1.57 minutes        72077ms           1.20 minutes
 //  Register+Divide               86952ms      1.44 minutes        64852ms           1.08 minutes
 //  Register+ShiftDivide          76404ms      1.2734 minutes      58314ms           0.972 minutes(!)
 //  Register+ShiftDivide-PROGMEM  76374ms      1.2729 minutes      58283ms           0.971 minutes(!)
 //  Compatible Mode+Multiply      321876ms     5.36 minutes       232628ms           3.88 minutes
 //  Compatible Mode+Divide        314652ms     5.25 minutes       226404ms           3.77 minutes
 //  Compatible Mode+ShiftDivide   304093ms     5.07 minutes       219866ms           3.66 minutes
 //
 //  26 jan 2019 update: New records with caching (buffering)!
 //  Register+ShiftDivide+caching   39435ms     0.65725 minutes     22512ms           0.375 minutes(!) - speed gain: 1.9x/2.5x
 //  Compatible Mode+ShiftDivide+  142392ms     2.3732  minutes     59284ms           0.988 minutes(!) - speed gain: 2.1x/3.7x 
 //  caching                        
 // 
 //  29 jan 2019 update (1): New records by using assembler (ASM)!
 //  ASM+ShiftDivide                37125ms     0.618 minutes(!)    30239ms           0.504 minutes(!) - speed gain: 2.1x/1.9x
 //  ASM+ShiftDivide+caching        23889ms     0.398 minutes(!)    17004ms           0.283 minutes(!) - speed gain: 3.1x/3.4x
 //
 //  29 jan 2019 update (2): New records by using assembler (ASM) + font-table in dynamic memory!    
 //  ASM+ShiftDivide                37095ms     0.617 minutes(!)    30209ms           0.503 minutes(!) - speed gain: 2.1x/1.9x
 //  ASM+ShiftDivide+caching        23860ms     0.397 minutes(!)    16974ms           0.282 minutes(!) - speed gain: 3.2x/3.5x
 //
 //  30 jan 2019:
 //  Another gain can be reached by using new define (which is default now), XTM_AVR_SHIFTWISE_DIVIDE_ASM.
 //  I don't have the time right now to update the tables above again. Fastest speed is now 12756ms = 
 //  0.213 minutes = 12.756 seconds to count up to 100000 and display it. 28x faster than Batista's library.

 //  Conclusion:
 //  And the OVERALL winner is..... ;-) 


// Uncomment this define if you want to test with R. Batista library 
// (if you have it installed). You need to recompile this sketch. 
//#define LIB_1638_RBATISTA

#include "xtm1638Example04.h"

bool isButtonPressed( const uint8_t iPos, uint8_t iButtons  )
{
  if( iButtons == 0 )
   { iButtons = ledandkey.getButtons();
     if( iButtons == 0 )
      { return false; }
   }
  return (iButtons & (0x1 << iPos))?true:false;
}


uint8_t getButtonPressed()
{
  uint8_t iButtons = ledandkey.getButtons();
  uint8_t i = 0;

  while( i < 8 )
  {
    if( isButtonPressed(i++, iButtons ) )
     { return (i-1); }
  }

  return XTM_NOBUTTON;
}

void startCounterDemo(bool bWithoutsetLed = false) // Count up to 100.000
{
 clearLEDs();
 setLed(0);
 setLed(3);
 setLed(4);
 setLed(7);

 printStr( "COUNTER" );
 delay(2000);
 clearLED(0);
 printStr( "SPEED" );
 delay(2000);
 clearLED(7);
 printStr( "}   DEMO" );
 delay(2000);
 clearLED(3);
 printStr( bWithoutsetLed?"NO LEDS":"100000x " );
 delay(2000);
 clearLED(4);
 cls();

 unsigned long i = 0;
 uint8_t       x = 0;
 uint8_t       y = 0;
 unsigned long iBench = millis();

 if( bWithoutsetLed )
 {
   while( ++i < 100001UL )
    { printNum(i); }
 }
 else {
        while( ++i < 100001UL )
        {
            clearLED(y);
            setLed(x);
            y = x;
            ++x;
            if( x > 7 )
              { x = 0; }
            printNum(i);
        }
      }

 iBench = millis()-iBench;
 printStr( "DONE" );
 delay(2000);
 clearLEDs();
 printStr( "SCORE" );
 delay(2000);

 cls();
 printNum( iBench );
 delay(4000);
}


void setup()
{
  Serial.begin( 9600 );
  printStr("WhatsUp?");
  delay(2000);
  printStr("Stress");
  delay(2000);
  printStr("Test");
  delay(2000);
  printStr("Library");
  delay(2000);
}

bool bIsDirty = true; 

void loop()
{
  delay(300); // Give some time, be gentle 
  
  if( bIsDirty ) // avoid flicker
   { printStr("PressBtn"); }
  
  bIsDirty=false;
  
  uint8_t iButton = getButtonPressed();
  if( iButton != XTM_NOBUTTON )
  {
    if( iButton == XTM_BUTTON1 )
    {
       bIsDirty=true;
       startCounterDemo();
    }
    else
    
    if( iButton == XTM_BUTTON2 )
    {
       bIsDirty=true;
       startCounterDemo(true);
    }
  }
}
