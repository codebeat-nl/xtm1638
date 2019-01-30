 // Project: xtm1638 usage examples, nr. 1 
 // Author : codebeat - Erwin Haantjes - http://codebeat.nl
 // Source : https://github.com/codebeat-nl/xtm1638
 // Date   : 17 may 2017
 // ---------------------------------
 // WHAT IT DOES
 // Sample program, a hello world example.
 // A program to show you how to work with the xtm1638 class, 
 // this example includes:
 // o Handle compatible mode (so you can use the library on any 'Arduino')
 // o Show some 'info'
 // o Read and use a button
 // o Set leds on top of the board 
 // o Do an animation with custom characters
 //
 // BUTTON ASSIGMENTS (LEFT TO RIGHT)
 // o Button #1 - SAY hello 
 // o Button #2 - Show numbers (hex) 
 // o Button #3 - Do a little wave animation with custom chars 
 

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
 
 // Read the xtm1638.config.h file to find more options
#endif

void setup()
{
  // Nothing to do here
}

void loop()
{
    // Check if button pressed 
   uint8_t iButton = ledandkey.getButtonPressed();
   
   ledandkey.setChars("INPUT ?");
   
   delay(300); // Give processing some time

    // Button press detected?
    if( iButton != XTM_NOBUTTON )
    {
        // User want to say hello? 
       if( iButton == XTM_BUTTON1 )
        { 
          ledandkey.setLED(0);
          ledandkey.setLED(7);
          ledandkey.setChars("HELLO");
          delay(2000);
          ledandkey.setChars("WORLD");
          delay(2000);
          ledandkey.clearLEDs();
        }
       else
        
        // User want to show some numbers? 
       if( iButton == XTM_BUTTON2 )
        { 
          ledandkey.setLED(3);
          ledandkey.setLED(4);
          
          ledandkey.setChars("01234567");
          delay(2000);
          ledandkey.setChars("89ABCDEF");
          delay(2000);
          ledandkey.setLED(3);
          ledandkey.clearLEDs();
       }
       else
        
        // User want to do animation? 
       if( iButton == XTM_BUTTON3 )
        { 
          ledandkey.setLED(6);
          ledandkey.setLED(7);
            
          uint8_t i = 40;
          bool    b = false;
          
          while( i-- )
          {
            for( uint8_t x=0; x<8; x++ )
            {
              ledandkey.setByte( x, b?0x37:0x08 );
              b=!b;
            }  
           
           delay(100); 
           b=!b;
          }
          
          ledandkey.clearLEDs();
        }
    }
}
