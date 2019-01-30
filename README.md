# Fastest library to control TM1638 chip (for example: "LED AND KEY") based modules
## updated 30-jan-2019

## Category
- Arduino C++
- Library, class xtm1638
- TM1638
- LED and KEY


## About
The TM1638 can be found in combination of 8 digit, 7-segment LED displays which also incorporate 8 buttons as well as bi-colored red and green LEDs. This library allows you to control the module with ease, using MCU's optimal direct port access on AVR's or in "Arduino library compatible mode" on any other 'Arduino'. Usage demo sketches included.


## Features
- Significantly faster than existing implementations (see also benchmark results below)
- Low latency
- Configurable ports and pins
- Optimized arithmic operations (ASM for AVR), shiftdivide for C++
- SuperB speed on ATMEL/MicroChip AVR by using direct port access and assembler
- Optional 8 byte segment cache
- FastGPIO compatibility (see also xtm1638.config.h)
- Auto detection when compiling for optimal configuration
- Cool gauge feature, gauge functionality to be able to display bars, (battery) status, VU-meter, etc
- Runs on any Arduino with auto compatible mode
- Easy to use class methods
- Light weight, small memory footprint (can vary by used compile settings, Arduino IDE and library)
- Compiler configuration messages when compiling 
- Configuration file included (no need to change class files)
- 5 Examples, usage sketches/demo's included


## Application usage cases
- Counters which requires low latency, for example gear control
- Clocks
- Battery equipment - indicator 
- Amplifiers, general display and/or VU-meter indicator


## Benefits
- Ideal to performing other time critical tasks on the same MCU
- Use it on a low memory capacity MCU such as ATTiny85


## How to install
- Download zip
- Unpack it in the libraries folder
- Rename the folder to xtm1638
- Restart IDE
- Open example by using the example and search for xtml1638
- Open tm1638Example01
- Try to compile it (no errors = installed)


## Included examples
- tm1638Example01.ino - Hello world example
- tm1638Example02.ino - Clock example with hours, minutes, seconds and user interface
- tm1638Example03.ino - Characterset, gauge functionality (bars and indicators), orientation change
- tm1638Example04.ino - Dedicated to benchmarks (see benchmarks below)
- tm1638Example05.ino - Gauge, battery indicator, animation, orientation change


## Benchmarks

