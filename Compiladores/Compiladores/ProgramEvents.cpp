#include "ProgramEvents.h"
#include "Globals.h"
#include "ERRO_DEF.h"
#include <Windows.h>
//---------------------------------------------------------------------------------------------------------------------
ProgramEvents::ProgramEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
ProgramEvents::~ProgramEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
void ProgramEvents::initialize(int _Argc, void** _Argv){
	this->setGroupID(PROGRAM_GROUP);
}	
//---------------------------------------------------------------------------------------------------------------------
void ProgramEvents::treatEvent(int _Args, void* _Argv[]){
	va_list _variableList;
	va_start (_variableList, _Args); 

	char* _erroString = va_arg(_variableList, char*);

	switch(_Args){//TODO:tratar erros
	case SUCCEED:
		break;
	case OPERATION_DONE:
		break;
	case PRINTED:
		break;
	case FILE_NOT_FOUND_EXCEPTION:
		printf("Arquivo fonte nao encontrado.\n");
		system("pause");
		exit(FILE_NOT_FOUND_EXCEPTION);
	case NORMAL_ERROR:
		break;
	case FATAL_ERROR:
		printf("ERRO FATAL!!!\n");
		system("pause");
		exit(FATAL_ERROR);
	}

	va_end(_variableList);
}
