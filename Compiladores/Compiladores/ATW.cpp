#include "ATW.h"
#include "Globals.h"
#include "CompilerComponent.h"
#include "ERRO_DEF.h"
//---------------------------------------------------------------------------------------------------------------------
ATW::ATW()
{
}
//---------------------------------------------------------------------------------------------------------------------
ATW::~ATW(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
void ATW::initialize(int _Argc, void** _Argv){
	//self init
	this->setGroupID(COMPILER_GROUP);

	if(_Argc < 3)
		_eManager->callHandlers(this->getGroupID(), INSUFFICIENT_ARGUMENTS, NULL);

	this->_SystemCore = reinterpret_cast<Core*>(_Argv[0]);

	{//ST initialization
		_SymbolTable = new SymbolTable();

		//Param configuration
		void* _Param[2];
		_Param[0] = (void*)_SystemCore->getFileManager();
		_Param[1] = _Argv[1];

		_SymbolTable->initialize(2, _Param);
	}

	{//COMPONENTS default initialization
		_Components = new vector<CompilerComponent*>();

		if(_Components->size()>0)
			_Components->clear();

		CompilerComponent* _nComp = new CompilerComponent();
		ATWLex* _Lex = new ATWLex();
		ATWSim* _Sin = new ATWSim();
		ATWSem* _Sem = new ATWSem();
		FileHelper* _fHelper = new FileHelper();

		void* _LexParam[2] = {_SystemCore->getFileManager(), _SymbolTable};
		_Lex->initialize(2, _LexParam);

		void* _SinParam[3] = {_Lex, _Sem, _fHelper};
		_Sin->initialize(3, _SinParam);

		void* _SemParam[3] = {_SymbolTable};
		_Sem->initialize(3, _SemParam);
		
		void* _CompParam[4] = {_Lex, _Sin, _Sem, _fHelper};
		_nComp->initialize(4, _CompParam);

		insertComponent(_nComp);
	}

	char* _FileName = (char*)_Argv[2];
	_SystemCore->getFileManager()->openFile(_FileName);
}
//---------------------------------------------------------------------------------------------------------------------
void ATW::dispose(int _Argc, void** _Argv){
	{//ST clearing
		_SymbolTable->dispose(_Argc, _Argv);
		delete _SymbolTable;
		_SymbolTable = 0x0;
	}
	{//Comp freeing
		vector<CompilerComponent*>::iterator it = _Components->begin();
		for(;it!=_Components->end();it++){
			(*it)->dispose(_Argc, _Argv);
			delete (*it);
			(*it) = 0x0;
		}
		delete _Components;
		_Components = 0x0;
	}
}
//---------------------------------------------------------------------------------------------------------------------
SymbolTable* ATW::getSymbolTable(){
	if(_SymbolTable == 0x0)
		_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return _SymbolTable;
}
//---------------------------------------------------------------------------------------------------------------------
int ATW::run(void* _Args){
	if(_Components->size()>0){
		_Components->at(0)->run(PARALLEL);	
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------------------
int ATW::insertComponent(CompilerComponent* Comp){
	Comp->setID(_Components->size()+1);
	this->_Components->push_back(Comp);
	return true;
}
//---------------------------------------------------------------------------------------------------------------------
int ATW::removeComponent(int _ComponentID){
	//TODO:Colocar impl
	return true;
}