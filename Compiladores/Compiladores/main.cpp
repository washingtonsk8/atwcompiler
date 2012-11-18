#include "FileManager.h"
#include "ATWLex.h"
#include "Globals.h"
#include "SymbolTable.h"
#include "ATW.h"
#include "Core.h"
#include "ATWSinEvents.h"
#include "ATWLexEvents.h"
#include "ATWSemEvents.h"
#include "ProgramEvents.h"
#include "ARS_ConsoleColorHandler.h"
#include "ARS_PrinterHelper.h"
#include "ERRO_DEF.h"
#include "CHBuffer.h"
#include "ThreadManager.h"
#include "CodeGeneratorModule.h"
//---------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <time.h>
#include <sys/types.h>
//-----------------------------------------------------IMPORTANTE------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------FOCA NO TRABALHO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//          .---.                        //----------------------------------------------------------------------------
//         /o   o\						 //-----------@@@--------@@@----------@@@@@@@@@--------------------------------
//      __(=  "  =)__					 //------------@@@------@@@-----------@@@@@@@@@@@@-----------------------------
//       //\'-=-'/\\					 //-------------@@@----@@@------------@@@------@@@-----------------------------
//          )   (_						 //--------------@@@--@@@-------------@@@------@@@-----------------------------
//         /      `#"=-._				 //---------------@@@@@@--------------@@@------@@@----------------------------- 
//        /       \     ``"=.			 //--------------@@@--@@@-------------@@@------@@@-----------------------------
//       /  /   \  \         `=..--.	 //-------------@@@----@@@------------@@@------@@@-----------------------------
//   ___/  /     \  \___      _,  , #`\	 //------------@@@------@@@-----------@@@@@@@@@@@@-----------------------------
//  `-----' `""""`'-----``"""`  \  \_/	 //-----------@@@--------@@@----------@@@@@@@@@--------------------------------
//                               `-`	 //----------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------FOCA NO TRABALHO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

/*NOTA - 1:TODOS O ERROS ESTÃO SENDO CHAMADOS VIA EventHandler 
E CADA ERRO TEM O SEU RESPECTIVO CODIGO NO ARQUIVO ERRO_DEF.h*/

/*NOTA - 2:TODA A ARQUITETURA JA ESTA IMPLEMENTADA E FUNCIONANDO INCLUINDO THREAD MANAGER*/
/*NOTA - 3:VERIFICAÇÃO DE TIPOS, CLASSES E COMPATIBILIDADE TOTALMENTE IMPLEMENTADA*/

//::TODO::GERENCIADOR DE MEMORIA GCX
//::TODO::COLOCAR GERAÇÃO DE CODIGO GCX
//::TODO::CRIAR MONTADOR NO MODULO DE GERAÇÃO DE CÓDIGO

