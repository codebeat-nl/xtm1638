#include "Timer.h"

/* constructors */
TTimer::TTimer(unsigned long ulInterval, bool bAutoReset, bool bCycleMode )
{
  autoreset = bAutoReset;
  cyclemode = bCycleMode;
  active    = true;
	previous  = 0;
	interval  = ulInterval;
}

TTimer::TTimer(unsigned long ulCurrent,unsigned long ulInterval, bool bAutoReset, bool bCycleMode )
{
  autoreset = bAutoReset;
  cyclemode = bCycleMode;
  active    = true;
	previous  = ulCurrent;
	interval  = ulInterval;
}

 /* Operation functions */
 void TTimer::reset()
{ 
  if( cyclemode )
   {  previous = interval; }
  else { previous = millis(); } 
}

void TTimer::disable()
{ active = false; }

void TTimer::enable()
{ active = true; }

bool TTimer::setInterval( unsigned long ulInterval)
{
  if( ulInterval > 0 )
  { 
    interval = ulInterval; 
    reset();
    return true;
  }
  return false;
}

bool TTimer::setCycleMode( bool bCycleMode )
{
  if( bCycleMode != cyclemode )
  { 
    cyclemode = bCycleMode; 
    reset();
    return true;
  }
  return false;
}


 /* status functions */
bool TTimer::isActive()
{ return (interval > 0 && active); }

bool TTimer::isCycleMode()
{ return (interval > 0 && active); }

bool TTimer::isTime( bool bForceTimeResetIfTime )
{
  if( isActive() && (( cyclemode && (previous == 0 || (--previous == 0))) || ( !cyclemode && (millis()-previous >= interval) ))  ) 
  {
    if( autoreset || bForceTimeResetIfTime )
     { reset(); }
    return true;
  }
  return false;
}

unsigned long TTimer::getInterval()
{ return interval; }


