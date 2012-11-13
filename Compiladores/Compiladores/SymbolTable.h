#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

/*FUNÇÃO DE HASH DO CAPETA
*SDBMHash(key)
*/

/*
*Extra includes
*/
#include <vector>
#include <string>
#include "FileManager.h"
#include "IInitializable.h"
#include "IDisposable.h"

/*
*MAIOR PRIMO DA NOSSA TABELA PARA CALCULAR O MOD E ACHAR UMA POSIÇÃO VÁLIDA
*/
#define DEFAULT_TABLE_SIZE 9973 
#define M_MIN(A,B) (A<B)?A:B

using namespace std;

/*
*Elemento da nossa tabela
*/
//---------------------------------------------------------------------------------------------------------------------
typedef enum tipo{
	TIPO_INTEIRO,
	TIPO_REAL,
	TIPO_LOGICO,
	TIPO_VAZIO
}Type;
//---------------------------------------------------------------------------------------------------------------------
typedef enum classe{
	CLASSE_VAR,
	CLASSE_CONST,
	CLASSE_PONTO,
	CLASSE_FACE,
	CLASSE_OBJETO,
	CLASSE_LUZ,
	CLASSE_COR,
	CLASSE_VAZIO
}Class;
//---------------------------------------------------------------------------------------------------------------------
typedef struct TableElement{
	int	_TokenID;
	char _Lexeme[16];
	unsigned int _End;
	Class _Class;
	Type _Type;

	void copyStr(const char* _Value){
		memset(_Lexeme, 0, 16);
		int cpySize = M_MIN(15,strlen(_Value));
		strncpy_s(_Lexeme, _Value, cpySize);
	}

	void build_element(
		int _TokenIDP,
		const char* _LexemeP,
		unsigned int _EndP = 0x0,
		Class classeP = CLASSE_VAZIO,
		Type tipoP = TIPO_VAZIO){
			_TokenID = _TokenIDP;
			copyStr(_LexemeP);
			_End = _EndP;
			_Class = classeP;
			_Type = tipoP;
	}
}TableElement;
//---------------------------------------------------------------------------------------------------------------------
/*
*Elemento da nossa tabela
*/

/*
*Tabela de símbolos
*/
class SymbolTable : public ProgramComponent, public IInitializable, public IDisposable{
public:
	//Construtor/destrutor padrão
	SymbolTable(void);
	~SymbolTable(void);

	void initialize(int _Argc, void** _Argv);
	void dispose(int _Argc, void** _Argv);

	void loadTable(FileManager* _fManager);
	void printTable();
	void insertElement(int _TokenID, string _Lexeme);
	void removeElement(string _Lexeme);
	TableElement* getElement(string _Lexeme);
	void resetTable();
	void allocTable();
	void unallocTable();
	void verifyIntegrity();

	//---------------------------------------EXTRA---
	void setType(string _Lexeme, Type _Type);
	void setClass(string _Lexeme, Class _Class);
	void setEnd(string _Lexeme, unsigned int _End);
	void setVal(string _Lexeme, int _valI = 0, float _valF = 0.0f);
	//-----------------------------------------------
	Type getType(string _Lexeme);
	Class getClass(string _Lexeme);
	unsigned int getEnd(string _Lexeme);
	int getValI(string _Lexeme);
	float getValF(string _Lexeme);
	//-----------------------------------------------
	bool typeCompare(string _Lexeme, Type _Type);
	bool classCompare(string _Lexeme, Class _Class);
	bool endCompare(string _Lexeme, unsigned int _End);
	//---------------------------------------EXTRA---

private:
	TableElement** _SymbolTable;
};
#endif
