#include "Core.h"
#include "Globals.h"
//---------------------------------------------------------------------------------------------------------------------
Core::Core(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
Core::~Core(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
FileManager* Core::getFileManager(){
	return _fManager;
}
//---------------------------------------------------------------------------------------------------------------------
void Core::initialize(int _Argc, void** _Argv){
	this->setGroupID(PROGRAM_GROUP);
	_fManager = new FileManager();
	_fManager->initialize(0, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void Core::dispose(int _Argc, void** _Argv){
	{// fManager clearing
		_fManager->dispose(_Argc, _Argv);
		delete _fManager;
		_fManager = 0x0;
	}
}
