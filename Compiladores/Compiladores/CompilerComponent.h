#ifndef COMPILERCOMPONENT_H
#define COMPILERCOMPONENT_H

#include "ATWLex.h"
#include "ATWSin.h"
#include "FileHelper.h"
#include "ProgramComponent.h"
#include "Globals.h"
#include "IInitializable.h"
#include "IDisposable.h"
#include "ILoadable.h"
#include "CHBuffer.h"
#include "ATWSem.h"
#include <queue>
#include <Windows.h>
//----------------------------------------------------------------------------------------------------------------------------------
#define DEFAULT_MAX_BUFFER_CAPACITY 10
//----------------------------------------------------------------------------------------------------------------------------------
class CompilerComponent : public ProgramComponent, public IInitializable, public IDisposable
{
public:
	CompilerComponent(void);
	~CompilerComponent(void);

	int create(ATWLex* _lInstance, ATWSin* _sInstance, ATWSem* _seInstance, FileHelper* _fInstance){
		_Lex     = _lInstance;
		_Sin     = _sInstance;
		_Sem     = _seInstance;
		_fHelper = _fInstance;
		return true;
	}

	void initialize(int _Argc, void** _Argv);
	void dispose(int _Argc, void** _Argv);
	void run(int _Flag);

	ATWLex* getLex(){return _Lex;}
	ATWSin* getSin(){return _Sin;}
	ATWSem* getSem(){return _Sem;}
	FileHelper* getfHelper(){return _fHelper;}

private:
	ATWLex* _Lex;
	ATWSin* _Sin;
	ATWSem* _Sem;
	FileHelper* _fHelper;

	/*PARALLEL BUFFERING*/
	CHBuffer<ATW_BUFF_ELEMENT>* _tBuffer;//TOKEN BUFFER 
};
#endif

