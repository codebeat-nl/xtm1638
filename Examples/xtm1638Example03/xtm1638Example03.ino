 // Project: xtm1638 usage examples, nr. 3 
 // Author : codebeat - Erwin Haantjes - http://codebeat.nl
 // Source : https://github.com/codebeat-nl/xtm1638
 // Date   : 17 may 2017
 // ---------------------------------
 // WHAT IT DOES
 // Sample program to show you how to work with the object, this example includes:
 // o Handle compatible mode (so you can use the library on any 'Arduino')
 // o Read and use buttons, handle button combinations
 // o The gauge functionality and it possibilities
 // o Change the orientation of operation
 // o Character demo
 // o Performance test
 //                     
 //
 // BUTTON ASSIGMENTS (LEFT TO RIGHT)
 // o Button #1 - Switch Single or Dual (average) mode of gauge
 // |__ + Button #2 - Shows a message that two buttons are pressed 
 // o Button #2 - Select gauge type
 // o Button #3 - Gauge peakhold on/off
 // o Button #4 - Select gauge style
 // o Button #5 - Character demo, shows all characters possible
 // o Button #6 - Performance test, count up to 100.000, report you a score in milliseconds
 // o Button #7 - Performance test, same as #6 but without flickering LEDS
 // o Button #8 - Switch orientation of the board
 //               NOTICE: All functionality is swapped, is upside down, also buttons!
 

 // Include the library first
#include <xtm1638.h>

 // ARDUINO COMPATIBLE MODE EXPLAINED
 // ---------------------------------
 // If the code detects it is not compiled on an ATMEL AVR, it will switch to Arduino
 // compatible mode, which means the code doesn't use port registers directly (Arduino's
 // library handles this). In this case you need to specify (digital) PIN numbers. 
 // Overall, compatible mode is slower than the direct port access methods the code
 // provides, but when your application is not time critical, it just works fine.
 // Also, in compatible mode, the code consumes more memory, a few hundred bytes more. 

#ifdef XTM_ARDUINO_COMPATIBLE  
 // Specify (digital) PINS as described on the board
 #define PIN_DIG_LEDKEY_DATAIO  8
 #define PIN_DIG_LEDKEY_CLOCK   9
 #define PIN_DIG_LEDKEY_STROBE  10
#else 
  // Specify register port PINS specified in the documentation of the board
 #define PIN_REG_LEDKEY_DATAIO  PB0
 #define PIN_REG_LEDKEY_CLOCK   PB1
 #define PIN_REG_LEDKEY_STROBE  PB2
#endif

 // Create the class object
#ifdef XTM_ARDUINO_COMPATIBLE  
  // Specify (digital) PINS as described on the board
 static xtm1638 ledandkey( PIN_DIG_LEDKEY_DATAIO, 
                           PIN_DIG_LEDKEY_CLOCK, 
                           PIN_DIG_LEDKEY_STROBE 
                         );
#else
  // Specify register port PINS specified in the documentation of the board
 static xtm1638 ledandkey( PIN_REG_LEDKEY_DATAIO, 
                           PIN_REG_LEDKEY_CLOCK, 
                           PIN_REG_LEDKEY_STROBE 
                         );
#endif

#define        iMinStyle       XTM_GAUGE_STYLE_PIPE
#define        iMaxStyle       XTM_GAUGE_STYLE_LED
#define        iMinSubStyle    XTM_GAUGE_SUBSTYLE_NORMAL
#define        iMaxSubStyle    XTM_GAUGE_SUBSTYLE_INBOUND

// Global vars, setting holders
static bool    bOrient       = XTM_ORIENT_NORMAL;
static uint8_t iFirstPerc    = 0;     // Percents %
static uint8_t iSecondPerc   = 100;   // Percents %
static uint8_t iPeakHold     = 0;     // Percents %
static uint8_t bPeakEnabled  = true;
static uint8_t bDualPerc     = true;
static uint8_t iStyle        = iMaxStyle;
static uint8_t iSubStyle     = iMinSubStyle;
  
  // Names and settings of gauge type 
