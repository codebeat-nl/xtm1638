# Fastest library to control TM1638 chip (for example: "LED AND KEY") based modules, using (optional) direct port access on ATMEL MCU's.


## Category
- Arduino C++
- Library, class xtm1638
- TM1638
- LED and KEY


## About
The TM1638 can be found in 8 digit, 7-segment LED displays which also incorporate 8 buttons as well as bi-colored red and green LEDs. This wrapper class library allows you to control the module with ease, using MCU's optimal direct port access on AVR's or in "Arduino library compatible mode" on any other 'Arduino'. Usage demo sketches included.


## Features
- Significantly faster than existing implementations (see also benchmark results below)
- Configurable ports and pins
- Optimized arithmic operations
- SuperB speed on ATMEL AVR by using direct port access
- Auto detection when compiling for optimal configuration
- Arduino library compatible mode (use it on any 'Arduino')
- Easy to use class methods
- Light weight, small memory footprint
- Compiler configuration messages when compiling 
- Configuration file included
- Example, usage sketches/demo's included


## Application usage cases
- Counters and clocks which requires low latency
- Gear control
- Performing other time critical tasks on the same MCU
- Use it on a low memory capacity MCU such as ATTiny85


## How to install
- Download zip
- Unpack it in the libraries folder
- Rename the folder to xtm1638
- Restart IDE
- Open example by using the example and search for xtml1638
- Open tm1386Example01
- Try to compile it (no errors = installed)


## Benchmarks

```
### PERFORMANCE STATS 
Running sketch: tm1386example04.ino (included stress test example)
Device/MCU    : Arduino Nano/ATMega328 
                [Link](http://www.atmel.com/images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet_Complete.pdf)
----------------------------------------------------------------------
Mode:                         Score:       Minutes:           no-leds score:  no-leds minutes:
 Ricardo Batista TM1638 lib    448676ms     7.48 minutes(!)    363178ms        6.05 minutes(!)       

---------- this xtm1638 library:
 Register+Multiply             94177ms      1.57 minutes        72077ms        1.20 minutes
 Register+Divide               86952ms      1.44 minutes        64852ms        1.08 minutes
 Register+ShiftDivide*         76404ms      1.2734 minutes      58314ms        0.972 minutes(!)
 Register+ShiftDivide-PROGMEM  76374ms      1.2729 minutes      58283ms        0.971 minutes(!)
 Compatible Mode+Multiply      321876ms     5.36 minute        232628ms        3.88 minutes
 Compatible Mode+Divide        314652ms     5.25 minutes       226404ms        3.77 minutes
 Compatible Mode+ShiftDivide   304093ms     5.07 minutes       219866ms        3.66 minutes

*) = default
```


## Compile size

Example tm1386example04.ino:
```
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
 #define PIN_DIG_LEDKEY_DATAIO  8
 #define PIN_DIG_LEDKEY_CLOCK   9
 #define PIN_DIG_LEDKEY_STROBE  10
#else 
  // Specify register port PINS specified in the documentation of the board
 #define PIN_REG_LEDKEY_DATAIO  PB0
 #define PIN_REG_LEDKEY_CLOCK   PB1
 #define PIN_REG_LEDKEY_STROBE  PB2
#endif

.......
void setup()
{
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
  
 ledandkey.setChars("Hello");
}

void loop()
{
  // ......
}

```


## Compiler messages

When compiling the code, the compiler informs you about current applied configuration. For example, when using port register on ATMEL AVR's, you will see something like this:

```
libraries/xtm1638/xtm1638.h:85:74: note: #pragma message: Compiling 1638 H file: Port manipulation mode - PORTB

   *#pragma message("Compiling 1638 H file: Port manipulation mode - PORTB")*
```
However, when compiling in compatible mode, you will see something like this:

```
libraries/xtm1638\xtm1638.h:123:75: note: #pragma message: Compiling 1638 H file: Arduino library compatible mode.

  #pragma message("Compiling 1638 H file: Arduino library compatible mode.")
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



