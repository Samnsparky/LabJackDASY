/**
 * Name: LinkedTimerCombo.cpp
 * Desc: A linked list that holds comboboxes for timer values
**/

//	Windows
#include "stdafx.h"

#include "LinkedTimerCombo.h"

/**
 * Name: LinkedTimerCombo(newComboBox, newNode)
 * Para: newComboBox, the combo box that this encapsulates
 *		 newNode, the next node in the linked list
**/
LinkedTimerCombo::LinkedTimerCombo(CComboBox * newComboBox, LinkedTimerCombo * newNode)
{
	init(newComboBox, newNode);
}

/**
 * Name: LinkedTimerCombo(newComboBox)
 * Desc: Constructor that sets the next node to null
 * Para: newComboBox, the combo box that this encapsulates
**/
LinkedTimerCombo::LinkedTimerCombo(CComboBox * newComboBox)
{
	init(newComboBox, NULL);
}

/**
 * Name: LinkedTimerCombo.init()
 * Desc: Common "constructor" code that can be called from the
 *		 overloaded constructor
 * Para: newComboBox, the combo box that this encapsulates
 *		 newNode, the next node in the linked list
**/
void LinkedTimerCombo::init(CComboBox * newComboBox, LinkedTimerCombo * newNode)
{
	comboBox = newComboBox;
	nextNode = newNode;
}

/**
 * Name: LinkedTimerCombo.SetComboBox(newComboBox)
 * Desc: Sets this node's combo box to the provided object
**/
void LinkedTimerCombo::SetComboBox(CComboBox * newComboBox)
{
	comboBox = newComboBox;
}

/**
 * Name: LinedTimerCombo.GetComboBox()
 * Desc: Returns this node's combo box
**/
CComboBox * LinkedTimerCombo::GetComboBox()
{
	return comboBox;
}

/**
 * Name: SetNext(newNode)
 * Desc: Sets the next node in this list to the provided object
**/
void LinkedTimerCombo::SetNext(LinkedTimerCombo * newNode)
{
	nextNode = newNode;
}

/**
 * Name: LinkedTimerCombo.GetNext();
 * Desc: Returns the next node in this linked list
**/
LinkedTimerCombo * LinkedTimerCombo::GetNext()
{
	return nextNode;
}
