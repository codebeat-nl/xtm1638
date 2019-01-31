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
  - Date   : 20-may-2017 (v2.00)
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
  - http://github.codebeat.nl           ; Github (projects)
  
  Thank you for supporting if you do!

  Happy coding, greetz,
  Erwin Haantjes (codebeat) 
*/

#include "xtm1638.h"


#ifdef XTM_SHIFTWISE_DIVIDE
namespace
{

    uint32_t ___q;

    #if defined(__AVR__) && defined(XTM_AVR_SHIFTWISE_DIVIDE_ASM)
   uint8_t ___x;
   
    //void divmod10(uint32_t in, uint32_t &div, uint8_t &mod) __attribute__((noinline));
   void _avrdivmod10(uint32_t in, uint32_t &div, uint8_t &mod) //__attribute__((noinline))
   {
      //assumes that div/mod pointers arrive in r18:r19 and r20:r21 pairs (doesn't matter which way around)
      //and that in arrives in r22:r25 quad
      asm volatile(
      "movw r30, %2   \n\t"  //uint32_t* divPtr = &div;
      "movw r26, %1    \n\t" //uint32_t* modPtr = &mod;
   
      "mov   r0, %A0  \n\t"  //byte temp = in
      "movw r18, %A0  \n\t"  //uint32_t q = in;
      "movw r20, %C0  \n\t"  
      "ori  r18, 0x01 \n\t"  //q |= 1;
   
      "lsr  r25       \n\t"  //x = in >> 2 //note: x reuses registers of 'in', as 'in' was backed up in r0
      "ror  r24       \n\t"
      "ror  r23       \n\t"
      "ror  r22       \n\t"
      "lsr  r25       \n\t"  
      "ror  r24       \n\t"
      "ror  r23       \n\t"
      "ror  r22       \n\t"
   
      "sub  r18, r22  \n\t" //q = q - x;
      "sbc  r19, r23  \n\t"
      "sbc  r20, r24  \n\t"
      "sbc  r21, r25  \n\t"
   
      "movw r22, r18  \n\t" //x = q;
      "movw r24, r20  \n\t"
      "lsr  r25       \n\t" //x = x >> 4;
      "ror  r24       \n\t"
      "ror  r23       \n\t"
      "ror  r22       \n\t"
      "lsr  r25       \n\t"
      "ror  r24       \n\t"
      "ror  r23       \n\t"
      "ror  r22       \n\t"
      "lsr  r25       \n\t"
      "ror  r24       \n\t"
      "ror  r23       \n\t"
      "ror  r22       \n\t"
      "lsr  r25       \n\t"
      "ror  r24       \n\t"
      "ror  r23       \n\t"
      "ror  r22       \n\t"
   
      "add  r22, r18  \n\t" //x = x + q
      "adc  r23, r19  \n\t"
      "adc  r24, r20  \n\t"
      "adc  r25, r21  \n\t"
   
      "movw r18, r22  \n\t" //q = x
      "movw r20, r24  \n\t"
      "add  r18, r23  \n\t" //q = q + (x >> 8)
      "adc  r19, r24  \n\t"
      "adc  r20, r25  \n\t"
      "adc  r21, r1   \n\t"

      "movw r18, r20  \n\t" //q = q >> 16
      "eor  r20, r20  \n\t"
      "eor  r21, r21  \n\t"
      "add  r18, r23  \n\t" //q = q + (x>>8)
      "adc  r19, r24  \n\t"
      "adc  r20, r25  \n\t"
      "adc  r21, r1   \n\t" //NOTE: r1 is a known 0.
      "add  r18, r22  \n\t" //q = q + x
      "adc  r19, r23  \n\t"
      "adc  r20, r24  \n\t"
      "adc  r21, r25  \n\t"
   
      "mov  r18, r19  \n\t" //q = q >> 8
      "mov  r19, r20  \n\t"
      "mov  r20, r21  \n\t"
      "eor  r21, r21  \n\t"
      "add  r18, r22  \n\t" //q = q + x
      "adc  r19, r23  \n\t"
      "adc  r20, r24  \n\t"
      "adc  r21, r25  \n\t"
   
      "andi r18, 0xF8 \n\t" //q = q & ~0x7
   
      "sub   r0, r18  \n\t" //in = in - q
   
      "lsr  r21       \n\t" //q = q >> 2
      "ror  r20       \n\t"
      "ror  r19       \n\t"
      "ror  r18       \n\t"
      "lsr  r21       \n\t"
      "ror  r20       \n\t"
      "ror  r19       \n\t"
      "ror  r18       \n\t"
   
      "sub  r0, r18   \n\t" //in = in - q
      "st    X, r0    \n\t" //mod = in;
   
      "lsr  r21       \n\t" //q = q >> 1
      "ror  r20       \n\t"
      "ror  r19       \n\t"
      "ror  r18       \n\t"
   
      "st      Z, r18  \n\t" //div = q
      "std  Z+1, r19  \n\t"
      "std  Z+2, r20  \n\t"
      "std  Z+3, r21  \n\t"
   
      :
      : "r" (in), "r" (&mod), "r" (&div)
      : "r0", "r26", "r27", "r31", "r31"
      );
   }   
   
   
   #else
    uint32_t ___t;
   uint32_t ___x;
   #endif
   
