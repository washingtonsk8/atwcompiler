#include "CompilerComponent.h"
#include "ERRO_DEF.h"
#include "Globals.h"
#include "GlobalStructSite.h"
#include <queue>
#include "ThreadManager.h"

//---------------------------------------------------------------------------------------------------------------------
/*PRE DEF*/
DWORD WINAPI runLex(LPVOID lpParam);
DWORD WINAPI runSin(LPVOID lpParam);
/*PRE DEF*/
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
CompilerComponent::CompilerComponent(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
CompilerComponent::~CompilerComponent(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
void CompilerComponent::initialize(int _Argc, void** _Argv){
	if(_Argc < 3)
		_eManager->callHandlers(this->getGroupID(), INSUFFICIENT_ARGUMENTS, NULL);

	_Lex = (ATWLex*)_Argv[0];
	_Sin = (ATWSim*)_Argv[1];
	_Sem = (ATWSem*)_Argv[2];
	_fHelper = 0x0;//(FileHelper*)_Argv[2];//NAO UTILIZADO NESTA VERSAO

	/*INIT TOKEN BUFFER*/
	_tBuffer = new CHBuffer<ATW_BUFF_ELEMENT>(DEFAULT_MAX_BUFFER_CAPACITY);
}
//---------------------------------------------------------------------------------------------------------------------
void CompilerComponent::dispose(int _Argc, void** _Argv){
	if(_Lex != 0x0){
		delete _Lex;
		_Lex = 0x0;
	}

	if(_Sin != 0x0){
		delete _Sin;
		_Sin = 0x0;
	}

	if(_Sem != 0x0){
		delete _Sem;
		_Sem = 0x0;
	}

	if(_fHelper != 0x0){
		delete _fHelper;
		_fHelper = 0x0;
	}

	if(_tBuffer != 0x0){
		delete _tBuffer;
		_tBuffer = 0x0;
	}
	//TODO:check
}
//---------------------------------------------------------------------------------------------------------------------
void CompilerComponent::run(int _Flag){
	/*CUSTOM PARAMETER PASSING*/
	NORMAL_BUNDLE _nBundle;
	ThreadManager*_tManager;
	_nBundle.create(_tBuffer, _Lex, _Sin);
	/*CUSTOM PARAMETER PASSING*/

	switch(_Flag){
	case PARALLEL:
		_tManager = ThreadManager::getInstance();
		_tManager->createThread(runSin, &_nBundle, 0);
		_tManager->createThread(runLex, &_nBundle, 1);

		//::TODO::INIT TIME COUNTER
		_tManager->resumeThread(1);
		_tManager->resumeThread(0);
		_tManager->waitAllThreads();
		//::TODO::RELEASE TIME COUNTER

		break;
		/*RUNING COMPILLER*/
	default:
		//::TODO::INIT TIME COUNTER
		_Sin->Run(_Flag, &_nBundle);
		//::TODO::RELEASE TIME COUNTER
		break;
	}
}
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------PARALLEL EXECUTION -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
DWORD WINAPI runLex(LPVOID lpParam){
	NORMAL_BUNDLE* _nBundle = (NORMAL_BUNDLE*)lpParam;
	ATWLex* _Lex = _nBundle->_Lex;
	CHBuffer<ATW_BUFF_ELEMENT>* _tBuffer = _nBundle->_tBuffer;

	ATW_BUFF_ELEMENT _Cur;

	do{
		_Cur = _Lex->getToken();//LEX CALL
		_tBuffer->insertElement(_Cur);
	}while(_Cur._Token != ENDFILE);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
DWORD WINAPI runSin(LPVOID lpParam){
	/*PARAMETER LIST ARGUMENTS CROSS PASSING*/
	NORMAL_BUNDLE* _nBundle = (NORMAL_BUNDLE*)lpParam;
	ATWSim* _Sin = _nBundle->_Sin;

	/*RUNING SINTATIC ANALYSER WITH PARALLEL FLAG AND NEW ARGUMENTS LIST*/
	_Sin->Run(PARALLEL, _nBundle);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------PARALLEL EXECUTION -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------