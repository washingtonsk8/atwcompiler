#include "EventsManager.h"
#include "EventHandler.h"
#include "ERRO_DEF.h"
#include "Globals.h"
//---------------------------------------------------------------------------------------------------------------------
EventsManager::EventsManager(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
EventsManager::~EventsManager(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
void EventsManager::initialize(int _Argc, void** _Argv){
	for(int i = 0; i < TOTAL_GROUP_IDS; i++){
		vector<EventHandler*> _nVec;
		_eHandlers.push_back(_nVec);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void EventsManager::dispose(int _Argc, void** _Argv){
	vector<vector<EventHandler*>>::iterator groupIt = _eHandlers.begin();

	for(;groupIt!=_eHandlers.end();groupIt++){
		vector<EventHandler*>* groupPtr = &(*groupIt);

		vector<EventHandler*>::iterator internalIT = groupPtr->begin();

		for(;internalIT!=groupPtr->end();internalIT++){
			if((*internalIT)!=NULL)
				delete (*internalIT);//TODO:verificar logica

			(*internalIT) = 0x0;
		}

		groupPtr->clear();
		groupPtr = 0x0;
	}

	_eHandlers.clear();
	//TODO:Implements
}
//---------------------------------------------------------------------------------------------------------------------
int EventsManager::callHandlers(unsigned int _GroupID, int _Args, void* _Argv[]){
	if(_GroupID > _eHandlers.size() || _GroupID < 0)
		return FATAL_ERROR;

	vector<EventHandler*> vecPtr = _eHandlers[_GroupID];

	for(unsigned int i = 0; i < vecPtr.size(); i++)
		if(vecPtr[i]->FUNCTIONAL)
			vecPtr[i]->treatEvent(_Args, _Argv);

	return SUCCEED;
}
//---------------------------------------------------------------------------------------------------------------------
int EventsManager::notifyExHandler(unsigned int _GroupID, int _HandlerID, int _Args, void* _Argv[]){
	if(_GroupID > _eHandlers.size() || _GroupID < 0)
		return FATAL_ERROR;

	if(_eHandlers[_GroupID][_HandlerID]->FUNCTIONAL)
		_eHandlers[_GroupID][_HandlerID]->treatEvent(_Args, _Argv);

	return SUCCEED;
}
//---------------------------------------------------------------------------------------------------------------------
int EventsManager::registerHandler(unsigned int _GroupID, EventHandler* _Handler){
	if(_GroupID > _eHandlers.size() || _GroupID < 0)
		return FATAL_ERROR;

	_eHandlers[_GroupID].push_back(_Handler);
	return SUCCEED;
}
//---------------------------------------------------------------------------------------------------------------------
int EventsManager::unregisterHandler(unsigned int _GroupID, EventHandler* _Handler){
	if(_GroupID > _eHandlers.size() || _GroupID < 0)
		return FATAL_ERROR;

	int searchID = _Handler->getGroupID();

	vector<EventHandler*>::iterator it = _eHandlers[_GroupID].begin();

	for(;it!=_eHandlers[_GroupID].end();it++){

		if((*it)->getGroupID() == searchID){
			_eHandlers[_GroupID].erase(it);
			return SUCCEED;	
		}
	}
	return NORMAL_ERROR;
}
//---------------------------------------------------------------------------------------------------------------------
int EventsManager::resetHandlers(){
	vector<vector<EventHandler*>>::iterator groupIt = _eHandlers.begin();

	for(;groupIt!=_eHandlers.end();groupIt++){
		vector<EventHandler*>* groupPtr = &(*groupIt);

		vector<EventHandler*>::iterator internalIT = groupPtr->begin();

		for(;internalIT!=groupPtr->end();internalIT++){
			if((*internalIT)!=NULL)
				delete (*internalIT);//TODO:verificar logica
				
			(*internalIT) = 0x0;
		}

		groupPtr->clear();
		groupPtr = 0x0;
	}

	_eHandlers.clear();

	return SUCCEED;
}




