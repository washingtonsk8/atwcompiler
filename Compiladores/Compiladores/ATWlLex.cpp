#include "ATWLex.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include "Globals.h"
#include "ERRO_DEF.h"
#include "ARS_PrinterHelper.h"
#include <iostream> 
#include <iomanip>  
//---------------------------------------------------------------------------------------------------------------------
using namespace std;
using namespace ARS_PRINTER_HELPER;
//---------------------------------------------------------------------------------------------------------------------
ATWLex::ATWLex(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
ATWLex::~ATWLex(void)
{
}
void run(int _Flag){
}
//---------------------------------------------------------------------------------------------------------------------
char  _ReadChar;
//---------------------------------------------------------------------------------------------------------------------
void ATWLex::initialize(int _Argc, void** _Argv){
	if(_Argc < 2)
		_eManager->callHandlers(this->getGroupID(), FATAL_ERROR, NULL);

	setGroupID(COMPILER_GROUP);
	this->_fManager = (FileManager*)_Argv[0];
	this->_SymbolTable = (SymbolTable*)_Argv[1];
	_LineCount = 1;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL ATWLex::isDigit(char c){
	return (c >= 48 && c <= 57);
}
//---------------------------------------------------------------------------------------------------------------------
BOOL ATWLex::isLetter(char c){
	return ((c >= 65 && c <= 90)||(c >= 97 && c <= 122));
}
//---------------------------------------------------------------------------------------------------------------------
BOOL ATWLex::isAlpha(char c){
	return (isDigit(c)||isLetter(c));
}
//---------------------------------------------------------------------------------------------------------------------
BOOL ATWLex::isValid(char c){
	//Caracteres ordenados por probabilidade de aparecimento (Espaço, l, quebra, L...)
	return (c == 32 || c > 95 && c < 124 || c == 13 || c > 64 && c < 92 || c > 39 && c < 64 ||
		c == 125 || c == 33 || c == 34 || c == 9 || c == 36 || c == 93);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWLex::buildConstValue(int* _desI, float* _desF, const char* _value){
	int inteiro;
	float real;

	if(EOF == sscanf_s(_value, "%4d", &inteiro))//RESTRICTION
		_eManager->callHandlers(PROGRAM_GROUP, FATAL_ERROR, NULL);

	if(EOF == sscanf_s(_value, "%f", &real))//RESTRICTION
		_eManager->callHandlers(PROGRAM_GROUP, FATAL_ERROR, NULL);

	(*_desF) = real;
	(*_desI) = inteiro;
}
//---------------------------------------------------------------------------------------------------------------------
ATW_BUFF_ELEMENT ATWLex::getToken(){
	Token _CurrentToken = DEFAULT_TOKEN;//Atribui token padrão ao token atual
	TableElement* _Element;//Referência para elemento da tabela
	StateType _State = START;//Atribui estado inicial ao estado atual
	_Lexeme = "";//Reseta lexema

	ATW_BUFF_ELEMENT _toReturn;//NOVA OP PARA RETURN
	_toReturn.Build(_LineCount, _CurrentToken, _Lexeme.c_str());//NOVA OP PARA RETURN

	//CONST CONTROLL
	_constType = TIPO_VAZIO;

	while (_State != DONE)
	{
		_ReadChar = toupper(_fManager->getNextChar());
		if (!_ReadChar){//Lê um char carregado do Buffer
			_Lexeme += _ReadChar;//Concatenação
			if (_State == INCOMMENT || _State == INBREAK || _State == INDOT0){//TODO: Verificar quais estados devem ser verificados para o lançamento do erro!
				_eManager->callHandlers(this->getGroupID(), UNEXPECTED_FILE_END, (void**)_LineCount);
			}//end if
			_toReturn.Build(_LineCount, ENDFILE, _Lexeme.c_str());
			return _toReturn;//Retorna que ocorreu fim de arquivo
		}//end if
		
		if (!isValid(_ReadChar)){
			_Lexeme += _ReadChar;//Concatenação
			void* _Param[2] = {(void*)_LineCount, (void*)&_ReadChar};
			_eManager->callHandlers(this->getGroupID(), INVALID_CHARACTER, _Param);
		}//end if

		switch (_State) {
		case START:
			if(isLetter(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INID;
			}else if(isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INNUM1;
			}else{
				switch (_ReadChar){
				case 32://ESPACO EM BRANCO 
					break;//Volta para o início do loop
				case 13://Primeiro Char da Quebra de Linha
					_fManager->getNextChar();//Pula o caractere 10				
					_LineCount ++;
					_CurrentToken = EXP_END;
					_State = DONE;
					break;
				case 9://TAB
					break;//Volta para o início do loop
				case '.':
					_Lexeme += _ReadChar;//Concatenação
					_State = INDOT0;
					break;
				case '{':
					_State = INCOMMENT;
					break;
				case '=':
					_Lexeme += _ReadChar;//Concatenação
					_State = INATRIB;
					break;
				case '>':
					_Lexeme += _ReadChar;//Concatenação
					_State = INGT;
					break;
				case '<':
					_Lexeme += _ReadChar;//Concatenação
					_State = INLT;
					break;
				case '$':
					_State = INBREAK;
					break;
				case '(':
					_Lexeme += _ReadChar;//Concatenação
					_CurrentToken = LPAREN;
					_State = DONE;
					break;
				case ')':
					_Lexeme += _ReadChar;//Concatenação
					_CurrentToken = RPAREN;
					_State = DONE;
					break;
				case ',':
					_Lexeme += _ReadChar;//Concatenação
					_CurrentToken = COMMA;
					_State = DONE;
					break;
				case '+':
					_Lexeme += _ReadChar;//Concatenação
					_CurrentToken = PLUS;
					_State = DONE;
					break;
				case '-':
					_Lexeme += _ReadChar;//Concatenação
					_CurrentToken = MINUS;
					_State = DONE;
					break;
				case '*':
					_Lexeme += _ReadChar;//Concatenação
					_CurrentToken = TIMES;
					_State = DONE;
					break;
				case '/':
					_Lexeme += _ReadChar;//Concatenação
					_CurrentToken = OVER;
					_State = DONE;
					break;
				default:
					void* _Param[2] = {(void*)_LineCount, (void*)_Lexeme.c_str()};
					_eManager->callHandlers(this->getGroupID(), LEXEME_ERROR, _Param);
				}//end switch
			}//end else
			break;
		case INID:
			if(!isAlpha(_ReadChar)){
				_fManager->ungetNextChar();//Devolve o char lido
				_Element = _SymbolTable->getElement(_Lexeme);

				if (_Element){//Se o elemento existir na tabela
					_CurrentToken = (Token)_Element->_TokenID;//Pode ser ID ou PALAVRA RESERVADA
				}//end if
				else{//Se não existir, insere e retorna
					_SymbolTable->insertElement(ID, _Lexeme);
					_CurrentToken = ID;
				}//end else
				_State = DONE;
			}//end if
			else{
				_Lexeme += _ReadChar;//Concatenação
			}//end else
			break;
		case INCOMMENT:
			if (_ReadChar == '}'){
				_State = START;
			}//end if
			else if (_ReadChar == 13){
				_fManager->getNextChar();//Pula o caractere 10
				_LineCount ++;
			}
			break;
		case INBREAK:
			if (_ReadChar == 13){//QUEBRA DE LINHA
				_LineCount ++;
				_fManager->getNextChar();//Pega o Char 10
				_State = START;
			}//end if
			else if (_ReadChar == '{'){
				_State = INBREAKCOMMENT;
			}//end else if
			else if(_ReadChar != 32 && _ReadChar != 9){//ERRO!
				_Lexeme += _ReadChar;//Concatenação
				void* _Param[2] = {(void*)_LineCount, (void*)_Lexeme.c_str()};
				_eManager->callHandlers(this->getGroupID(), LEXEME_ERROR, _Param);
			}//end else if
			break;
		case INBREAKCOMMENT:
			if (_ReadChar == '}'){
				_State = INBREAK;
			}//end if
			else if (_ReadChar == 13){
				_fManager->getNextChar();//Pula o caractere 10
				_LineCount ++;
			}
			break;
		case INLT:
			if (_ReadChar == '>'){
				_CurrentToken = DIFF;
				_State = DONE;
			}else if (_ReadChar == '='){
				_CurrentToken = LE;
				_State = DONE;
			}else{
				_fManager->ungetNextChar();//Devolve char lido
				_CurrentToken = LT;
				_State = DONE;
			}//end else
			break;
		case INATRIB:
			if (_ReadChar == '='){//Duplo igual
				_CurrentToken = DEQ;
				_State = DONE;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_CurrentToken = EQ;
				_State = DONE;
			}//end else
			break;
		case INGT:
			if (_ReadChar == '='){
				_CurrentToken = GE;
				_State = DONE;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_CurrentToken = GT;
				_State = DONE;
			}//end else
			break;
		case INNUM1:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INNUM2;
				_CurrentToken = CONSTANT;
				_constType = TIPO_INTEIRO;
			}else if (_ReadChar == '.'){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT0NUM;			
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_CurrentToken = CONSTANT;
				_constType = TIPO_INTEIRO;//CONST CONTROLL
				_State = DONE;
			}//end else				
			break;
		case INNUM2:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INNUM3;
			}else if (_ReadChar == '.'){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT0NUM;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else		
			break;
		case INNUM3:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INNUM4;
			}else if (_ReadChar == '.'){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT0NUM;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else		
			break;          
		case INNUM4:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				void* _Param[2] = {(void*)_LineCount, (void*)_Lexeme.c_str()};
				_eManager->callHandlers(this->getGroupID(), LEXEME_ERROR, _Param);
			}else if (_ReadChar == '.'){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT0NUM;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else		
			break;            		
		case INDOT0:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT1;
				_CurrentToken = CONSTANT;
				_constType = TIPO_REAL;//CONST CONTROLL
			}else{
				void* _Param[2] = {(void*)_LineCount, (void*)_Lexeme.c_str()};
				_eManager->callHandlers(this->getGroupID(), LEXEME_ERROR, _Param);
			}//end else				
			break;
		case INDOT0NUM:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT1;
				_constType = TIPO_REAL;//CONST CONTROLL
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else
			break;
		case INDOT1:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT2;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else			
			break;                
		case INDOT2:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT3;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else		
			break;                
		case INDOT3:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				_State = INDOT4;
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else			
			break;
		case INDOT4:
			if (isDigit(_ReadChar)){
				_Lexeme += _ReadChar;//Concatenação
				void* _Param[2] = {(void*)_LineCount, (void*)_Lexeme.c_str()};
				_eManager->callHandlers(this->getGroupID(), LEXEME_ERROR, _Param);
			}else{
				_fManager->ungetNextChar();//Devolve o char lido
				_State = DONE;
			}//end else
			break;
		default:
			_State = DONE;
			break;
		}//end switch
	}//end while

	_toReturn.Build(_LineCount, _CurrentToken, _Lexeme.c_str());//NOVO MEIO DE RETORNAR PARA DIMINUIR ACOPLAMENTO
	if(_CurrentToken == CONSTANT){
		_toReturn._Tipo = _constType;/*
		buildConstValue(&_toReturn._valI, &_toReturn._valF, _toReturn._Lex);*/
	}
	return _toReturn;
}
