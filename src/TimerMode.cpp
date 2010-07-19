/**
 * Name: TimerMode.h
 * Desc: A data structure holding a LabJack timer mode / clock value
 *		 and its corresponding string description
**/

#include <cstring>
#include "stdafx.h" 

#include "TimerMode.h" 

/**
 * Name: TimerMode()
 * Desc: The default constructor for the TimerMode object
**/
TimerMode::TimerMode()
{
	// TODO: Raise exception?
}

/**
 * Name: TimerMode(newLJValue, newDesc)
 * Desc: Creates a new immutable data structure to hold
 *		 a LabJack timer value and its description
**/
TimerMode::TimerMode(long newLJValue, CString newDesc)
{
	desc = newDesc;
	ljValue = newLJValue;
}

/**
 * Name: TimerMode.getDescription()
 * Desc: Returns a string description of this timer mode
**/
CString TimerMode::GetDescription()
{
	return desc;
}

/**
 * Name: TimerMode.GetLabJackValue();
 * Desc: Get the long value representation of this timer mode
		 for use with the LabJack UD driver
**/
long TimerMode::GetLabJackValue()
{
	return ljValue;
}
