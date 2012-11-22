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
#include "ATWLabelManager.h"
//---------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <time.h>
#include <sys/types.h>
#include <string>
#include <stdio.h>
#include <tchar.h>
using namespace std;
/*
 *	Integrantes: Alysson Ribeiro da Silva, Tiago Menegaz e Washington Luis
 */
//-----------------------------------------------------IMPORTANTE------------------------------------------------------

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
using namespace LabelController;
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
	_c.insertCodeToWriteAsm("ADI ");
	_c.insertCodeToWriteAsm("A, ");
	_c.insertCodeToWriteAsm("B\n");
	_c.insertCodeToWriteAsm("LOD A, #10(DS)\n");
	_c.insertCodeToWriteAsm("ADD ", 10, true);
	_c.insertCodeToWriteAsm("A, ", 11, true);
	_c.insertCodeToWriteAsm("#10\n", 12, true);
	_c.insertCodeToWriteAsm("LOD A, #10(DS)\n");
	_c.insertCodeToWriteAsm("LOD B, #12(DS)\n");
	_c.insertCodeToWriteAsm("ADD ", 22, true);
	_c.insertCodeToWriteAsm("A B\n", 23, true);
	_c.flush();
	_c.insertCodeToWriteAsm("SBI ");
	_c.insertCodeToWriteAsm("A, ");
	_c.insertCodeToWriteAsm("B\n");
	_c.insertCodeToWriteAsm("LOD A, #10(DS)\n");
	_c.insertCodeToWriteAsm("SUB ", 10, true);
	_c.insertCodeToWriteAsm("A, ", 11, true);
	_c.insertCodeToWriteAsm("#10\n", 12, true);
	_c.insertCodeToWriteAsm("LOD A, #10(DS)\n");
	_c.insertCodeToWriteAsm("LOD B, #12(DS)\n");
	_c.insertCodeToWriteAsm("ADD ", 22, true);
	_c.insertCodeToWriteAsm("A B\n", 23, true);
	_c.flush();
	_c.insertCodeToWriteAsm("ADI ");
	_c.insertCodeToWriteAsm("A, ");
	_c.insertCodeToWriteAsm("B\n");
	_c.insertCodeToWriteAsm("LOD A, #10(DS)\n");
	_c.insertCodeToWriteAsm("SUB ", 10, true);
	_c.insertCodeToWriteAsm("A, ", 11, true);
	_c.insertCodeToWriteAsm("#10\n", 12, true);
	_c.insertCodeToWriteAsm("LOD A, #10(DS)\n");
	_c.insertCodeToWriteAsm("LOD B, #12(DS)\n");
	_c.insertCodeToWriteAsm("ADD ", 22, true);
	_c.insertCodeToWriteAsm("A B", 23, true);
	_c.flush();
	_c.dispose(0, NULL);
}
/*CODE GENERATOR MODULE TESTS*/
//---------------------------------------------------------------------------------------------------------------------
/*LABEL CONTROLLER MODULE*/
void LabelControllerTests(){
	char* rot1 = ATWNovoRot();
    char* rot2 = ATWNovoRot();
	char* rot3 = ATWNovoRot();

	printf("%s\n%s\n%s\n", rot1, rot2, rot3);
}
/*LABEL CONTROLLER MODULE*/
//---------------------------------------------------------------------------------------------------------------------
/*COMPILER MAIN LOGIC*/
void _RunCompiler(int argc, char* argv[], char* envp[]){
	void* _SuperArg[2] = {"symbols.txt", argv[1]};
	//void* _SuperArg[2] = {"symbols.dat", "ex2.gl"};

	load();
	initialize(2, _SuperArg);
	_ATW->run(NULL);
	dispose();
}
/*COMPILER MAIN LOGIC*/
//---------------------------------------------------------------------------------------------------------------------
/*MAIN LOGIC*/
BOOL ExecCommand(char* szExe, char* szCmdLine, DWORD &dwProcessId)
{
	STARTUPINFO		si;
	PROCESS_INFORMATION	pi;
	BOOL			bResult;

	dwProcessId = 0;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);	// You need to set the size or it doesn't work

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	LPTSTR                ProcessName  = _T("C:\\Temp\\Downloads\\576326_GCX\\gcx.exe");
	LPTSTR                CommandLines = _T("551660_exemplo1.exe");

	bResult = CreateProcess(
	ProcessName, // pointer to name of executable module
	CommandLines,  // pointer to command line string
	NULL,	// pointer to process security attributes
	NULL,	// pointer to thread security attributes
	FALSE,	// handle inheritance flag 
	0,		// creation flags
	NULL,	// pointer to new environment block
	NULL,	// pointer to current directory name
	&si,	// pointer to STARTUPINFO
	&pi		// pointer to PROCESS_INFORMATION
	);

	if (bResult)
	{
		dwProcessId = pi.dwProcessId;

		// You get back two open handles if the process has been successfull created.
		// But, in this case, we don't want them so close them so we don't leak handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		DWORD dwError = GetLastError();
		// You can do something with dwError here if you want
	}

	return bResult;
}
//MAIN LOGIC
//---------------------------------------------------------------------------------------------------------------------
int main (int argc, char* argv[], char* envp[]){
	/*T_Manager tests*/
	//THREAD_CREATION_MANAGER_T();
	
	/*HCBuffer tests*/
	//CONTROLED_CUSTOM_BUFFER_T();
	
	/*WRITING TEST*/
	//FILE_WRITING_T();	

	/*CODE GENERATOR TEST*/
	//CODE_GENERATOR_T();

	/*LABEL GENERATOR MODULE TEST*/
	//LabelControllerTests();

	/*Compiler Running*/
	_RunCompiler(argc, argv, envp);

	system("pause");
	return 0;
}
