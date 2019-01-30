/*
||
|| @file 	TTimer.cpp
|| @version	1.6
|| @author	Erwin Haantjes
|| @contact	erwin@illumation.net
||
|| @description
|| | Provide an easy way of triggering functions at a set interval
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#ifndef TIMER_H
#define TIMER_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

class TTimer 
{
  public:
   unsigned long tag = 0; 
    
    TTimer(unsigned long ulInterval, bool bAutoReset = true, bool bCycleMode = false);
    TTimer(unsigned long ulCurrent, unsigned long ulInterval, bool bAutoReset = true, bool bCycleMode = false);
	
	void reset();
	void disable();
	void enable();
  bool setInterval( unsigned long ulInterval );
	bool setCycleMode( bool bCycleMode = true );

	bool isActive();
	bool isTime( bool bForceTimeResetIfTime = false );
  bool isCycleMode();
  unsigned long getInterval();

  private: 
    bool active;
    bool autoreset;
    bool cyclemode;
    unsigned long previous;
    unsigned long interval;
};

#endif


