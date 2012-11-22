#include "ATWLabelManager.h"

namespace LabelController{
	//VARIABLES--------------------------------------------------
	EventsManager* _ev = EventsManager::getInstance();
	Address*	   _CurrentPCAdress = (Address*)ATWMemory::getInstance()->getPCAddess();
	Address        _Labels[MAX_LABELS] = {-1};
	int            _RotCount = 0;
	queue<int>     _AvailableFreeLabels;	
	//VARIABLES--------------------------------------------------
	Address getLabelAddress(char* _Label){
		unsigned int _CurrentKey = SDBMHash(_Label) % MAX_LABELS;

		if(_Labels[_CurrentKey] == -1)
			_ev->callHandlers(PROGRAM_GROUP, FATAL_ERROR, NULL);

		return _Labels[_CurrentKey];
	}
	//METHODS----------------------------------------------------
	char* ATWNovoRot(){
		char _Rot[255];
		sprintf_s(_Rot, "ROT%i", _RotCount);

		//if(_Labels[_CurrentKey] != -1)
			//_ev->callHandlers(PROGRAM_GROUP, FATAL_ERROR, NULL);
		if(_AvailableFreeLabels.size() == 0){
			_RotCount ++;
		}
		else{
			int _NewLabel = _AvailableFreeLabels.front();
			_AvailableFreeLabels.pop();
		}

		char* _newRot;
		_newRot = (char*)malloc(sizeof(char)*255);
		memset(_newRot, 0, sizeof(char)*255);
		strcpy(_newRot, _Rot);

		return _newRot;
	}
	//---------------------------------------------------------------------------------------------------
	void ATWRemoveLabel(int _Label){
		char _Rot[255];
		sprintf_s(_Rot, "ROT%i", _Label);

		//insert into label vector
		unsigned int _CurrentKey = SDBMHash(_Rot) % MAX_LABELS;	
		//insert into label vector

		if(_Labels[_CurrentKey] == -1)
			_ev->callHandlers(PROGRAM_GROUP, FATAL_ERROR, NULL);

		int _freedLabel = _Labels[_CurrentKey];

		_Labels[_CurrentKey] = -1;//NULL POINTER LABEL

		_AvailableFreeLabels.push(_freedLabel);
	}
	//METHODS----------------------------------------------------
}