```
### PERFORMANCE STATS 
Running sketch: tm1386example04.ino (included stress test example)
Device/MCU    : Arduino Nano/ATMega328 
                [Link](http://www.atmel.com/images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet_Complete.pdf)
----------------------------------------------------------------------
 Mode:                         Score:       Minutes:           no-leds score:  no-leds minutes:
    Register+Multiply             94177ms      1.57 minutes        72077ms        1.20 minutes
    Register+Divide               86952ms      1.44 minutes        64852ms        1.08 minutes
    Register+ShiftDivide          76404ms      1.2734 minutes      58314ms        0.972 minutes(!)
    Register+ShiftDivide-PROGMEM  76374ms      1.2729 minutes      58283ms        0.971 minutes(!)
    Compatible Mode+Multiply      321876ms     5.36 minutes       232628ms        3.88 minutes
    Compatible Mode+Divide        314652ms     5.25 minutes       226404ms        3.77 minutes
    Compatible Mode+ShiftDivide   304093ms     5.07 minutes       219866ms        3.66 minutes

	----
	
   26 jan 2019 update: New records with caching (buffering)!
    Register+ShiftDivide+caching   39435ms     0.65725 minutes     22512ms        0.375 minutes(!) - speed gain: 1.9x/2.5x
    Compatible Mode+ShiftDivide+  142392ms     2.3732  minutes     59284ms        0.988 minutes(!) - speed gain: 2.1x/3.7x 
    caching                        
	
   29 jan 2019 update (1): New records by using assembler (ASM)!
	  ASM+ShiftDivide                37125ms     0.618 minutes(!)    30239ms        0.504 minutes(!) - speed gain: 2.1x/1.9x
	  ASM+ShiftDivide+caching        23889ms     0.398 minutes(!)    17004ms        0.283 minutes(!) - speed gain: 3.1x/3.4x

   29 jan 2019 update (2): New records by using assembler (ASM) + font-table in dynamic memory!    
	  ASM+ShiftDivide                37095ms     0.617 minutes(!)    30209ms        0.503 minutes(!) - speed gain: 2.1x/1.9x
	  ASM+ShiftDivide+caching        23860ms     0.397 minutes(!)    16974ms        0.282 minutes(!) - speed gain: 3.2x/3.5x
	 
	
   30 jan 2019:
    Another gain can be reached by using new define (which is default now), XTM_AVR_SHIFTWISE_DIVIDE_ASM.
	  I don't have the time right now to update the tables above again. Fastest speed is now 12756ms = 
	  0.213 minutes = 12.756 seconds to count up to 100000 and display it. To reach this, use fastest 
    config settings specified below.  	
   
	----
   
   FASTEST CONFIG (highest mem usage)     AVERAGE CONFIG (fast)            SLOWEST CONFIG (very slow compared to all others)
    # XTM_AVR_ASM_MODE					          # XTM_APPLY_CACHED_SEGMENTS      # XTM_ARDUINO_COMPATIBLE
	  # XTM_APPLY_CACHED_SEGMENTS			      # XTM_SHIFTWISE_DIVIDE           # XTM_ARITHMETIC_MULTIPLY
	  # XTM_NOPROGMEM
	  # XTM_SHIFTWISE_DIVIDE
    # XTM_AVR_SHIFTWISE_DIVIDE_ASM
	
```


## Compile size

Example tm1386example04.ino:
```
NOTICE: Results can vary by used MCU-type, compile settings, Arduino IDE and library

Compiled size on an Arduino Nano (ATMega328), this sketch with use of:

Ricardo Batista TM1638 library    : size 4992b  462b mem
----------------------------------
This xtm1638 library,
o With normal devide or multiply:
  - Portmode (AVR only)           : size 2812b  104b mem  (AVR direct port manipulation (=much faster))
  - Arduino compatible mode       : size 3104b  104b mem  (with digitalwrite etc)
  - Arduino compatible mode  \     
    without PROGMEM font table    : size 3098b  174b mem  
 
o With ShiftDivide mode:
  - Portmode (AVR only)           : size 3018b  116b mem  (AVR direct port manipulation (=much faster))
  - Arduino compatible mode       : size 3310b  116b mem  (with digitalwrite etc)
  - Arduino compatible mode  \     
    without PROGMEM font table    : size 3304b  186b mem  

30 jan 2019:
	Since there a few major changes, this table needs an update however I don't have the time right now to 
  update the tables above again. At least it compiles about 1K smaller than Batista's library.
```


## Usage

Example for the ATMEL AVR/MCU's
```
#include <xtm1638.h>
static xtm1638 ledandkey( PB0, PB1, PB2 );
ledandkey.setChars("Hello");

```

Example for the other Arduino's (compatible mode)
```
#include <xtm1638.h>
static xtm1638 ledandkey( 8, 9, 10 );
ledandkey.setChars("Hello");
```



## Extended usage example
```
 // On top of your sketch
#include <xtm1638.h>

#ifdef XTM_ARDUINO_COMPATIBLE  
 // Specify (digital) PINS as described on the board
 #define PIN_LEDKEY_DATAIO  8
 #define PIN_LEDKEY_CLOCK   9
 #define PIN_LEDKEY_STROBE  10
#else 
  // Specify register port PINS specified in the documentation of the board
 #define PIN_LEDKEY_DATAIO  PB0
 #define PIN_LEDKEY_CLOCK   PB1
 #define PIN_LEDKEY_STROBE  PB2
#endif

// Create the class object
static xtm1638 ledandkey( PIN_LEDKEY_DATAIO, 
                          PIN_LEDKEY_CLOCK, 
                          PIN_LEDKEY_STROBE 
                          );

.......
void setup()
{
 ledandkey.setChars("Hello");
 delay(2000);
}

void loop()
{
 ledandkey.setChars("Input?");
  
  // Be gentle
  delay(300);
  
  if( ledandkey.getButtonPressed() == XTM_BUTTON1 )
  {
    ledandkey.setChars("Hello");
    delay(2000);
    ledandkey.setChars("Again");
    delay(2000);
  }

  ...... etc
}

```


