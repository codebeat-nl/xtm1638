#pragma no-cache
/*
  XTM1638 Library v2.02 (a TM1638 Led & Key library based upon AVR
  TM1638 "Library" v1.02 of IronCreek Software.

  Very fast library to control TM1638 chip (for example: "LED AND KEY") based
  modules, using (optional) direct port access on ATMEL (now MicroChip) MCU's.

  CopyLight (c) 2017-2019 codebeat, Erwin Haantjes, http://codebeat.nl
  Featuring some essential parts: Copyright (c) 2013 IronCreek Software

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  Just a WARNING:
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  VERSION history:
  - Date	: 20-may-2017 (v2.00)
    updated : 31-dec-2018 (v2.01)
    updated : 30-jan-2019 (v2.02)
	
	Original by IronCreek Software, available here:
      Source: https://github.com/int2str/TM1638
      Topic : https://forum.arduino.cc/index.php?topic=190472.0
 
  v2.02
   - Added caching method for segments, SUPERB performance! Compatible mode
     has been also improved because of this. See also XTM_APPLY_CACHED_SEGMENTS in 
	 changed xtm1638.config.h file;
   - Added more optimizations in assembler for AVR, see also 
     XTM_AVR_SHIFTWISE_DIVIDE_ASM in changed xtm1638.config.h file;
   - Added FastGPIO support for AVR, requires third-party library by Pololu 
     Corporation, see also XTM_AVR_ASM_MODE in changed xtm1638.config.h file;   
   - Add new constructor without parameters that use defaults specified in
     xtm1638.config.h file; 
   - Fix compatibility issue with ARM MCU's; 
   - Removed ARDUINO <= 100 IDE support, sorry, time to upgrade;
   - Fix position and layout problems with XTM_GAUGE_STYLE_PIPE style parameter 
     at gauge function. Most of code rewritten;
   - Added a new gauge style XTM_GAUGE_STYLE_CENTER_LINE;
   - Updated stats however not all latest performance updates;
   - Added extra example, no 5.
   
  v2.01
   - Added setSignedNumber() function;
   - Fix non display of null/zero (0) value in number functions;
   - Changed 2 characters, i and +.
    
 
  V2.00 (v1.01 IMPROVEMENTS by codebeat)
  ---------------------------------
  o Change layout of class and some names, macros and many other things;
  o Optional constructor parameters, no need to change library;
  o Adding many auto detection device/MCU defines;
  o Compatible Arduino mode, however, register/port manipulation is faster but
    only possible on AVR's (Atmel) boards/devices/MCU's. Still, because of this
    compatible mode, it is possible to use the code on non AVR models such as
    NodeMCU, ESP8266, etc;
  o Adding display settings functionality;
  o Adding orientation functionality (normal use or upside down use);
  o Replaced font method;
  o Adding more font characters;
  o Adding better divide/math methods (in namespace), overall performance improvement;
  o It is small and lightweight but a little heavier in size compared to previous
    version (approx +670 bytes) because of changes, improvements. Still much,
    MUCH, smaller comparing to rjbatista tm1638-library and MUCH more less
    MCU intensive;
  o Adding gauge functionality to be able to display bars, (battery) status etc;
  o Adding library config file;
  o Adding several (useful) examples with extended docu info;

  v1.01
   - Added divmod10_asm() (<---????? NOT there!)
   - Un-rolled send loop
   - Switch to toggeling output ports

  v1.00
   - Initial release

  SUPPORT ME ON PATREON!
  ----------------------
  Support the effort/time (many hours) spend to create this software solution, 
  if you like it, enjoyed it, appreciate it, discovering benefits by using it. 
  Support on Patreon if you are able to:
  https://www.patreon.com/codebeat
  
  If you do, you help and/or stimulate:
  - To spend more time to continue to create free open-source software like this;
  - Providing improvements and bug fixes; 
  - Time to deliver support or to write documentation;
  - Develop new projects;
  - Pay bills, hosting costs, rent, energy, costs of life;
  - Time to create (educational) video's (on my channel) and such.  
  
  Other ways to support me:
  -------------------------
  By sharing name, by sharing links to projects, by adopting or improving, 
  by contacting, by using thumbs up buttons, by subscribing, 
  by visiting/reading blog, etc.

  codebeat channels:
  - http://www.codebeat.nl             ; Main website
  - http://blog.codebeat.nl            ; Blog website (projects and more)
  - http://youtube.codebeat.nl         ; YouTube channel (shortcut)
  - http://patreon.codebeat.nl         ; Patreon support channel (shortcut)   
  - http://github.codebeat.nl     	   ; Github (projects)
  
  Thank you for supporting if you do!

  Happy coding, greetz,
  Erwin Haantjes (codebeat) 

*/

