
#include <arduino.h>
  
  // Include the library first
#ifdef LIB_1638_RBATISTA
  #include <TM1638.h>
#else
  #include <xtm1638.h>
#endif

 // ARDUINO COMPATIBLE MODE EXPLAINED
 // ---------------------------------
 // If the code detects it is not compiled on an ATMEL AVR, it will switch to Arduino
 // compatible mode, which means the code doesn't use port registers directly (Arduino's
 // library handles this). In this case you need to specify (digital) PIN numbers. 
 // Overall, compatible mode is slower than the direct port access methods the code
 // provides, however, when your application is not time critical, it just works fine.
 // Also, in compatible mode, the code consumes more memory, a few hundred bytes more. 

#if defined(LIB_1638_RBATISTA) || defined(XTM_ARDUINO_COMPATIBLE)  
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


#ifdef LIB_1638_RBATISTA
  
  #define XTM_NOBUTTON             0x40
  #define XTM_BUTTON1              0x00
  #define XTM_BUTTON2              0x01
  #define printStr(x) ledandkey.clearDisplay(); ledandkey.setDisplayToString(x) 
  #define printNum(x) ledandkey.setDisplayToDecNumber(x, 0x00, false)
  #define clearLEDs() ledandkey.setLEDs(0x00)
  #define clearLED(x) ledandkey.setLED( 0x00, x ); 
  #define setLed(x) ledandkey.setLED( 0x01, x ); 
  #define setOrientation(x) 
  #define cls() ledandkey.clearDisplay()
  
  static TM1638 ledandkey( PIN_DIG_LEDKEY_DATAIO, 
                           PIN_DIG_LEDKEY_CLOCK, 
                           PIN_DIG_LEDKEY_STROBE 
                         ); 
#else
  //ledandkey.clear();
  
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
  
  #define printStr(x) ledandkey.clear(); ledandkey.setChars(x)
  #define printNum(x) ledandkey.setNumber(x)
  #define clearLEDs() ledandkey.clearLEDs()
  #define clearLED(x) ledandkey.clearLED(x)
  #define setLed(x) ledandkey.setLED( x ) 
  #define setOrientation(x) ledandkey.setOrientation( x )
  #define cls() ledandkey.clear()
#endif