    // This cannot be optimized in C++, so don't try it, you are wasting your time, sure ;-)

    void _bit32Div10(uint32_t* div)
    {
        #if defined(__AVR__) && defined(XTM_AVR_SHIFTWISE_DIVIDE_ASM)
         _avrdivmod10( *div, *div, ___x );
      #else
         ___x= (*div|1) - (*div>>2); // div = in/10 <~~> div = (0.75*in) >> 3
        ___q= (___x>>4) + ___x;
        ___x= ___q;
        ___q= (___q>>8) + ___x;
        ___q= (___q>>8) + ___x;
        ___q= (___q>>8) + ___x;
        ___q= (___q>>8) + ___x;

        *div = (___q >> 3);
        #endif
   }

    uint8_t _bit32Mod10(uint32_t* mod)
    {
       #if defined(__AVR__) && defined(XTM_AVR_SHIFTWISE_DIVIDE_ASM)
         _avrdivmod10(*mod, ___q, ___x );
        return ___x;
      #else
        _bit32Div10(&(___t= *mod));
        return (uint8_t)(*mod - (((___t << 2) + ___t) << 1));
      #endif
   }
 
}
#endif

xtm1638::xtm1638( uint8_t iDataIoPin, uint8_t iClockPin, uint8_t iStrobePin )
{
  #ifdef XTM_AVR_ASM_MODE
    #pragma message("WARNING: Calling constructor with parameters make no sense when compiled in ASM mode!")
    reset();
  #else
   setup( iDataIoPin, iClockPin, iStrobePin );
  #endif   
}

xtm1638::xtm1638()
{
   #ifdef XTM_ARDUINO_COMPATIBLE
    
      // #pragma message("NOTICE: Compiled with defaults pins specified in xtm1638.config.h")
     setup( XTM_ARD_AUTO_PIN_DATAIO,
             XTM_ARD_AUTO_PIN_CLOCK,
             XTM_ARD_AUTO_PIN_STROBE
         ); 
   #else 
      #ifndef XTM_AVR_ASM_MODE 
         // #pragma message("NOTICE: Compiled with default register pins specified in xtm1638.config.h")
         setup( XTM_REG_DEF_PIN_DATAIO,
               XTM_REG_DEF_PIN_CLOCK,
               XTM_REG_DEF_PIN_STROBE
             ); 
      
      #else
       #ifdef XTM_AVR_ASM_MODE 
         #pragma message("NOTICE: Compiled with default ASM pins specified in xtm1638.config.h")
       #endif
       reset();
      #endif
   
   #endif
}