#ifndef XTM1638AE_H
#define XTM1638AE_H

#define XTM_OBJECT_VER    2.02
#define XTM_PORTB         0x00
#define XTM_PORTC         0x01
#define XTM_PORTD         0x02

 // Change this config file instead of changing this file if you want to change
 // something:
#include "xtm1638.config.h"

#ifdef __AVR__
 #include <avr/pgmspace.h>
 #include <util/delay.h>
#else
 #include <arduino.h>

 //#ifndef _delay_us
 // #define _delay_us(x)
 //#endif
#endif


#ifndef XTM_ARDUINO_COMPATIBLE
 #if !defined(__AVR__) || (!defined( PORTB ) && !defined( PORTC ) && !defined( PORTD ))
  #define XTM_ARDUINO_COMPATIBLE
 #else
   #ifndef XTM_PORT
     #error "1638 H file: XTM_PORT define is missing!"
   #endif

   #if XTM_PORT == XTM_PORTB && !defined(PORTB)
    #define XTM_ARDUINO_COMPATIBLE
    #pragma message("WARNING: PORTB not available on this device, switched to compatible mode!")

   #elif XTM_PORT == XTM_PORTC && !defined(PORTC)
    #define XTM_ARDUINO_COMPATIBLE
    #pragma message("WARNING: PORTC not available on this device, switched to compatible mode!")

   #elif XTM_PORT == XTM_PORTD && !defined(PORTD)
    #define XTM_ARDUINO_COMPATIBLE
    #pragma message("WARNING: PORTD not available on this device, switched to compatible mode!")
   #endif
   
 #endif
#endif

#ifdef XTM_ARDUINO_COMPATIBLE
  #include "xtm1638.config.h"
  
  #ifndef __AVR__
    #ifdef XTM_AVR_ASM_MODE
     #undef XTM_AVR_ASM_MODE
    #endif   
    #ifdef XTM_AVR_SHIFTWISE_DIVIDE_ASM
     #undef XTM_AVR_SHIFTWISE_DIVIDE_ASM
    #endif
  #endif	
#endif  


#if defined(XTM_NOPROGMEM) || !defined(PROGMEM) || !defined(pgm_read_byte)
 #pragma message("Compiling 1638 H file: PROGMEM off, font table in memory")
 #ifndef XTM_NOPROGMEM
   #define XTM_NOPROGMEM
 #endif
 #define XTM_MEM_ALLOC_TYPE
 #define XTM_GET_DIGIT(x) XTM_DIGITS[x]
#else
  #define XTM_MEM_ALLOC_TYPE PROGMEM
  #define XTM_GET_DIGIT(x) pgm_read_byte( &XTM_DIGITS[x] )
#endif


