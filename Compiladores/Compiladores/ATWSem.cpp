#include "ATWSem.h"

ATWSem::ATWSem(void){}
ATWSem::~ATWSem(void){}

void ATWSem::initialize(int _Argc, void** _Argv){
	if(_Argc < 1)
		_eManager->callHandlers(this->getGroupID(), INSUFFICIENT_ARGUMENTS, NULL);

	this->setGroupID(COMPILER_GROUP);
	_SymbolTable = (SymbolTable*)_Argv[0];
}