//::DONE:: - GERENCIADOR DE ERROS DO A.SEM
//::DONE:: - A.SEM
//::DONE:: - PADRONIZAÇÃO DAS AÇÕES SEMANTICAS
//::DONE:: - MODIFICAÇÃO GERAL DO A.SIM
//::DONE:: - MODIFICAÇÃO GERAL DO A.LEX
//::DONE:: - MODIFICAÇÃO GERAL DA TS
//::DONE:: - PREPARAÇÃO PARA GERAÇÃO DE CÓDIGO
//::DONE:: - GERENCIADOR DO GERADOR DE CODIGO
//::DONE:: - ESTRUTURA AUXILIAR PARA MANIPULAÇÃO DE CODIGO ASSEMBLY CRIADA
//-----------------------------------------------------IMPORTANTE------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
using namespace std;
using namespace ARS_COLOR_HANDLER;
using namespace ARS_PRINTER_HELPER;
//---------------------------------------------------------------------------------------------------------------------
EventsManager*   _eManager;
ATWSinEvents*   _sExceptionH;
ATWLexEvents*   _lExceptionH;
ATWSemEvents*   _seExceptionH;
ProgramEvents*  _pExceptionH;
Core* _Core;
ATW*  _ATW;
//---------------------------------------------------------------------------------------------------------------------
void load(){
	{
		_eManager = EventsManager::getInstance();
		_sExceptionH = new ATWSinEvents();
		_lExceptionH = new ATWLexEvents();
		_seExceptionH = new ATWSemEvents();
		_pExceptionH = new ProgramEvents();
	}
	{
		_Core = new Core();
		_ATW = new ATW();
	}
}
//---------------------------------------------------------------------------------------------------------------------
void initialize(int _Argc, void** _Argv){
	if(_Argc < 2)
		_eManager->callHandlers(PROGRAM_GROUP, INSUFFICIENT_ARGUMENTS, NULL);
	{
		_eManager->initialize(0, NULL);
		_sExceptionH->initialize(0, NULL);
		_seExceptionH->initialize(0, NULL);
		_lExceptionH->initialize(0, NULL);
		_pExceptionH->initialize(0, NULL);
		_eManager->registerHandler(PROGRAM_GROUP, _pExceptionH);
		_eManager->registerHandler(COMPILER_GROUP, _seExceptionH);
		_eManager->registerHandler(COMPILER_GROUP, _lExceptionH);
		_eManager->registerHandler(COMPILER_GROUP, _sExceptionH);
	}
	{
		_Core->initialize(0, NULL);

		//init ATW
		//[0] -> Core
		//[1] -> tabela simbolos
		//[2] -> arquivo fonte
		void* _ATWParam[3] = {_Core, _Argv[0], _Argv[1]};
		_ATW->initialize(3, _ATWParam);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void dispose(){
	{//ATW clearing
		_ATW->dispose(0, NULL);
		delete _ATW;
		_ATW = 0x0;
	}
	{//CORE clearing
		_Core->dispose(0, NULL);
		delete _Core;
		_Core = 0x0;
	}
	{//EManager clearing
		_eManager->dispose(0, NULL);
		_eManager = 0x0;
	}
	//TODO:implements
}
//---------------------------------------------------------------------------------------------------------------------
/*THREAD CREATION CONTROLL TESTS*/
DWORD WINAPI threadT(LPVOID lpParam){
	for(int i = 0; i<1000000; i++)
		printf("teste\n");
	return true;
}
void THREAD_CREATION_MANAGER_T(){
	ThreadManager* tManager = ThreadManager::getInstance();
	tManager->createThread(threadT, NULL, 0);
	tManager->resumeThread(0);
	printf("Esperando thread de teste...\n");
	tManager->waitThread(0);
}
/*THREAD CREATION CONTROLL TESTS*/
//---------------------------------------------------------------------------------------------------------------------
/*CONTROLED CUSTOM BUFFER TESTS*/
void CONTROLED_CUSTOM_BUFFER_T(){
	CHBuffer<int>* buffer;
	buffer = new CHBuffer<int>(10);

	for(int i = 0; i < 10; i++)
		buffer->insertElement(i);

	buffer->print();

	int b[10];

	for(int i = 0; i < 10; i++){
		printf("\n");
		b[i] = buffer->removeElement();
		buffer->print();
	}
}
/*CONTROLED CUSTOM BUFFER TESTS*/
//---------------------------------------------------------------------------------------------------------------------
/*FILE WRITING TESTS*/
void FILE_WRITING_T(){
	FileManager* a;
	a = new FileManager();
	a->initialize(0, NULL);
	a->openFile("writeTest", GENERIC_WRITE, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	a->writeInFile("TESTE DE GRAVACAO PARA ASSEMBLY\nadi A B\nSTIF #1.8, 12(DS)");
	a->writeInFile("\n");
	a->writeInFile("ENDFILE\n");
	a->writeInFile("POST END\n");
	a->closeFile();
}
/*FILE WRITING TESTS*/
//---------------------------------------------------------------------------------------------------------------------
/*CODE GENERATOR MODULE TESTS*/
void CODE_GENERATOR_T(){
	CodeGeneratorModule _c;
	_c.initialize("codeGeneratorTest");
	_c.insertCodeToWriteMac("ADI ");
	_c.insertCodeToWriteMac("A, ");
	_c.insertCodeToWriteMac("B\n");
	_c.insertCodeToWriteMac("LOD A, #10(DS)\n");
	_c.insertCodeToWriteMac("ADD ", 10, true);
	_c.insertCodeToWriteMac("A, ", 11, true);
	_c.insertCodeToWriteMac("#10\n", 12, true);
	_c.insertCodeToWriteMac("LOD A, #10(DS)\n");
	_c.insertCodeToWriteMac("LOD B, #12(DS)\n");
	_c.insertCodeToWriteMac("ADD ", 22, true);
	_c.insertCodeToWriteMac("A B\n", 23, true);
	_c.flush();
	_c.insertCodeToWriteMac("SBI ");
	_c.insertCodeToWriteMac("A, ");
	_c.insertCodeToWriteMac("B\n");
	_c.insertCodeToWriteMac("LOD A, #10(DS)\n");
	_c.insertCodeToWriteMac("SUB ", 10, true);
	_c.insertCodeToWriteMac("A, ", 11, true);
	_c.insertCodeToWriteMac("#10\n", 12, true);
	_c.insertCodeToWriteMac("LOD A, #10(DS)\n");
	_c.insertCodeToWriteMac("LOD B, #12(DS)\n");
	_c.insertCodeToWriteMac("ADD ", 22, true);
	_c.insertCodeToWriteMac("A B\n", 23, true);
	_c.flush();
	_c.insertCodeToWriteMac("ADI ");
	_c.insertCodeToWriteMac("A, ");
	_c.insertCodeToWriteMac("B\n");
	_c.insertCodeToWriteMac("LOD A, #10(DS)\n");
	_c.insertCodeToWriteMac("SUB ", 10, true);
	_c.insertCodeToWriteMac("A, ", 11, true);
	_c.insertCodeToWriteMac("#10\n", 12, true);
	_c.insertCodeToWriteMac("LOD A, #10(DS)\n");
	_c.insertCodeToWriteMac("LOD B, #12(DS)\n");
	_c.insertCodeToWriteMac("ADD ", 22, true);
	_c.insertCodeToWriteMac("A B", 23, true);
	_c.flush();
	_c.dispose(0, NULL);
}
/*CODE GENERATOR MODULE TESTS*/
//---------------------------------------------------------------------------------------------------------------------
/*COMPILER MAIN LOGIC*/
void _RunCompiler(int argc, char* argv[], char* envp[]){
	//void* _SuperArg[2] = {"symbols.txt", argv[1]};
	void* _SuperArg[2] = {"symbols.txt", "testeDecl.gl"};

	load();
	initialize(2, _SuperArg);
	_ATW->run(NULL);
	dispose();
}
/*COMPILER MAIN LOGIC*/
//---------------------------------------------------------------------------------------------------------------------
/*MAIN LOGIC*/
int main (int argc, char* argv[], char* envp[]){
	/*T_Manager tests*/
	//THREAD_CREATION_MANAGER_T();
	
	/*HCBuffer tests*/
	//CONTROLED_CUSTOM_BUFFER_T();
	
	/*WRITING TEST*/
	//FILE_WRITING_T();	

	/*CODE GENERATOR TEST*/
	//CODE_GENERATOR_T();

	/*Compiler Running*/
	_RunCompiler(argc, argv, envp);

	system("pause");
	return 0;
}