void xtm1638::setup( uint8_t iDataIoPin, uint8_t iClockPin, uint8_t iStrobePin )
{
    #ifndef XTM_AVR_ASM_MODE 
      _pinDataIO = iDataIoPin;
      _pinClock  = iClockPin;
      _pinStrobe = iStrobePin;
   #endif
   
   reset();
}


void xtm1638::reset()
{
    XTM_SETUP();

    sendCommand(XTM_DATA_CMD | XTM_WRITE_DISP);
    sendCommand(XTM_DISP_CTRL | XTM_DISP_ENABLE | XTM_DISP_PWM_MASK);

    XTM_INIT();
    
    clear();
    clearLEDs();
}


void xtm1638::send(uint8_t b)
{
    for (uint8_t i = 8; i; --i, b >>= 1)
    {
        XTM_CLK_LOW();
        if (b & 1)
         { XTM_DAT_HIGH(); }
        else { XTM_DAT_LOW(); }
        XTM_CLK_HIGH();
    }
}

void xtm1638::sendCommand(uint8_t cmd)
{
    XTM_STB_LOW();
    send(cmd);
    XTM_STB_HIGH();
}

void xtm1638::sendData(uint8_t addr, uint8_t data)
{
    sendCommand(XTM_DATA_CMD | XTM_FIXED_ADDR);
    XTM_STB_LOW();
    send(XTM_ADDR_CMD | addr);
    send(data);
    XTM_STB_HIGH();
}

uint8_t xtm1638::receive()
{
    uint8_t rc = 0;

    // Change DAT pin to INPUT and enable pull-up
    XTM_START_RECEIVE();

    for (uint8_t i = 8, b = 1; i; --i, b <<= 1)
    {
        XTM_CLK_LOW();

        // Not required when in compatible mode
        #ifndef XTM_ARDUINO_COMPATIBLE
         // Must wait tWAIT for CLK transition
         _delay_us(1);
        #endif
            
        if( XTM_COMPARE_RECEIVED() )
         { rc |= b; }

        XTM_CLK_HIGH();
    }

        // Disable pull-up and reset pin
    XTM_STOP_RECEIVE();

    return rc;
}

void xtm1638::clear()
{
   int8_t i = XTM_REG_MAX+1;
   while( i >= 0 )
   {
      if( i > 0 )
      { sendData(i, 0x00); }
     #ifdef XTM_APPLY_CACHED_SEGMENTS
      if( i < 8 )
      { _segbuff[i] = _segbuff[i+1] = 0x00; }
     #endif
      i-=2;
   }   
}

void xtm1638::setDisplay( bool bEnabled, uint8_t iBrightness )
{
    XTM_STB_HIGH();
    XTM_CLK_HIGH();
 
    sendCommand( XTM_DATA_CMD );
    sendCommand( XTM_DISP_CTRL | ( bEnabled ? 8 : 0) | ((iBrightness >= 0 && iBrightness < 8)?iBrightness:7) );

    XTM_STB_LOW();
}

void xtm1638::setOrientation( bool bUpsideDown )
{
   _orient = bUpsideDown?XTM_ORIENT_UPSIDEDOWN:XTM_ORIENT_NORMAL;
}


uint8_t xtm1638::rotateByte( uint8_t value )
{
    return ( value & 0xC0 | (value & 0x07) << 3 | (value & 0x38) >> 3 );
}

void xtm1638::setByte(uint8_t pos, uint8_t value)
{
    if( _orient == XTM_ORIENT_UPSIDEDOWN )
    {
       pos = 7-pos;
       if( value > 0 )
        { value = rotateByte( value ); }
    }

   
   if( pos < 8 )
   #ifdef XTM_APPLY_CACHED_SEGMENTS
    if( _segbuff[pos] != value )
    {
       _segbuff[pos] = value | (_dotMask & (1 << pos) ? XTM_DOT : 0);
       sendData(pos << 1, _segbuff[pos] );
    }
   #else 
     sendData(pos << 1, value | (_dotMask & (1 << pos) ? XTM_DOT : 0) );
   #endif
}