#ifndef XTM_ARDUINO_COMPATIBLE

 #ifdef XTM_AVR_ASM_MODE
 
 // Assembler mode
 
 // FastGPIO: If you don't have this lib, download it from here:
 // https://github.com/pololu/fastgpio-arduino
 #include <FastGPIO.h> 
 
 #pragma message("Compiling 1638 H file: Assembler mode.")
 
 #define XTM_CLK_LOW()            FastGPIO::Pin<XTM_AVR_ASM_PIN_CLOCK>::setOutput(0);
 #define XTM_CLK_HIGH()           FastGPIO::Pin<XTM_AVR_ASM_PIN_CLOCK>::setOutput(1);
 #define XTM_STB_LOW()            FastGPIO::Pin<XTM_AVR_ASM_PIN_STROBE>::setOutput(0);
 #define XTM_STB_HIGH()           FastGPIO::Pin<XTM_AVR_ASM_PIN_STROBE>::setOutput(1);
 #define XTM_DAT_LOW()            FastGPIO::Pin<XTM_AVR_ASM_PIN_DATAIO>::setOutput(0);
 #define XTM_DAT_HIGH()           FastGPIO::Pin<XTM_AVR_ASM_PIN_DATAIO>::setOutput(1);

 #define XTM_SETUP()              FastGPIO::Pin<XTM_AVR_ASM_PIN_CLOCK>::setOutput(0);  \
                                  FastGPIO::Pin<XTM_AVR_ASM_PIN_STROBE>::setOutput(0); \
                                  FastGPIO::Pin<XTM_AVR_ASM_PIN_DATAIO>::setOutput(0);
 #define XTM_INIT()               XTM_STB_HIGH(); XTM_CLK_HIGH();
 #define XTM_START_RECEIVE()      FastGPIO::Pin<XTM_AVR_ASM_PIN_DATAIO>::setInput(); XTM_DAT_HIGH();
 #define XTM_STOP_RECEIVE()       FastGPIO::Pin<XTM_AVR_ASM_PIN_DATAIO>::setOutput(0); XTM_DAT_LOW();
 #define XTM_COMPARE_RECEIVED()   (uint8_t)FastGPIO::Pin<XTM_AVR_ASM_PIN_DATAIO>::isInputHigh() 
 
 #else
  
  // Port manipulation mode
 #if XTM_PORT == XTM_PORTB
  #pragma message("Compiling 1638 H file: Port manipulation mode - PORTB")
  #define XTM_OUT_REG          PORTB
  #define XTM_IN_REG           PINB
  #define XTM_DDR_REG          DDRB
 #elif XTM_PORT == XTM_PORTC
  #pragma message("Compiling 1638 H file: Port manipulation mode - PORTC")
  #define XTM_OUT_REG          PORTC
  #define XTM_IN_REG           PINC
  #define XTM_DDR_REG          DDRC
 #elif XTM_PORT == XTM_PORTD
  #pragma message("Compiling 1638 H file: Port manipulation mode - PORTD")
  #define XTM_OUT_REG          PORTD
  #define XTM_IN_REG           PIND
  #define XTM_DDR_REG          DDRD
 #else
   #error "1638 H file: Something is wrong with configuration, cannot determine XTM_PORT settings!"
 #endif

 #define XTM_BIT_DAT              _BV(_pinDataIO)
 #define XTM_BIT_CLK              _BV(_pinClock)
 #define XTM_BIT_STB              _BV(_pinStrobe)

 #define XTM_CLK_LOW()            (XTM_OUT_REG &= ~XTM_BIT_CLK)
 #define XTM_CLK_HIGH()           (XTM_OUT_REG |= XTM_BIT_CLK)
 #define XTM_STB_LOW()            (XTM_OUT_REG &= ~XTM_BIT_STB)
 #define XTM_STB_HIGH()           (XTM_OUT_REG |= XTM_BIT_STB)
 #define XTM_DAT_LOW()            (XTM_OUT_REG &= ~XTM_BIT_DAT)
 #define XTM_DAT_HIGH()           (XTM_OUT_REG |= XTM_BIT_DAT)


 #define XTM_SETUP()              XTM_DDR_REG |= XTM_BIT_STB | XTM_BIT_CLK | \
                                  XTM_BIT_DAT; XTM_OUT_REG |= XTM_BIT_STB | \
                                  XTM_BIT_CLK
 #define XTM_INIT()
 #define XTM_START_RECEIVE()      XTM_DDR_REG &= ~XTM_BIT_DAT; XTM_DAT_HIGH();
 #define XTM_STOP_RECEIVE()       XTM_DDR_REG |= XTM_BIT_DAT; XTM_DAT_LOW();
 #define XTM_COMPARE_RECEIVED()   XTM_IN_REG & XTM_BIT_DAT
 #endif // XTM_AVR_ASM_MODE
  