static const char* aStyleNames[] = {"! Pipe", "! Stripe", "!BullTop", "!BullBtm", "!CentrLn", "{ LEDS }"};  
  
  // Names and settings of gauge style
static const char* aSubStyleNames[] = {"Normal", "Center", "inbound"};  


void startCharDemo()
{
  // NOTICE:
  // I use a macro of the class to get characters, forget it, it's low level stuff and
  // only used here to show all the characters available. Normally you don't need this.
          
   uint8_t i = 0;
   uint8_t x;
   bool    b;
   bool    bWantEscape = false;
          
          ledandkey.clearLEDs();
          ledandkey.setLED(0);
          ledandkey.setLED(3);
          ledandkey.setLED(4);
          ledandkey.setLED(7);
          
          ledandkey.setChars( "CHR DEMO" ); 
          delay(2000);
          ledandkey.clearLED(0);
          ledandkey.setChars( "HOLD DWN" ); 
          delay(2000);
          ledandkey.clearLED(7);
          ledandkey.setChars( "BUTTON 5" ); 
          delay(2000);
          ledandkey.clearLED(3);
          ledandkey.setChars( "TO ESCPE" ); 
          delay(2000);
          ledandkey.clearLED(4);
          
           // Wait for release, if not show user message and wait again....
          while( !ledandkey.waitForNoButtonPressed( XTM_BUTTON5 ) )
           { 
             ledandkey.setChars( "RELEASE" ); 
             delay( 2000 );
             ledandkey.setChars( "BUTTON" ); 
             delay( 2000 );
             ledandkey.setChars( "NOW" ); 
             delay( 2000 );
             ledandkey.clear(); 
           }

          
          while( !bWantEscape && (i+7 < XTM_SPECIAL_CHAR_OFFSET) ) 
          {
            if( ledandkey.getButtonPressed() == XTM_BUTTON5 )
             { bWantEscape = true; }
            
            for( x=0; x < 8; ++x )
             { ledandkey.setByte( x, XTM_GET_DIGIT(i+x) ); }
            ++i;
            delay(200);
          } 

          if( !bWantEscape )
          {
            delay(1000);
            ledandkey.setChars( "SPECIAL" ); 
            delay(2000);
            ledandkey.setChars( "   CHARS" ); 
            delay(2000);
            ledandkey.setChars( "   {CHRS" ); 
            
            i = XTM_SPECIAL_CHAR_OFFSET;
            while( !bWantEscape && (XTM_GET_DIGIT(i)) ) 
            {
              if( ledandkey.getButtonPressed() == XTM_BUTTON5 )
               { bWantEscape = true; }
              
              if( !bWantEscape )
              {
                b = false;
                for( x=0; x < 8; ++x )
                 { 
                   ledandkey.setByte( 0, (b=!b)?XTM_GET_DIGIT(i+1):0x00 ); 
                   delay(100);
                 }
                
                delay(200);
              }
              i+=2;
            } 
          }
           
          ledandkey.setChars( "END DEMO" ); 
          delay(1000);
  
}


void startCounterDemo(bool bWithoutSetLed = false) // Count up to 100.000
{
 ledandkey.clearLEDs();
 ledandkey.setLED(0);
 ledandkey.setLED(3);
 ledandkey.setLED(4);
 ledandkey.setLED(7);
          
 ledandkey.setChars( "COUNTER" ); 
 delay(2000);
 ledandkey.clearLED(0);
 ledandkey.setChars( "SPEED" ); 
 delay(2000);
 ledandkey.clearLED(7);
 ledandkey.setChars( "}   DEMO" ); 
 delay(2000);
 ledandkey.clearLED(3);
 ledandkey.setChars( bWithoutSetLed?"NO LEDS":"100000x " ); 
 delay(2000);
 ledandkey.clearLED(4);
 ledandkey.clear();
          
 unsigned long i = 0;
 uint8_t       x = 0;
 uint8_t       y = 0;
 unsigned long iBench = millis();

 if( bWithoutSetLed )
 { 
   while( ++i < 100001UL )
    { ledandkey.setNumber(i); } 
 }
 else {
        while( ++i < 100001UL )
        {
            ledandkey.clearLED(y);
            ledandkey.setLED(x);
            y = x;
            ++x;
            if( x > 7 )
              { x = 0; }
            ledandkey.setNumber(i); 
        } 
      }
 
 iBench = millis()-iBench;
 ledandkey.setChars( "DONE" ); 
 delay(2000);
 ledandkey.clearLEDs();
 ledandkey.setChars( "SCORE" ); 
 delay(2000);
 
 ledandkey.clear();
 ledandkey.setNumber( iBench ); 
 delay(4000);
}


