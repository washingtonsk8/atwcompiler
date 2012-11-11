#include "ProgramComponent.h"
#include "Globals.h"
#include "ERRO_DEF.h"
//---------------------------------------------------------------------------------------------------------------------
ProgramComponent::ProgramComponent(void)
{
	_eManager = EventsManager::getInstance();
	_ID = -1;
	_GroupID = GHOST_GROUP;
}
//---------------------------------------------------------------------------------------------------------------------
ProgramComponent::~ProgramComponent(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
int ProgramComponent::setGroupID(int _value)
{
	if(_value > 0 && _value < TOTAL_GROUP_IDS)
		_GroupID = _value;
	else
		return FATAL_ERROR;
	return SUCCEED;
}
//---------------------------------------------------------------------------------------------------------------------
int ProgramComponent::getGroupID()
{
	return _GroupID;
}
//---------------------------------------------------------------------------------------------------------------------
int ProgramComponent::setID(int _value)
{
	if(_value > 0)
		_ID = _value;
	else
		return FATAL_ERROR;
	return SUCCEED;
}
//---------------------------------------------------------------------------------------------------------------------
int ProgramComponent::getID()
{
	return _ID;
}