#else
 // Compatible mode
 #pragma message("Compiling 1638 H file: Arduino library compatible mode.")

 #include <arduino.h>
 
 #define XTM_CLK_LOW()            digitalWrite( _pinClock , LOW  );
 #define XTM_CLK_HIGH()           digitalWrite( _pinClock , HIGH );
 #define XTM_STB_LOW()            digitalWrite( _pinStrobe, LOW  );
 #define XTM_STB_HIGH()           digitalWrite( _pinStrobe, HIGH );
 #define XTM_DAT_LOW()            digitalWrite( _pinDataIO, LOW  );
 #define XTM_DAT_HIGH()           digitalWrite( _pinDataIO, HIGH );

 #define XTM_SETUP()              pinMode( _pinClock, OUTPUT );  \
                                  pinMode( _pinStrobe, OUTPUT ); \
                                  pinMode( _pinDataIO, OUTPUT );
 #define XTM_INIT()               XTM_STB_HIGH(); XTM_CLK_HIGH();
 #define XTM_START_RECEIVE()      pinMode( _pinDataIO, INPUT  ); XTM_DAT_HIGH();
 #define XTM_STOP_RECEIVE()       pinMode( _pinDataIO, OUTPUT ); XTM_DAT_LOW();
 #define XTM_COMPARE_RECEIVED()   digitalRead( _pinDataIO )
 
#endif


// Register mappings
#define XTM_REG_MAX              0x0F
#define XTM_REG_LED_OFFSET       1
#define XTM_LED_TO_REG(p)        (XTM_REG_LED_OFFSET + (p << 1))

// Instructions
#define XTM_DATA_CMD             0x40
#define XTM_DISP_CTRL            0x80
#define XTM_ADDR_CMD             0xC0

// Data command set
#define XTM_WRITE_DISP           0x00
#define XTM_READ_KEYS            0x02
#define XTM_FIXED_ADDR           0x04

// Display control command
#define XTM_DISP_PWM_MASK        0x07 // First 3 bits are brightness (PWM controlled)
#define XTM_DISP_ENABLE          0x08


// ----------------------------------------------------------------------------
// Functions and parameters
// ----------------------------------------------------------------------------

// Parameters for setLED()
#define XTM_OFF          0x0
#define XTM_GREEN        0x1
#define XTM_RED          0x2

// Parameters for setNumber()
#define XTM_RIGHT        0x01
#define XTM_LEFT         0x00

// Parameters for setNumberPad()
#define XTM_PAD_SPACE    0x00
#define XTM_PAD_0        0x3F

// Parameters for setOrientation()
#define XTM_ORIENT_NORMAL        true
#define XTM_ORIENT_UPSIDEDOWN    false

// Return values getButtonPressed()
#define XTM_NOBUTTON             0x40
#define XTM_BUTTON1              0x00
#define XTM_BUTTON2              0x01
#define XTM_BUTTON3              0x02
#define XTM_BUTTON4              0x03
#define XTM_BUTTON5              0x04
#define XTM_BUTTON6              0x05
#define XTM_BUTTON7              0x06
#define XTM_BUTTON8              0x07


// Parameters for gauge(), see also included xtm1638Example03 and xtm1638Example05 
// how to use the gauge() function.
#define XTM_GAUGE_SINGLE              0x7F  // Single is default, use both segments
                                            // up and lower segments
#define XTM_GAUGE_STYLE_PIPE          0x6A  // Pipe is default, shows something
                                            // like this: ||||||||||||||||
#define XTM_GAUGE_STYLE_STRIPE        0x6B  // Shows something like this: ========
                                            // peak in middle as: -
#define XTM_GAUGE_STYLE_BULLET_TOP    0x6C  // Shows something like this: oooooooo
                                            // (on segment top), peak on bottom as _
#define XTM_GAUGE_STYLE_BULLET_BOTTOM 0x6D  // Shows something like this: oooooooo
                                            // (on segment top), peak on top as -
#define XTM_GAUGE_STYLE_CENTER_LINE   0x6E  // Shows something like this: |-------
#define XTM_GAUGE_STYLE_LED           0x6F  // Use the LEDs on top of the device,
                                            // Green is gauge, red is peak

#define XTM_GAUGE_SUBSTYLE_NORMAL     0x8A  // Default, show normal

