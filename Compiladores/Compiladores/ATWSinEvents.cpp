#include "ATWSinEvents.h"
#include "Globals.h"
#include "ERRO_DEF.h"
//---------------------------------------------------------------------------------------------------------------------
ATWSinEvents::ATWSinEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
ATWSinEvents::~ATWSinEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSinEvents::initialize(int _Argc, void** _Argv){
	this->setGroupID(COMPILER_GROUP);
}	
//---------------------------------------------------------------------------------------------------------------------
void ATWSinEvents::treatEvent(int _Args, void* _Argv[]){
	switch(_Args){
	case UNEXPECTED_TOKEN:
		printf("%i:Token nao esperado [%s].\n",(int)_Argv[0], (char*)_Argv[1]);
		system("pause");
		exit(UNEXPECTED_TOKEN);
		break;
	case NULL_ARGUMENT:
		//printf("ERRO FATAL: argumento nulo no codigo fonte na classe ATWsin.\n");
		//system("pause");
		//exit(NULL_ARGUMENT);
		break;
	case INSUFFICIENT_ARGUMENTS:
		printf("ERRO FATAL: argumentos insuficientes na passagem de parametro na classe ATWsin.\n");
		system("pause");
		exit(INSUFFICIENT_ARGUMENTS);
		break;
	case UNEXPECTED_FILE_END:
		printf("%i: Fim de arquivo nao esperado.", (int)_Argv[0]);
		system("pause");
		exit(UNEXPECTED_FILE_END);
	}
}