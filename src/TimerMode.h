/**
 * Name: TimerMode.h
 * Desc: A data structure holding a LabJack timer mode / clock value
 *		 and its corresponding string description
**/

//	Windows
#include "stdafx.h" 

class TimerMode
{
	public:
		TimerMode(void);
		TimerMode(long newLJValue, CString newDesc);
		CString GetDescription();
		long GetLabJackValue();

	private:
		CString desc;
		long ljValue;
};