#define XTM_GAUGE_SUBSTYLE_CENTER     0x8B  // Center = 0%  100% ----|---- 100%
                                            //                       0%

#define XTM_GAUGE_SUBSTYLE_INBOUND    0x8C  // Side = 0%      0% ----|---- 0%
                                            //                      100%


#define XTM_DOT                 0x80

#define XTM_CHAR_ERR            0x49

 // Special characters
#define XTM_MINUS               0x40
#define XTM_PLUS                0x46
#define XTM_BLANK               0x00
#define XTM_DEGREES             0x63
#define XTM_UNDERSCORE          0x08
#define XTM_EQUALS              0x48
#define XTM_LEFT                0x70
#define XTM_RIGHT               0x46
#define XTM_BRACKET_LEFT        0x39
#define XTM_BRACKET_RIGHT       0x0F
#define XTM_GREATER_LEFT        0x46
#define XTM_GREATER_RIGHT       0x70
#define XTM_ACCOLADE_LEFT       XTM_GREATER_LEFT
#define XTM_ACCOLADE_RIGHT      XTM_GREATER_RIGHT
#define XTM_COLON               0x09
#define XTM_PARENTHESIS_LEFT    XTM_BRACKET_LEFT
#define XTM_PARENTHESIS_RIGHT   XTM_BRACKET_RIGHT
#define XTM_QUESTION_MARK       0x53+0x80

#define XTM_SPECIAL_CHAR_OFFSET 0x24


//      Bits:                 Hex:
//        -- 0 --               -- 01 --
//       |       |             |        |
//       5       1            20        02
//       |       |             |        |
//        -- 6 --               -- 40 --
//       |       |             |        |
//       4       2            10        04
//       |       |             |        |
//        -- 3 --  .7           -- 08 --   .80
//
// How to 'calculate' a new character?
// -----------------------------------
// Just add the segments you want, for example:
// 01h + 02h + 40h + 04h + 08h = 4Fh => 0x4F == an E in opposite direction.    



XTM_MEM_ALLOC_TYPE const uint8_t XTM_DIGITS[] =
{
//  Numbers
//     0     1     2     3     4     5     6     7     8     9
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,             //(10)

//  Hex
//     A     b     C     D     E     F
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71,                                     //(06)

//  Letters ( * = same as K )
//     G     h     I     J     K     L     M     n     O     P     Q     r
    0x3D, 0x74, 0x06, 0x1F, 0x76, 0x38, 0x15, 0x54, 0x3F, 0x73, 0x67, 0x50, //(12)

//     S     t     U      V    W    *X     Y     Z
    0x6D, 0x78, 0x1C,  0x3E, 0x2A, 0x76, 0x6E, 0x5B,                        //(08)

// Special chars, offset = 0x24 (36)
   (uint8_t)'-', XTM_MINUS,
   (uint8_t)'+', XTM_PLUS,
   (uint8_t)' ', XTM_BLANK,
   (uint8_t)'^', XTM_DEGREES,
   (uint8_t)'_', XTM_UNDERSCORE,
   (uint8_t)'=', XTM_EQUALS,
   (uint8_t)'[', XTM_BRACKET_LEFT,
   (uint8_t)']', XTM_BRACKET_RIGHT,
   (uint8_t)'<', XTM_GREATER_LEFT,
   (uint8_t)'>', XTM_GREATER_RIGHT,
   (uint8_t)'{', XTM_ACCOLADE_LEFT,
   (uint8_t)'}', XTM_ACCOLADE_RIGHT,
   (uint8_t)':', XTM_COLON,
   (uint8_t)'(', XTM_PARENTHESIS_LEFT,
   (uint8_t)')', XTM_PARENTHESIS_RIGHT,
   (uint8_t)'?', XTM_QUESTION_MARK,
   0x00
};


#ifdef XTM_ARITHMETIC_MULTIPLY
 #define XTM_16_MULDIV_NUMBER_BY_10 number *= 0.1
 #define XTM_32_MULDIV_NUMBER_BY_10 number *= 0.1
 #define XTM_32_MODDIV_NUMBER_BY_10 number % 10
