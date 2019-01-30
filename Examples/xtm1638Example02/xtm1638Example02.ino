 // Project: xtm1638 usage examples, nr. 2 
 // Author : codebeat - Erwin Haantjes - http://codebeat.nl
 // Source : https://github.com/codebeat-nl/xtm1638
 // Date   : 17 may 2017
 // ---------------------------------
 // WHAT IT DOES
 // Sample program, a simple basic clock (with hours, minutes and seconds),
 // to show you how to work with the xtm1638 class, this example includes:
 // o Handle compatible mode (so you can use the library on any 'Arduino')
 // o Read and use buttons and how to handle button combinations
 // o Using a timer to time share operation without interuption
 // o Change the orientation of board operation
 //
 // NOTICE
 // o This is an example, this clock might not that accurate for daily use.
 //   You can fine-tune it yourself if you want to.
 // o RTC is missing so there is no time back-up, power loss = reset.
 //
 // BUTTON ASSIGMENTS (LEFT TO RIGHT)
 // o Button #1 - SET Hour button 
 // |__ + Button #6 - Hour up
 // |__ + Button #7 - Hour down
 // o Button #2 - SET minute button 
 // |__ + Button #6 - Hour up
 // |__ + Button #7 - Hour down
 // o Button #3 - RESET seconds button 
 // |__ + Button #6 - Reset seconds to 00
 // |__ + Button #7 - Reset seconds to 00
 // o Button #4 - Set brightness of display 
 // |__ + Button #6 - Brightness up
 // |__ + Button #7 - Brightness down
 // o Button #5 - Toggle blinky "dot" on/off 
 // o Button #8 - Switch orientation of the board
 //               NOTICE: All functionality is swapped, is upside down, also buttons!
 

 // Include the library first
#include <xtm1638.h>
#include "Timer.h"


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
 
 // Read the xtm1638.config.h file to find more options
#endif


 // Timers for clock update and button processing
static TTimer        tSec      = TTimer( 1000, (bool)true, (bool)false );
static TTimer        tButton   = TTimer(  200, (bool)true, (bool)false );

 // Clock data
static uint8_t       iSec      = 0;
static uint8_t       iMin      = 0;
static uint8_t       iHour     = 0;
static bool          bBlink    = false;

 // Board settings
static bool          bOrient         = XTM_ORIENT_NORMAL;
static uint8_t       iBrightness     = 7;

 // Operation states
static bool          bBlinkEnabled   = true;
static bool          bButtonPressed  = false;
static uint8_t       iMsgTimeOut     = false;


 // Updates a digit leading zero
void updateDigit( uint8_t iPos, uint8_t iValue )
{
   if( iValue < 10 )
   { 
     ledandkey.setByte(iPos, 0x3F );
     ledandkey.setDigit(iPos+1, iValue);
   }
  else { ledandkey.setNumber(iValue, iPos+1 ); }    
}

 // Updates time settings and changes
void updateTime(bool bTimeUpdate = false, bool bDisplay = true )
{
  if( bTimeUpdate )
   { ++iSec; }
   
  if( iSec > 59 )
   { iSec = 0; 
     if( bTimeUpdate )
      { ++iMin; }
   }  
  
  if( iMin > 59 )
   { 
     if( bTimeUpdate )
     {
       iSec = iMin = 0; 
       ++iHour;
     }
     else { iMin = 0; }  
   }  
  
  if( iHour > 23 )
   { 
     if( bTimeUpdate )
     {
       iHour = iSec = iMin = 0;
     } else { iHour = 0; }    
   }  

  if( bDisplay )
  {
    iMsgTimeOut=0;
    if( bTimeUpdate )
     { bBlink=!bBlink; }
    
    updateDigit(0, iHour );
    updateDigit(3, iMin );
    updateDigit(6, iSec );
    
    ledandkey.setByte(2, (bBlink && bBlinkEnabled)?XTM_EQUALS:XTM_OFF );
    ledandkey.setByte(5, (bBlink && bBlinkEnabled)?XTM_EQUALS:XTM_OFF );
  }
}

 // Shows a message on display for a period of seconds