void setup()
{
  // Nothing special here
  Serial.begin( 9600 );
  ledandkey.setDisplay(true,2);
}

 
void loop()
{
   // Check if button pressed 
  uint8_t iButton = ledandkey.getButtonPressed();

   // Button press detected?
  if( iButton != XTM_NOBUTTON )
  {
        // User want to change gauge dual(average)/single mode? 
       if( iButton == XTM_BUTTON1 )
        { 
          delay(200);
           // Or when the user pressed two buttons, show this
          if( ledandkey.isButtonPressed( XTM_BUTTON2 ))
           {  
              ledandkey.setChars( "YOU" ); 
              delay(2000);
              ledandkey.setChars( "pressed" ); 
              delay(2000);
              ledandkey.setChars( "two" ); 
              delay(2000);
              ledandkey.setChars( "buttons" ); 
           }
          else {
                  bDualPerc=!bDualPerc;
                  ledandkey.setChars(bDualPerc?"DualAVG":"Single" ); 
                  delay(2000);
			   }
        }
       else 
        // User want to change gauge type? 
       if( iButton == XTM_BUTTON2 )
        { 
          ++iStyle;
          if( iStyle > iMaxStyle )
           { iStyle = iMinStyle; }
           
          ledandkey.setChars( aStyleNames[iStyle-iMinStyle] ); 
        }
       else
        
        // User want to turn peakhold ON or OFF? 
       if( iButton == XTM_BUTTON3 )
        { 
          bPeakEnabled=!bPeakEnabled;
          ledandkey.setChars( bPeakEnabled?"Peak on":"Peak off" ); 
        }
       else
        
       
       // User want to change style? 
       if( iButton == XTM_BUTTON4 )
        { 
          ++iSubStyle;
          if( iSubStyle > iMaxSubStyle )
           { iSubStyle = iMinSubStyle; }
           
          ledandkey.setChars( aSubStyleNames[iSubStyle-iMinSubStyle] ); 
        }
       else 
       
       // User want to show character demo? 
       if( iButton == XTM_BUTTON5 )
        { startCharDemo(); }
       else
       
       // User want to show speed demo? 
       if( iButton == XTM_BUTTON6 )
        { startCounterDemo(); }
       else    
      
        // User want to show speed demo without LEDs? 
       if( iButton == XTM_BUTTON7 )
        { startCounterDemo(true); }
       else    

        // User want to change orientation? 
       if( iButton == XTM_BUTTON8 )
        { 
          ledandkey.setOrientation( bOrient ); 
          bOrient=!bOrient;
          ledandkey.setChars(bOrient?"= Normal":"= Upside" ); 
        }
       else { return; } 
       
       if( iStyle == XTM_GAUGE_STYLE_LED )
        { ledandkey.clear(); }
       else { ledandkey.clearLEDs(); } 
  
       delay(2000);
       // Wait for release
       ledandkey.waitForNoButtonPressed();
  }

  
  if( iSecondPerc > 0 )
   { iSecondPerc-=2; }
  else { iSecondPerc = 100; } 
  
  iFirstPerc+=2;
  if( iFirstPerc > 100 )
   { iFirstPerc = 0; }
  

   // Apply 
  ledandkey.gauge( bPeakEnabled?(bDualPerc?iPeakHold:iFirstPerc):0, 
                   iFirstPerc, 
                   bDualPerc?iSecondPerc:XTM_GAUGE_SINGLE, 
                   iStyle,
                   iSubStyle );  
  
  iPeakHold = iFirstPerc>iSecondPerc?iFirstPerc:iSecondPerc;
  delay(100);
}
