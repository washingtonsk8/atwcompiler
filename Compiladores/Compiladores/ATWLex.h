#ifndef	ATWLEX_H
#define ATWLEX_H

#include "ProgramComponent.h"
#include "FileManager.h"
#include "FileHelper.h"
#include "SymbolTable.h"
#include "IInitializable.h"
#include "GlobalStructSite.h"
#include "globaEnumSite.h"
#include <queue>

class ATWLex;
class ATWSin;

class ATWLex : public ProgramComponent, public IInitializable
{
public:
	ATWLex(void);
	~ATWLex(void);

	//Retorna o Token obtido no processo de varredura do arquivo
	ATW_BUFF_ELEMENT getToken();
	void initialize(int _Argc, void** _Argv);
	int getCurrentLine(){return _LineCount;}
	string getCurrentLexeme(){return _Lexeme;}
	void run(int _Flag);

private:
	FileManager* _fManager;
	SymbolTable* _SymbolTable;
	BOOL isDigit(char c);
	BOOL isLetter(char c);
	BOOL isAlpha(char c);
	BOOL isValid(char c);
	string _Lexeme;
	int _LineCount;

	/*CONSTANT CONTROLL FLAGS N' PARSER*/
	Type _constType;
	void buildConstValue(int* _desI, float* _desF, const char* _value);
	/*CONSTANT CONTROLL FLAGS N' PARSER*/
};

#endif