void showMessage(char* sMessage, uint8_t iSecs = 2 )
{
  iMsgTimeOut=iSecs;
  ledandkey.setChars( sMessage );
}

void setup()
{
  // Nothing special here, just say hello
  showMessage(" =Hello=");
}

void loop()
{
  if( tSec.isTime() ) 
   { 
    updateTime(true, (iMsgTimeOut == 0)); 
    if( iMsgTimeOut > 0 )
     { --iMsgTimeOut; }
   }
  
  if( tButton.isTime() )
  { 
    // Check if button pressed 
   uint8_t iButton = ledandkey.getButtonPressed();

  
    // Button press detected?
    if( iButton != XTM_NOBUTTON )
    {
        bButtonPressed = true;
        bool bBtUp     = ledandkey.isButtonPressed( XTM_BUTTON6 );
        bool bBtDown   = !bBtUp?ledandkey.isButtonPressed( XTM_BUTTON7 ):false;
        
        // User want to change hours? 
       if( iButton == XTM_BUTTON1 )
        { 
          //char values[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
          //ledandkey.setBytes(values, 0);
          //return;
          
          ledandkey.setLED(0);
          ledandkey.setLED(1);
          
           // Up?
          if( bBtUp )
           { ++iHour; 
              updateTime();
           }
          else if( bBtDown )
                { 
                  iHour = (iHour > 0)?(iHour-1):23; 
                  updateTime();
                }
        }
       else
        
        // User want to change minutes? 
       if( iButton == XTM_BUTTON2 )
        { 
          ledandkey.setLED(3);
          ledandkey.setLED(4);
           
           // Up?
          if( bBtUp )
           { ++iMin; 
              updateTime();
           }
          else if( bBtDown )
                { 
                  iMin = (iMin > 0)?(iMin-1):59; 
                  updateTime();
                }
        }
       else
        
        // User want to reset seconds? 
       if( iButton == XTM_BUTTON3 )
        { 
          ledandkey.setLED(6);
          ledandkey.setLED(7);

           // Up?
          if( bBtUp || bBtDown )
           {  iSec = 0; 
              updateTime();
           }
        }
       else 
        
        // User want to change brightness? 
       if( iButton == XTM_BUTTON4 )
        { 
          showMessage("Brightns", 1 ); 
          if( bBtUp || bBtDown )
           {  
             if( bBtUp )
             {
               ++iBrightness;
               if( iBrightness > 7 )
                { iBrightness = 0; }
             }
             else {
                    if( iBrightness == 0 )
                     { iBrightness = 7; }
                    else { --iBrightness; } 
                  }
             
             ledandkey.setDisplay( true, iBrightness );
           }
           
           uint8_t i = 8; 
           while( i-- )
           { if( i <= iBrightness )
                 { ledandkey.setLED(i); }
                else { ledandkey.clearLED(i); } 
           }
        }
       else
       
        // User want to change blinky character on/off? 
       if( iButton == XTM_BUTTON5 )
        { 
          if( iMsgTimeOut == 0 )
          {
            bBlinkEnabled=!bBlinkEnabled;
            showMessage( bBlinkEnabled?"Blnk On":"Blnk Off" ); 
          }
        }
        else { ledandkey.clearLEDs(); }
        
        // User want to change orientation? 
       if( iButton == XTM_BUTTON8 )
        { 
          if( iMsgTimeOut == 0 )
          {
            ledandkey.setOrientation( bOrient ); 
            bOrient=!bOrient;
            showMessage(bOrient?"= Normal":"= Upside" ); 
          }
        }
        //else { ledandkey.clearLEDs(); }
    }
    else {
           if( bButtonPressed )
           {
             bButtonPressed = false;
             ledandkey.clearLEDs();
           }
         }
    }
}