## Compiler messages

When compiling the code, the compiler informs you about current applied configuration. For example, when using port register on AVR's, you will see something like this:

```
libraries/xtm1638/xtm1638.h:85:74: note: #pragma message: Compiling 1638 H file: Port manipulation mode - PORTB

```
However, when compiling in compatible mode, you will see something like this:

```
libraries/xtm1638\xtm1638.h:123:75: note: #pragma message: Compiling 1638 H file: Arduino library compatible mode.

```  


## Config file

You are able to change the behaviour of the class by using the xtm1386.config.h file. Change this file only when there are issues on:
- Port settings
- Auto detection features (failing)
- Compatibility
- Speed or global performance
- Program versus dynamic memory usage


## Port configuration

When using the class with direct port access on ATMEL AVR/MCU, you need to specify register port names of the same register port. At default the register port is **PORTB** *(XTM_PORTB)*. If you want to use another port register, you must change the port option available in the included xtm1386.config.h file.

```
/* DEFINE A PORT TO USE (NOT REQUIRED WHEN COMPATIBLE MODE):
 -----------------------------------------------------------
  NOTE: All three pins used must be bits on the same PORT
  register (ex. PORTB).
 ------------------------------------------------------------------------
  PORTB (XTM_PORTB)           PORTD (XTM_PORTD)         PORTC (XTM_PORTC)
   PB0 D8                     PD0 D0/RX                 PC0 A0
   PB1 D9                     PD1 D1/TX                 PC1 A1
   PB2 D10                    PD2 D2                    PC2 A2
   PB3 D11/MOSI               PD3 D3                    PC3 A3
   PB4 D12/MISO               PD4 D4                    PC4 A4
   PB5 D13/SCK                PD5 D5                    PC5 A5
   PD6 D6
   PD7 D7

   Just uncomment only one port option:
*/
#define XTM_PORT XTM_PORTB
//#define XTM_PORT XTM_PORTC
//#define XTM_PORT XTM_PORTD

```
**NOTICE:** *When the class detects is not possible to use a configurated register port, it will switch to compatible mode!*


## If you don't want to deal with port registers at all

You can completely switch off the use of port registers in the included xtm1386.config.h file, however, you will not able to enjoy optimal performance.
```
/* XTM_ARDUINO_COMPATIBLE:
 -------------------------
 By enabling this (uncomment it), you force the xtm1638 class to use Arduino
 compatible mode always (disable auto detection), which means it performs the
 same on any Andruino library supported board/processor. Enable this when there
 are (detection) problems or for testing proposal/debugging.
 Enabling this, also for ATMEL AVR's, can address a significantly instantly
 drop in performance for any code compatible ATMEL device. If you want instant
 performance for any ATMEL device, you better don't enable this.
*/
#define XTM_ARDUINO_COMPATIBLE
```

## Notes and version history

If you have questions or found some serious issues, please post your findings at the issues tab on this project. 

### The why of this library
This software is based upon the AVR-Only TM1638 "library" of IronCreek Software. Although the original library is suitable to use, it lacks some flexibility, compatibility and features.

I have used the 'standard' [TM1638 library](https://github.com/rjbatista/tm1638-library) of Ricardo Batista, but isn't able to use direct port access. It is written for functionality but not for performance. Especially on low end devices or allot connected peripherals, this can be a serious issue. 

This xtm1638 library can be classified as the best of both worlds, the performance of the improved IronCreek library with the portability of the Batista library. 

```
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
```



