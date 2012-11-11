#include "ATWLexEvents.h"
#include "Globals.h"
#include "ARS_PrinterHelper.h"
#include "ERRO_DEF.h"
//---------------------------------------------------------------------------------------------------------------------
using namespace ARS_PRINTER_HELPER;
//---------------------------------------------------------------------------------------------------------------------
ATWLexEvents::ATWLexEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
ATWLexEvents::~ATWLexEvents(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
void ATWLexEvents::treatEvent(int _Args, void* _Argv[]){
	switch(_Args){
	case INVALID_CHARACTER:
		printf("%i:Caractere invalido [%s].\n", (int)_Argv[0], (char*)_Argv[1]);
		system("pause");
		exit(INVALID_CHARACTER);
		break;
	case UNEXPECTED_FILE_END:
		printf("%i:Fim de arquivo nao esperado.\n", (int)_Argv);
		system("pause");
		exit(UNEXPECTED_FILE_END);
		break;
	case LEXEME_ERROR:
		printf("%i:Lexema nao identificado [%s].\n", (int)_Argv[0], (char*)_Argv[1]);
		system("pause");
		exit(LEXEME_ERROR);
		break;
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWLexEvents::initialize(int _Argc, void** _Argv){
	this->setGroupID(COMPILER_GROUP);
}