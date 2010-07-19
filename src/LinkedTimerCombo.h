/**
 * Name: LinkedTimerCombo.h
 * Desc: A very limitedlinked list that holds and configures 
 *		 comboboxes for timer values
**/

// TODO: It seems like inheritance would be a bit more graceful.
// However, .Net seems a bit more graceful to and the visual designer
// was choosen over using absolute values

/**
 * Name: LinkedTimerCombo
 * Desc: A linked list that holds and configures comboboxes 
 *		 for timer values
**/

//	Windows
#include "stdafx.h" 
#include <string.h>

// Application
#include "TimerMode.h"

// LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

class LinkedTimerCombo
{
	CComboBox * comboBox;
	LinkedTimerCombo * nextNode;

	public:
		LinkedTimerCombo(CComboBox * newComboBox, LinkedTimerCombo * newNode);
		LinkedTimerCombo(CComboBox * newComboBox);
		void SetComboBox(CComboBox * newComboBox);
		CComboBox * GetComboBox();
		void SetNext(LinkedTimerCombo * newNode);
		LinkedTimerCombo * GetNext();

	private:
		void init(CComboBox * newComboBox, LinkedTimerCombo * newNode);
};
