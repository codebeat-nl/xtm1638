 // Project: xtm1638 usage examples, nr. 5 
 // Author : codebeat - Erwin Haantjes - http://codebeat.nl
 // Source : https://github.com/codebeat-nl/xtm1638
 // Date   : 30 jan 2019
 // ---------------------------------
 // WHAT IT DOES
 // Sample program to show you how to work with the object, this example includes:
 // o Handle compatible mode (so you can use the library on any 'Arduino')
 // o Read and use buttons, handle button combinations
 // o The gauge functionality and it possibilities
 // o Change the orientation of operation
 // o Battery indicator and animation demo  
 //                     
 //
 // BUTTON ASSIGMENTS (LEFT TO RIGHT)
 // o Button #1 - Select gauge type
 // o Button #2 - Select gauge style
 // o Button #3 - Gauge peakhold on/off
 // o Button #4 - Switch Single or Dual (average) mode of gauge
 // |__ + Button #2 - Shows a message that two buttons are pressed 
 // o Button #5 - Battery indicator demo
 // o Button #6 - Simple animation demo
 // o Button #7 - Not in use
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
 
 // Read the xtm1638.config.h file to find more options
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

#define iMinStyle       XTM_GAUGE_STYLE_PIPE
#define iMaxStyle       XTM_GAUGE_STYLE_LED
#define iMinSubStyle    XTM_GAUGE_SUBSTYLE_NORMAL
#define iMaxSubStyle    XTM_GAUGE_SUBSTYLE_INBOUND

// Global vars, setting holders
static bool    bOrient       = XTM_ORIENT_NORMAL;
static bool    bAudioInput   = true;
static uint8_t iFirstPerc    = 0;     // Percents %
static uint8_t iSecondPerc   = 100;   // Percents %
static uint8_t iPeakHold     = 0;     // Percents %
static uint8_t bPeakEnabled  = false;
static uint8_t bDualPerc     = true;
static uint8_t iStyle        = iMaxStyle;
static uint8_t iSubStyle     = iMinSubStyle;
  
  // Names and settings of gauge type 
static const char* aStyleNames[] = {"! Pipe", "! Stripe", "!BullTop", "!BullBtm", "!CentrLn", "{ LEDS }"};  
  
  // Names and settings of gauge style
static const char* aSubStyleNames[] = {"Normal", "Center" /* = outbound */, "Inbound"};  


uint8_t getAnalogAudio( uint8_t iChannel )
{
  uint16_t iResult = analogRead( iChannel );
  
  if( iResult > 0 )
  { 
   iResult = round(iResult * (100.0 / 1024.0)); 
  }
  
  return ( iResult > 100 )?100:iResult;
}


void setup()
{
  // Nothing special here
  Serial.begin( 9600 );
  
   // Dim the display a bit, it is very, very bright at default
  ledandkey.setDisplay(true,2);
  
  pinMode( A1, INPUT );
  pinMode( A2, INPUT );
  
  ledandkey.setChars( "TOUCH" ); 
  delay(1500);
  ledandkey.setChars( "PIN   A1" ); 
  delay(1500);
  ledandkey.setChars( "OR   AND" ); 
  delay(1500);
  ledandkey.setChars( "PIN   A2" ); 
  delay(1500);
  ledandkey.setChars( "WITH" ); 
  delay(1500);
  ledandkey.setChars( "    YOUR" ); 
  delay(1500);
  ledandkey.setChars( "FINGER" ); 
  delay(1500); 
  ledandkey.clear();
}

 
void loop()
{
   // Check if button pressed 
  uint8_t iButton = ledandkey.getButtonPressed();

   // Button press detected?
  if( iButton != XTM_NOBUTTON )
  {
       delay(200);
        // User want to change gauge type? 
       if( iButton == XTM_BUTTON1 )
        { 
          ++iStyle;
          if( iStyle > iMaxStyle )
           { iStyle = iMinStyle; }
           
          ledandkey.setChars( aStyleNames[iStyle-iMinStyle] ); 
        }
       else
       
       // User want to change style? 
       if( iButton == XTM_BUTTON2 )
        { 
          ++iSubStyle;
          if( iSubStyle > iMaxSubStyle )
           { iSubStyle = iMinSubStyle; }
           
          ledandkey.setChars( aSubStyleNames[iSubStyle-iMinSubStyle] ); 
        }
       else 
        
        // User want to turn peakhold ON or OFF? 
       if( iButton == XTM_BUTTON3 )
        { 
          bPeakEnabled=!bPeakEnabled;
          ledandkey.setChars( bPeakEnabled?"Peak on":"Peak off" ); 
        }
       else
        
        // User want to change gauge dual(average)/single mode? 
       if( iButton == XTM_BUTTON4 )
        { 
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
               }
          delay(200);
        }
       else 
       
       if( iButton == XTM_BUTTON5 )
        { 
          // Fake battery indicator
          ledandkey.gauge( 0, 0, 80, iStyle, XTM_GAUGE_SUBSTYLE_INBOUND );
          ledandkey.setChars( "BATT", 0, false ); 
          delay( 4000 );
          ledandkey.setChars( "BATT" ); 
          ledandkey.setNumber( 80 );
          delay( 4000 );
          ledandkey.clear();
          ledandkey.clearLEDs();
        }
       else    
       
        
       if( iButton == XTM_BUTTON6 )
        { 
          // Simple 'Wave' animation
          
          // Wait until no key pressed
          while( ledandkey.getButtonPressed() != XTM_NOBUTTON );
          
          uint8_t i;
          bool    b = true; // waveflag
          
          // Do this while no key is pressed
          while( ledandkey.getButtonPressed() == XTM_NOBUTTON )
          {
            i=8;
            while( i-- )
            { 
              ledandkey.setByte( i, b?0x23:0x1C ); 
              if( i > 0 )
               { b=!b; }
              delay(10);
            }
            delay(100);
          }
          
          ledandkey.clear();
          ledandkey.clearLEDs();
        }
       else    
      
       if( iButton == XTM_BUTTON7 )
        { 
          // do something you want here
        }
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
        {  delay(2000);
           ledandkey.clear(); 
        }
       else { 
               ledandkey.clearLEDs(); 
               delay(2000);
            } 
  
      
       // Wait for release
       ledandkey.waitForNoButtonPressed();
  }

  if( bAudioInput )
  {
    iFirstPerc  = getAnalogAudio(A1);
    iSecondPerc = getAnalogAudio(A2);
  }
  else {
          if( iSecondPerc > 0 )
           { iSecondPerc-=2; }
          else { iSecondPerc = 100; } 
  
          iFirstPerc+=2;
          if( iFirstPerc > 100 )
            { iFirstPerc = 0; }
          
          delay(100);
        }
  

   // Apply 
  ledandkey.gauge( bPeakEnabled?(bDualPerc?iPeakHold:iFirstPerc):0, 
                   iFirstPerc, 
                   bDualPerc?iSecondPerc:XTM_GAUGE_SINGLE, 
                   iStyle,
                   iSubStyle );  
  
  iPeakHold = iFirstPerc>iSecondPerc?iFirstPerc:iSecondPerc;
  
}