void xtm1638::setBytes(const char* value, uint8_t offset)
{
    while (*value && offset < 8 )
     { setByte(offset++, *value++); }

    while( offset < 8 )
     { setByte(offset++, 0x00 ); }
}



uint8_t xtm1638::getOffsetDigits(uint32_t number)
{
    uint8_t digits = 0;
    while (number >= 10)
    {
      XTM_32_MULDIV_NUMBER_BY_10;

      ++digits;
    }
    return digits;
}


void xtm1638::setDigit(uint8_t pos, uint8_t value)
{
    setByte(pos, XTM_GET_DIGIT(value & (uint8_t)0xF) );
}

void xtm1638::setNumber(uint32_t number, uint8_t offset, uint8_t align)
{
    if( number == 0 )
   {
      setDigit( offset, 0 ); 
      return;
   }
   
   if (align == XTM_LEFT)
        offset += getOffsetDigits(number);

    while (number && offset != (uint8_t)0xFF)
    {

        #ifdef XTM_32_MODMULDIV_NUMBER_BY_10
       setDigit(offset--, XTM_32_MODMULDIV_NUMBER_BY_10 );
      #else
       setDigit(offset--, XTM_32_MODDIV_NUMBER_BY_10 );

         XTM_32_MULDIV_NUMBER_BY_10;
      #endif
    }
}

void xtm1638::setSignedNumber(int32_t number, bool bShowPlusSign, uint8_t offset, uint8_t align)
{
    uint8_t iSignChar = XTM_PLUS;
   
    if( number < 0 )
    {
      iSignChar     = XTM_MINUS;
      bShowPlusSign = true;
      number*=-1;
    }
   
    if( bShowPlusSign && number > 0 )
    {
      if( align == XTM_LEFT )
       { ++offset; }   
    }
    
   setNumber( number, offset, align );
   
    if( bShowPlusSign && number > 0 )
    {
      if( align == XTM_LEFT )
       { setByte( offset-1, iSignChar ); }
      else {    setByte( offset-getOffsetDigits(number)-1, iSignChar ); }
    }
}


void xtm1638::setNumberPad(uint32_t number, uint8_t offset, uint8_t width, uint8_t pad)
{
    while (number && width-- && offset != (uint8_t)0xFF)
    {
        #ifdef XTM_32_MODMULDIV_NUMBER_BY_10
       setDigit(offset--, XTM_32_MODMULDIV_NUMBER_BY_10 );
      #else
       setDigit(offset--, XTM_32_MODDIV_NUMBER_BY_10 );

         XTM_32_MULDIV_NUMBER_BY_10;
      #endif 
    }

    while (width-- && offset != (uint8_t)0xFF)
        setByte(offset--, pad);
}

void xtm1638::setLzNumber(uint32_t number, uint8_t offset, uint8_t nDigits)
{
   setNumberPad(number, offset, nDigits, XTM_PAD_0 );
}

void xtm1638::setNumberHex(uint32_t number, uint8_t offset, uint8_t width, uint8_t pad)
{
    while (number && width-- && offset != (uint8_t)0xFF)
    {
        setDigit(offset--, number & 0x0F);
        number >>= 4;
    }

    while (width-- && offset != (uint8_t)0xFF)
        setByte(offset--, pad);
}

void xtm1638::setChar(uint8_t pos, const char value)
{
  uint8_t i = (uint8_t)0xFF;
  char c    = 0;

  if (value >= '0' && value <= '9')
   { i = value - '0'; }
  else {
        if (value >= 'a' && value <= 'z')
         { i = value - 'a' + 10; }
        else {
               if(value >= 'A' && value <= 'Z')
                 { i = value - 'A' + 10; }
             }
       }

  if( i != (uint8_t)0xFF )
  { setByte(pos, XTM_GET_DIGIT(i) ); }
  else {
         i = XTM_SPECIAL_CHAR_OFFSET;

         while( ((c = XTM_GET_DIGIT(i))!=value) && c > 0 )
          { i+=2; }

         setByte( pos, c?XTM_GET_DIGIT(i+1):XTM_CHAR_ERR );
      }
}

