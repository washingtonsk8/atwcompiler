#include <stdlib.h>
#include <memory.h>
#include <cstring>
#include <stdio.h>
#include "ATWLex.h"
#include "SymbolTable.h"
#include "GeneralHashFunctions.h"
#include "FileManager.h"
#include "Globals.h"
#include "EventHandler.h"
#include "ERRO_DEF.h"
//---------------------------------------------------------------------------------------------------------------------
SymbolTable::SymbolTable(void){
}
//---------------------------------------------------------------------------------------------------------------------
SymbolTable::~SymbolTable(void){
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::loadTable(FileManager* _fManager){
	if(_SymbolTable != 0x0)//zera pos antes de alocar pra n dar pau
		_SymbolTable = 0x0;

	if(_SymbolTable == 0x0)// inicializa caso n esteja inicializado
		allocTable();

	char _r;
	string _ReadString;
	string _CurrentLexeme;
	int _CurrentTokenID;
	bool _Flag = false;

	while (_r = _fManager->getNextChar()){
		if(_r > 32){
			_ReadString += _r;
		}else if(_r != 13){
			_CurrentTokenID = atoi(_ReadString.c_str()) - 1;
			_ReadString = "";
		}else{
			_CurrentLexeme = _ReadString;
			_fManager->getNextChar();
			_Flag = true;
		}
		if(_Flag){
			this->insertElement(_CurrentTokenID, _CurrentLexeme);
			_CurrentTokenID = 0;
			_CurrentLexeme = "";
			_ReadString = "";
			_Flag = false;
		}
	}
	if (!_ReadString.empty()){
		this->insertElement(_CurrentTokenID, _CurrentLexeme);
	}

	_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::printTable(){
	for (int i = 0; i < DEFAULT_TABLE_SIZE; i ++){
		if(_SymbolTable[i] != NULL)
			printf ("\nPosition = %i, ID = %i, Lexeme = %s\n", i,
			_SymbolTable[i]->_TokenID, _SymbolTable[i]->_Lexeme);		
	}
	_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::insertElement(int _TokenID, string _Lexeme){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;		

		if(_SymbolTable[_Hash] != NULL){
			TableElement* _ToRemove = _SymbolTable[_Hash];
			_SymbolTable[_Hash]  = 0x0;
			free(_ToRemove);
		}

		TableElement* _NewElement = new TableElement();
		//(TableElement*)malloc(sizeof(TableElement));
		//memset(_NewElement, 0, sizeof(TableElement));

		_NewElement->build_element(_TokenID, _Lexeme.c_str());

		_SymbolTable[_Hash] = _NewElement;	
		_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
	}
	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::removeElement(string _Lexeme){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;

		if(_SymbolTable[_Hash] != NULL){
			free (_SymbolTable[_Hash]);
			_SymbolTable[_Hash] = NULL;
		}

		_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
	}	

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
TableElement* SymbolTable::getElement(string _Lexeme){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		TableElement* _r = _SymbolTable[_Hash];

		if(_r != NULL){
			return _r;
		}
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return NULL;
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::resetTable(){
	unallocTable();
	allocTable();

	_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::allocTable(){
	_SymbolTable = new TableElement*[DEFAULT_TABLE_SIZE]();//(TableElement**)malloc(sizeof(TableElement*)*DEFAULT_TABLE_SIZE);
	memset(_SymbolTable, 0, sizeof(TableElement*)*DEFAULT_TABLE_SIZE);

	for(int i = 0; i < DEFAULT_TABLE_SIZE; i++)
		_SymbolTable[i] = NULL;

	_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::unallocTable(){
	if(_SymbolTable != NULL){	
		for(int i = 0; i < DEFAULT_TABLE_SIZE; i++){
			if(_SymbolTable[i] != NULL){
				delete(_SymbolTable[i]);	
				_SymbolTable[i] = 0x0;	
			}
		}
		delete(_SymbolTable);
		_SymbolTable = 0x0;
	}

	_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::verifyIntegrity(){
	for(int i = 0; i < DEFAULT_TABLE_SIZE; i++){
		if(_SymbolTable[i] != NULL)//TODO:raise exception
			printf("Integrity error at position: %i pointing to: %p !!!\n", i, _SymbolTable[i]);	

		//TODO:remove debug print	
		printf("Position[%i] value %p\n", i, _SymbolTable[i]);	
	}
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::initialize(int _Argc, void** _Argv){
	if(_Argc < 2)
		exit(-1); //TODO::tratar erro aqui

	//TODO:COLOCAR NOME DO ARQUIVO AQUI
	//aloca tabela
	allocTable();

	//self init
	this->setGroupID(COMPILER_GROUP);

	FileManager* _fManager = (FileManager*)_Argv[0];
	char* _fName = (char*)_Argv[1];

	_fManager->openFile(_fName);
	loadTable(_fManager);
	_fManager->closeFile();

	_eManager->callHandlers(this->getGroupID(), SUCCEED, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::dispose(int _Argc, void** _Argv){
	//TODO:implements
	unallocTable();
}
//---------------------------------------------------------EXTRA-------------------------------------------------------
void SymbolTable::setType(string _Lexeme, Type _Type){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL)
		{
			_SymbolTable[_Hash]->_Type = _Type;
		}
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::setClass(string _Lexeme, Class _Class){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL)
		{
			_SymbolTable[_Hash]->_Class = _Class;
		}
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void SymbolTable::setEnd(string _Lexeme, unsigned int _End){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL){
			_SymbolTable[_Hash]->_End = _End;
		}
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
Type SymbolTable::getType(string _Lexeme){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL)
		{
			return _SymbolTable[_Hash]->_Type;
		}
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return TIPO_VAZIO;
}
//---------------------------------------------------------------------------------------------------------------------
Class SymbolTable::getClass(string _Lexeme){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL)
		{
			return _SymbolTable[_Hash]->_Class;
		}
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return CLASSE_VAZIO;
}
//---------------------------------------------------------------------------------------------------------------------
unsigned int SymbolTable::getEnd(string _Lexeme){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL){
			return _SymbolTable[_Hash]->_End;
		}
	}//end if

	//_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return -1;
}
//---------------------------------------------------------------------------------------------------------------------
bool SymbolTable::typeCompare(string _Lexeme, Type _Type){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL){
			return _SymbolTable[_Hash]->_Type == _Type;
		}else
			return false;
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return false;
}
//---------------------------------------------------------------------------------------------------------------------
bool SymbolTable::classCompare(string _Lexeme, Class _Class){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL){
			return _SymbolTable[_Hash]->_Class == _Class;
		}else
			return false;
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return 0x0;
}
//---------------------------------------------------------------------------------------------------------------------
bool SymbolTable::endCompare(string _Lexeme, unsigned int _End){
	if(_SymbolTable != NULL){
		unsigned int _Hash = SDBMHash(_Lexeme) % DEFAULT_TABLE_SIZE;
		if(_SymbolTable[_Hash] != NULL){
			return _SymbolTable[_Hash]->_End == _End;
		}else
			return false;
	}//end if

	_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);
	return 0x0;
}
//---------------------------------------------------------EXTRA-------------------------------------------------------