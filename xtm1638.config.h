#pragma no-cache

/* xtm1638 library compiler options
   Software related info in: xtm1638.h & xtml1638.h (please read it first)

   CHANGE THIS FILE ONLY WHEN THERE ARE ISSUES ON:
    o PORT SETTINGS
    o AUTO DETECTION FEATURES (FAILING)
    o COMPATIBILITY
    o SPEED OR GLOBAL PERFORMANCE
    o PROGRAM VERSUS DYNAMIC MEMORY USAGE


   PERFORMANCE STATS (Arduino Nano stress test - see xtm1638example04.ino)
   -----------------------------------------------------------------------
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
    # XTM_AVR_ASM_MODE					   # XTM_APPLY_CACHED_SEGMENTS      # XTM_ARDUINO_COMPATIBLE
	# XTM_APPLY_CACHED_SEGMENTS			   # XTM_SHIFTWISE_DIVIDE           # XTM_ARITHMETIC_MULTIPLY
	# XTM_NOPROGMEM
	# XTM_SHIFTWISE_DIVIDE
    # XTM_AVR_SHIFTWISE_DIVIDE_ASM
	
   NOTICE: Compile sizes and memory usage may vary between different versions of the Arduino IDE and library.	
*/

/* XTM_ARDUINO_COMPATIBLE: (default = disabled)
 -------------------------
 By enabling this (uncomment it), you force the xtm1638 class to use Arduino
 compatible mode always (disable auto detection), which means it performs the
 same on any Andruino library supported board/processor. Enable this when there
 are (detection) problems or for testing proposal/debugging.
 Enabling this, also for ATMEL/MICROCHIP AVR's, can address a significantly 
 instantly drop in performance for any code compatible AVR device. If you want 
 instant performance for any AVR device, you better don't enable this.
 NOTICE: You can improve performance significantly by enabling the define
         XTM_APPLY_CACHED_SEGMENTS below. 
*/
//#define XTM_ARDUINO_COMPATIBLE

 // Used when enabled and creating object without parameters:
#ifdef XTM_ARDUINO_COMPATIBLE
 #define XTM_ARD_AUTO_PIN_DATAIO 8
 #define XTM_ARD_AUTO_PIN_CLOCK  9
 #define XTM_ARD_AUTO_PIN_STROBE 10
#endif


/* XTM_AVR_ASM_MODE: (default = disabled)
 -------------------
 By enabling this (uncomment it), you force xtm1638 class to use optimized 
 assembler instructions to manipulate pins (AVR only). This is overall the 
 fastest method of all to use, however, requires an extra library called 
 FastGPIO (by Pololu Corporation) and you need to configure static pin 
 assignments below (not configurable via constructor). 
 Reason: The FastGPIO::Pin class provides static functions for manipulating pins.
 This class can only be used if the pin number is known at compile time, which 
 means it does not come from a variable that might change and it does not come 
 from the result of a complicated calculation.
 You can download the FastGPIO library from here:
 - https://github.com/pololu/fastgpio-arduino
*/
#define XTM_AVR_ASM_MODE

#ifdef XTM_AVR_ASM_MODE
 #define XTM_AVR_ASM_PIN_DATAIO 8
 #define XTM_AVR_ASM_PIN_CLOCK  9
 #define XTM_AVR_ASM_PIN_STROBE 10
#endif


/* XTM_APPLY_CACHED_SEGMENTS: (default = enabled)
 ----------------------------
 By enabling this (uncommented state), you enable the xtm1638 class to use 
 a 8 byte buffer to cache the content of the displays which means it performs 
 about 1.9x up to 3.7x faster (see also updated PERFORMANCE STATS table on top 
 of this file). 
 The cache saves writes to the bus when a char is already send once. This is
 a very effective way to avoid unnecessary delays and to unstress the MCU.
 Disable this only when almost out of memory, when you want to save some
 memory (8 bytes of dynamic memory), your project doesn't need SUPERB 
 performance or you discovered a real bug.  
*/
#define XTM_APPLY_CACHED_SEGMENTS


/* XTM_NOPROGMEM: (default = disabled)
 ----------------
 By enabling this (uncomment it), you force the xtm1638 class to use program/
 dynamic memory, instead of storage flash memory, it disables the auto
 detection feature. Disabling or enabling this feature does not effect or will
 suit any performance gain at general speed configurations (ASM_MODE and cache
 enabled will slightly). However, if capacity is low on storage memory but not 
 on dynamic memory, you can decide to enable this option permanently.
*/
//#define XTM_NOPROGMEM


/* XTM_ARITHMETIC_MULTIPLY: (default = disabled)
 --------------------------
 By enabling this (uncomment it), you force the xtm1638 class to use standard
 multiply arithmetic operations, it disables the "XTM_SHIFTWISE_DIVIDE" option
 (see below). Enable this when there are performance/compatibility problems
 or for testing proposal/debugging only.

 Enabling this is most of the time an overall PERFORMANCE DROP.
*/
//#define XTM_ARITHMETIC_MULTIPLY


/* XTM_SHIFTWISE_DIVIDE: (default = enabled)
   XTM_AVR_SHIFTWISE_DIVIDE_ASM: (default = enabled)
 -------------------------------
 By disabling this (comment it), you force the xtm1638 class to use standard
 multiply or divide arithmetic operations, it can be override by the previous
 listed "XTM_ARITHMETIC_MULTIPLY" option. Disable this when there are
 performance/compatibility problems or for testing proposal/debugging only.

 Disabling this is most of the time an overall PERFORMANCE DROP.
 
 To make it a little faster, you can decide to enable the assembler version
 of this (AVR only) by uncomment the XTM_AVR_SHIFTWISE_DIVIDE_ASM option. 
*/
#define XTM_SHIFTWISE_DIVIDE
#define XTM_AVR_SHIFTWISE_DIVIDE_ASM


/* DEFINE A PORT TO USE (NOT REQUIRED WHEN COMPATIBLE MODE OR ASM MODE):
 ------------------------------------------------------------------------
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

 
 // Used when creating object without parameters:
#define XTM_REG_DEF_PIN_DATAIO  PB0
#define XTM_REG_DEF_PIN_CLOCK   PB1
#define XTM_REG_DEF_PIN_STROBE  PB2


/* SOME DEFINES FOR YOU CONVENIENCE:
 -----------------------------------
 Disable, enable or add what you like
*/
#define PB4     4 //D12
#define PB3     3 //D11
#define PB2     2 //D10
#define PD5     5 //D5
#define PD4     4 //D4
#define PD3     3 //D3