void xtm1638::setChars(const char* value, uint8_t offset, bool bClrScr )
{
    while (*value && offset < 8 )
        setChar(offset++, *value++);

    if( bClrScr )
   while( offset < 8 )
     { setByte(offset++, 0x00 ); }
}

void xtm1638::setAlignedChars(const char* value, uint8_t align, bool bClrScr )
{
   if( align == XTM_LEFT )
    { setChars( value, 0 ); }
   else { 
         if( *value )
         { setChars( value, 8-getStrLen(value), bClrScr ); }    
       }    
}   

void xtm1638::setDots(uint8_t mask)
{
    _dotMask = mask;
}

void xtm1638::clearLED(uint8_t pos)
{
   if( _orient == XTM_ORIENT_UPSIDEDOWN )
    { pos=7-pos; }
   
   sendData(XTM_REG_LED_OFFSET+(pos*2), 0x00);
}

void xtm1638::clearLEDs()
{
    for (uint8_t a = XTM_REG_LED_OFFSET; a <= XTM_REG_MAX; a += 2)
      sendData(a, 0x00);
}

void xtm1638::setLED(uint8_t pos, uint8_t color)
{
   if( _orient == XTM_ORIENT_UPSIDEDOWN )
    { pos=7-pos; }
   
   
   sendData(XTM_LED_TO_REG(pos), color);
}

void xtm1638::setLEDs(uint8_t green, uint8_t red)
{
    for (uint8_t a = XTM_REG_LED_OFFSET, b = 1; a <= XTM_REG_MAX; a += 2, green >>= 1, red >>= 1)
        sendData(a, green & 1  | ((red & 1) << 1));
}

uint8_t xtm1638::getButtons()
{
  uint8_t iButtons = 0;

   XTM_STB_LOW();
   send(XTM_DATA_CMD | XTM_READ_KEYS);

   for(uint8_t bytes = 0; bytes != 4; ++bytes)
    iButtons |= receive() << bytes;

   XTM_STB_HIGH();

   if( _orient == XTM_ORIENT_UPSIDEDOWN )
   {
     // swap each other
     iButtons = (iButtons & 0b01010101) <<  1 | (iButtons & 0b10101010) >>  1;

     // swap each pair
     iButtons = (iButtons & 0b00110011) <<  2 | (iButtons & 0b11001100) >>  2;

     // swap each quad
     iButtons = (iButtons & 0b00001111) <<  4 | (iButtons & 0b11110000) >>  4;
   }

   return iButtons;
 }

bool xtm1638::isButtonPressed( uint8_t iPos, uint8_t iButtons  )
{
  if( iButtons == 0 )
   { iButtons = getButtons();
     if( iButtons == 0 )
      { return false; }
   }
  return (iButtons & (0x1 << iPos))?true:false;
}

uint8_t xtm1638::getStrLen( const char* value )
{
  uint8_t iResult = 0;
  while (*value && iResult++ < 8 ) { *value++; }
  return iResult;
}


uint8_t xtm1638::getButtonPressed()
{
  uint8_t iButtons = getButtons();
  uint8_t i = 0;

  while( i < 8 )
  {
    if( isButtonPressed(i++, iButtons ) )
     { return (i-1); }
  }

  return XTM_NOBUTTON;
}

bool  xtm1638::waitForButtonPressed( uint8_t iButton, uint8_t iTimeOutSec )
{
  uint8_t iWaitState = 0;

  if( iTimeOutSec > 0 )
  {
    if( iTimeOutSec > 50 )
     { iTimeOutSec = 50; }

    iTimeOutSec*=5;
  }
  
  while( getButtonPressed() != iButton  )
  {
    #ifndef XTM_ARDUINO_COMPATIBLE
     _delay_us( 200000UL );
    #else
      delay( 200 );
    #endif

    if( iTimeOutSec > 0 )
     { ++iWaitState;
       if( iWaitState > (uint8_t)254 || iWaitState > iTimeOutSec )
        { return false; }
     }
  }

  return true;
}

