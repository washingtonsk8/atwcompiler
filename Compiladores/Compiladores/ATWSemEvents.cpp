#include "ATWSemEvents.h"
#include "Globals.h"
#include "ARS_PrinterHelper.h"
#include "ERRO_DEF.h"
//---------------------------------------------------------------------------------------------------------------------
using namespace ARS_PRINTER_HELPER;
//---------------------------------------------------------------------------------------------------------------------
ATWSemEvents::ATWSemEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
ATWSemEvents::~ATWSemEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------//---------------------------------------------------------------------------------------------------------------------
void ATWSemEvents::treatEvent(int _Args, void* _Argv[]){
	switch(_Args){
	case UNDECLARED_ID:
		printf("%i:Identificador nao declarado [%s].\n", (int)_Argv[0], (char*)_Argv[1]);
		system("pause");
		exit(UNDECLARED_ID);
		break;
	case ALREADY_DECLARED_ID:
		printf("%i:Identificador ja declarado [%s]\n", (int)_Argv[0], (char*)_Argv[1]);
		system("pause");
		exit(ALREADY_DECLARED_ID);
		break;
	case IMCOMPATIBLE_CLASS:
		printf("%i:Classe de identificador incompativel [%s].\n", (int)_Argv[0], (char*)_Argv[1]);
		system("pause");
		exit(IMCOMPATIBLE_CLASS);
		break;
	case INCOMPATILBE_TYPES:
		printf("%i:Tipos incompativeis.\n", (int)_Argv[0]);
		system("pause");
		exit(INCOMPATILBE_TYPES);
		break;
	case VAL_RESTRICTION:
		printf("%i: Restricao de valores quebrada.\n", (int)_Argv[0]);
		system("pause");
		exit(VAL_RESTRICTION);
		break;
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSemEvents::initialize(int _Argc, void** _Argv){
	this->setGroupID(COMPILER_GROUP);
}