#else
  #ifdef XTM_SHIFTWISE_DIVIDE
  #define XTM_16_MULDIV_NUMBER_BY_10    _bit16Div10(&number)
  #define XTM_32_MULDIV_NUMBER_BY_10    _bit32Div10(&number)
  #define XTM_32_MODDIV_NUMBER_BY_10    _bit32Mod10(&number)
 #else
  #define XTM_16_MULDIV_NUMBER_BY_10 number /= 10
  #define XTM_32_MULDIV_NUMBER_BY_10 number /= 10
  #define XTM_32_MODDIV_NUMBER_BY_10 number % 10
 #endif
#endif

#ifdef XTM_APPLY_CACHED_SEGMENTS
 #pragma message("Compiling 1638 H file: Cached segments enabled.")
#endif
 
// Class to control a 8x 7-segment, 8x dual/single-color LED, 8x button module
// powered by the TM1638 chipset.
class xtm1638
{
  public:
    // Constructor: Construct a TM1638 object and initialize all the pins used.
    // About PIN / PORT configuration:
    // o All three pins used must be bits on the same PORT register (ex. PORTB)
    //   See also: https://www.arduino.cc/en/Reference/PortManipulation and also
    //   search for the pinouts of your Arduino.
    //   If your bits are not in the same register or you using a non-AVR
    //   device, code switch automaticly to compatible mode. using digital pins
    //   compatible with the Arduino programming library or you have
    //   to change the header file to keep speed and save a couple of
    //   hundred bytes.
    xtm1638( uint8_t iDataIoPin, uint8_t iClockPin, uint8_t iStrobePin );

	// Constructor: Construct object without parameters, uses the defaults specified 
	// in xtm1638.config.h file for specific mode. To view the defaults, take a 
	// look at this file.
	xtm1638();
	
  public:
	// Resets and initialize the device
	void reset();
  
	// Clears the 7-segment displays (only)
    void clear();

    // Set the display (digits and leds) enabled or disabled or change the
    // brightness of the display and leds.
    void setDisplay( bool bEnabled, uint8_t iBrightness = 7 );

    // Set the orientation of your device, normally keys are below the display
    // however you can change this to use the device with keys on top of the display 
	// (180 degrees turn). All text, leds and knobs will be interpreted upside down 
	// so it fits the orientation and expectations of the user and programmer.
    void setOrientation( bool bUpsideDown );

    // Rotates a byte, in fact a character 180 degrees.
	uint8_t rotateByte( uint8_t value );

    // Set a single 7-segment display to the given byte value.
    // This allows direct control of the elements to do spinning animations etc.
    void setByte(uint8_t pos, uint8_t value);

    // For example:
    // Set a 7-segments display to the given byte values at offset.
    // char values[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
    // setBytes(values);
    void setBytes(const char* value, uint8_t offset = 0);


    // Display a single digit at the given position.
    // Position is left-to-right, starting at 0.
    void setDigit(uint8_t pos, uint8_t value);

    // Display an unsigned number at a given offset and alignment.
    void setNumber(uint32_t number, uint8_t offset = 7, uint8_t align = XTM_RIGHT);
    
	// Display a signed number (negative and positive) at a given offset and alignment.
    void setSignedNumber(int32_t number, bool bShowPlusSign = false, uint8_t offset = 7, uint8_t align = XTM_RIGHT );

    // Display an unsigned number at a given offset and pad it with 0's or
    // spaces to a desired width. This function is helpful when the numbers can
    // fluctuate in length (ex. 100, 5, 1000) to avoid flickering and shifting segments.
    void setNumberPad(uint32_t number, uint8_t offset, uint8_t width, uint8_t pad = XTM_PAD_SPACE);
	
	// Display an unsigned number at a given offset and adds zero('s) before number
    // when the length (width) is less than required specified nDigits. 	
	// NOTICE: Offset must be higher or egual to nDigits.
	void setLzNumber(uint32_t number, uint8_t offset, uint8_t nDigits = 2);

    // Display an unsigned number in hex format at a given offset and pad it
    // with 0's or spaces to a desired width.
    void setNumberHex(uint32_t number, uint8_t offset, uint8_t width, uint8_t pad = XTM_PAD_SPACE);