bool xtm1638::waitForNoButtonPressed( uint8_t iTimeOutSec )
{ return waitForButtonPressed( XTM_NOBUTTON, iTimeOutSec ); }



void xtm1638::gauge( uint8_t iPeakPerc, uint8_t iFirstPerc, uint8_t iSecondPerc,
                     uint8_t iStyle, uint8_t iSubStyle )
{
    bool    bDivide         = ( iSubStyle != XTM_GAUGE_SUBSTYLE_NORMAL );
    bool    bDivideInCenter = bDivide?( iSubStyle != XTM_GAUGE_SUBSTYLE_INBOUND ):false;
    bool    bPipe           = (iStyle == XTM_GAUGE_STYLE_PIPE);
    uint8_t iSegs           = bDivide?(bPipe?8:4):(bPipe?16:8);
    uint8_t iDivSegs        = bDivide?(bPipe?4:iSegs):iSegs;
    bool    bTwoSegs        = bPipe || (iStyle == XTM_GAUGE_STYLE_STRIPE);
    bool    bBulletTop      = bTwoSegs?false:(iStyle == XTM_GAUGE_STYLE_BULLET_TOP);
    bool    bCenterLine     = bTwoSegs?false:(iStyle == XTM_GAUGE_STYLE_CENTER_LINE);

    if( iSecondPerc == XTM_GAUGE_SINGLE )
     { iSecondPerc = iFirstPerc; }

    if( iFirstPerc > 100 )
     { iFirstPerc = 100; }
    if( iSecondPerc > 100 )
     { iSecondPerc = 100; }
    if( iPeakPerc > 100 )
     { iPeakPerc = 100; }

    #ifdef XTM_ARITHMETIC_MULTIPLY
      iFirstPerc  = (iFirstPerc>0)?((uint8_t)round((iSegs * 0.01) * iFirstPerc )):0;
      iSecondPerc = (iSecondPerc>0)?((uint8_t)round((iSegs * 0.01) * iSecondPerc )):0;
      iPeakPerc   = (iPeakPerc>0)?((uint8_t)round((iSegs * 0.01) * iPeakPerc )):0;
    #else
      iFirstPerc  = (iFirstPerc>0)?((uint8_t)round((iSegs / 100.0) * iFirstPerc )):0;
      iSecondPerc = (iSecondPerc>0)?((uint8_t)round((iSegs / 100.0) * iSecondPerc )):0;
      iPeakPerc   = (iPeakPerc>0)?((uint8_t)round((iSegs / 100.0) * iPeakPerc )):0;
    #endif
 
    uint8_t i     = 0;
    uint8_t iPos  = 0;
    uint8_t c;
    uint8_t iLeft;
    uint8_t iRight;
    bool    bLeft;
    bool    bRight;
 
    //clearLEDs();
 
    while( ++i <= iSegs )
    {
       bLeft  = ( iFirstPerc  && iFirstPerc >= i  );
       bRight = ( iSecondPerc && iSecondPerc >= i );
    
       if( bDivide )
       {
          if( bPipe )
          {
             // 27-jan-2019: Quick fix of position
             iLeft  = (bDivideInCenter?(3-iPos):iPos);
             iRight = (bDivideInCenter?(4+iPos):7-iPos);
        }   
        else {
               iLeft  = (bDivideInCenter?(iDivSegs-i):(i-1));
               iRight = (bDivideInCenter?(iDivSegs+i-1):((iDivSegs*2)-i));
             }
        }
        else { iLeft = i-1; }
    
        if( iStyle == XTM_GAUGE_STYLE_LED )
        {
           if( bLeft || bRight || (i == iPeakPerc) )
           {
               if( bDivide )
               {
                  if( bLeft )
                   { setLED( iLeft ); }
                  else { clearLED( iLeft ); } 
                  if( bRight )
                   { setLED( iRight ); }
                  else { clearLED( iRight ); }  
                }
               else { setLED( iLeft ); }
        }
        else {
                clearLED( iLeft );
                if( bDivide )
                  { clearLED( iRight ); }
             }

        continue;
    }

    c = (uint8_t)0x00;

    if( bTwoSegs ) // Stripe or Pipe character?
    {
       // inbound: bDivide && !bDivideInCenter
       // center : bDivide && bDivideInCenter
      
      
        if( !bDivide ) // Two channels in one character?
        {
            if( bLeft )
             { c+=bPipe?(uint8_t)0x20:(uint8_t)0x01; } // Add | left-top or -- at top
         
            if( bRight )
             { c+=bPipe?(uint8_t)0x10:(uint8_t)0x08; } // Add | left-bottom or -- at bottom
        }
        else {
                 if( (bLeft && bRight) || bLeft || bRight )
                 { 
                     c+=bPipe?(uint8_t)0x30:(uint8_t)0x09;  // Add | left-top or -- at top
                                                           // Add | left-bottom or -- at bottom
                 }   
             }
         
        // Pipe can hold two values (doubled resolution) in one character, get and set next one
        if( bPipe && (bLeft || bRight) ) 
        {
            // Set peak value
            if( iPeakPerc == i )
             { c+=0x80; }
      
            // Get next    
            ++i;

            if( !bDivide ) // Two channels in one character?
            {
               if( iFirstPerc && iFirstPerc >= i )
                { c+=(uint8_t)0x02; } // Add | right-top 
               if( iSecondPerc && iSecondPerc >= i) 
                { c+=(uint8_t)0x04; } // Add | right-bottom 
            }
            else {
                    if( iFirstPerc && iFirstPerc >= i || iSecondPerc && iSecondPerc >= i )
                    {
                       c+=(uint8_t)0x06; // Add | right-top && add | right-bottom 
                    }
                 }
        }

        // Set peak value
        if( iPeakPerc == i && c < 0x40 )
         { c+=bPipe?(uint8_t)0x80:0x40; }

        if( bDivide )
        {
            setByte( iLeft , bLeft ?((bPipe &&  bDivideInCenter)?rotateByte(c):c):0x00 );
            setByte( iRight, bRight?((bPipe && !bDivideInCenter)?rotateByte(c):c):0x00 );
        }
        else { setByte( iPos, c ); }
    }
    else {
            uint8_t iChar     = bCenterLine?0x40:(bBulletTop?(uint8_t)0x63:(uint8_t)0x5C);
            uint8_t iPeakChar = bCenterLine?0x80:(bBulletTop?(uint8_t)0x08:(uint8_t)0x01);
         
            if( bDivide )
            {
              // Center line and first one?
                if( bCenterLine && iPos == 0 )
                {
                    c+=(bDivideInCenter)?(uint8_t)0x06:(uint8_t)0x30; // Add |
                }
          
                if( bLeft )
                 { c+=iChar+(( iPeakPerc == i )?iPeakChar:0); }

                setByte( iLeft , c );
                c=0x00;
              
                // Center line and first one?
                if( bCenterLine && iPos == 0 )
                {
                  c+=(bDivideInCenter)?(uint8_t)0x30:(uint8_t)0x06; // Add |
                }

                if( bRight )
                 { c+=iChar+(( iPeakPerc == i )?iPeakChar:0); }


                setByte( iRight , c );
            }
           else {
                    // Center line and first one?
                    if( bCenterLine && iPos == 0 )
                    {
                      c+=(uint8_t)0x30; // Add |
                    }
                 
                    if( bLeft || bRight )
                     { c+=iChar+(( iPeakPerc == i )?iPeakChar:0); }

                    setByte( iPos, c );
                }
         }

        ++iPos;
    }

}



