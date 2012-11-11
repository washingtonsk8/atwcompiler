#ifndef ATW_H
#define ATW_H

#include "CompilerComponent.h"
#include "ProgramComponent.h"
#include "SymbolTable.h"
#include "Core.h"
#include "IInitializable.h"
#include "IDisposable.h"

class ATW : public ProgramComponent, public IInitializable, public IDisposable
{
public:
	ATW();
	~ATW(void);

	void initialize(int _Argc, void** _Argv);
	void dispose(int _Argc, void** _Argv);

	int run(void* _Args);
	int insertComponent(CompilerComponent* Comp);
	int removeComponent(int _ComponentID);

	//Operações para get e set
	SymbolTable* getSymbolTable();
	Core* getCore();
	CompilerComponent* getComponent(int _ComponentID);

private:
	vector<CompilerComponent*>* _Components;//lista de elementos do compilador a.lex, a.sim, a.sem etc
	SymbolTable* _SymbolTable;//tabela de simbolos
	Core* _SystemCore;//core do sistema geral
};
#endif