    // Draw a character at a given position.
    // Not all characters are supported, check declaration for an overview.
    // If you want to draw custom characters (or animations), you better use 
	// setByte() function.
	void setChar(uint8_t pos, const char value );

    // Display a string starting at a given offset.
    void setChars(const char* value, uint8_t offset = 0, bool bClrScr = true );

    // Display a string starting at a given alignment
    void setAlignedChars(const char* value, uint8_t align = XTM_RIGHT, bool bClrScr = true );
	
    // Set which "dots" should be enabled.
    // Mask is mapped right to left (ex. 0x01 = right-most dot)
    void setDots(uint8_t mask);

    // Turn off a single LED
    void clearLED(uint8_t pos);

    // Turn off all LEDs
    void clearLEDs();

    // Set an LED at the given position to the specified color.
    // LEDs are numbered 0-7, left to right
    void setLED(uint8_t pos, uint8_t color = XTM_GREEN );

    // Bitmask setting up the color of all LEDs at once.
    // Examples:
    //    setLEDs(0xFF, 0x00);   <-- All green
    //    setLEDs(0x00, 0xFF);   <-- All red
    //    setLEDs(0xFF, 0xFF);   <-- All green+red
    //    setLEDs(0xF0, 0x0F);   <-- Right half green, left half red
    void setLEDs(uint8_t green, uint8_t red);

    // Returns a bitmask containing all button states.
    uint8_t getButtons();

    // Test if a button is pressed, returns true when iPos button is
    // pressed.
    bool isButtonPressed( uint8_t iPos, uint8_t iButtons = 0 );

    // Function to avoid/requirement include of strings.h, result is same as strlen() 
	uint8_t getStrLen( const char* value );
	
	// Returns a value <> XTM_NOBUTTON when a button is pressed.
    uint8_t getButtonPressed();

    // Wait for buttonpress, if no iTimeOutSec is specified the timeout
    // is infinite. if iTimeOutSec > 0 and seconds specified is reached,
    // function returns false, otherwise it returns true.
    bool waitForButtonPressed( uint8_t iButton, uint8_t iTimeOutSec = 0 );

    // Wait for NO buttonpress, if no iTimeOutSec is specified the timeout
    // is infinite. if iTimeOutSec > 0 and seconds specified is reached,
    // function returns false, otherwise it returns true.
    bool waitForNoButtonPressed( uint8_t iTimeOutSec = 0 );

    // Cool gauge functionality with peak indicator. Useful for VU-meter,
    // battery indicator, settings, measurement, etc.
    // Values are in percent, see also gauge options on top of this file.
    // Or refer to the demo's sketches provided with this library.
	// Examples:
    //   gauge( 0, 50 );        <-- 50% gauge
    //   gauge( 0, 50, 20 );    <-- First = 50%, second = 20%
    //   gauge( 80, 50, 20 );   <-- Peak = 80%, First = 50%, second = 20%
    void gauge( uint8_t iPeakPerc, uint8_t iFirstPerc,
                uint8_t iSecondPerc = XTM_GAUGE_SINGLE,
                uint8_t iStyle      = XTM_GAUGE_STYLE_PIPE,
                uint8_t iSubStyle   = XTM_GAUGE_SUBSTYLE_NORMAL
              );



protected:
    uint8_t _dotMask     = 0;
    bool    _orient      = XTM_ORIENT_NORMAL;
	
   #ifdef XTM_APPLY_CACHED_SEGMENTS
	uint8_t _segbuff[8]; // consumes 8 bytes of dynamic memory
   #endif
	
   #if defined(XTM_ARDUINO_COMPATIBLE) || !defined(XTM_AVR_ASM_MODE) 
    // Variables set with use of constructor
    uint8_t _pinStrobe;  // for example: PB0
    uint8_t _pinClock;   // for example: PB1
    uint8_t _pinDataIO;  // for example: PB2
   #endif 

	 // Internally used, applies parameters
	void setup( uint8_t iDataIoPin, uint8_t iClockPin, uint8_t iStrobePin );

    void send(uint8_t b);
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t addr, uint8_t data);

    uint8_t receive();
    uint8_t getOffsetDigits(uint32_t number);
};

#endif
