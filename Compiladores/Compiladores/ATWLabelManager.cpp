#include "ATWLabelManager.h"

namespace LabelController{
	//VARIABLES--------------------------------------------------
	EventsManager* _ev = EventsManager::getInstance();
	//Address*		   _CurrentPCAdress = (Address*) _memory->getPC();
	int            _Labels[MAX_LABELS] = {-1};
	int            _RotCount = 0;
	queue<int>     _AvailableFreeLabels;	
	//VARIABLES--------------------------------------------------

	//METHODS----------------------------------------------------
	char* ATWNovoRot(){
		char _Rot[255];
		sprintf_s(_Rot, "Rot%i", _RotCount);

		//insert into label vector
		unsigned int _CurrentKey = SDBMHash(_Rot) % MAX_LABELS;	
		//insert into label vector

		if(_Labels[_CurrentKey] != -1)
			_ev->callHandlers(PROGRAM_GROUP, FATAL_ERROR, NULL);

		if(_AvailableFreeLabels.size() == 0){
			_Labels[_CurrentKey] = _RotCount;
			_RotCount ++;
		}
		else{
			int _NewLabel = _AvailableFreeLabels.front();
			_AvailableFreeLabels.pop();
			_Labels[_CurrentKey] = _NewLabel;
		}

		static char _newRot[255];
		strcpy(_newRot, _Rot);

		return _newRot;
	}
	//---------------------------------------------------------------------------------------------------
	void ATWRemoveLabel(int _Label){
		char _Rot[255];
		sprintf_s(_Rot, "Rot%i", _Label);

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