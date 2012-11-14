#ifndef ATW_SEM_H
#define ATW_SEM_H
//---------------------------------------------------------------------------------------------
#include "ProgramComponent.h"
#include "IInitializable.h"
#include "SymbolTable.h"
#include "ERRO_DEF.h"
#include "GlobalStructSite.h"
#include "Globals.h"
//---------------------------------------------------------------------------------------------
typedef enum ValRestrictionOP{
	VR_GREATER,
	VR_GREATEREQ,
	VR_LESS,
	VR_LESSEQ,
	VR_EQ
}VROperation;
//---------------------------------------------------------------------------------------------
class ATWSem : public ProgramComponent, public IInitializable{
public:
	ATWSem(void);
	~ATWSem(void);
	//-----------------------------------------------------------------------------------------
	void initialize(int _Argc, void** _Argv);
	//-----------------------------------------------------------------------------------------
	//(1) unicidade - não declarado
	void unicidadeNotDeclared(ATW_BUFF_ELEMENT _Token){
		TableElement* _tToken = _SymbolTable->getElement(_Token._Lex);
		if(_tToken->_Class == CLASSE_VAZIO){
			void* _Param[2] = {(void*)_Token._LINE,(void*)_Token._Lex};
			_eManager->callHandlers(this->getGroupID(), UNDECLARED_ID , _Param);
		}
	}
	//-----------------------------------------------------------------------------------------
	//(2) unicidade - ja declarado
	void unicidadeAlreadyDeclared(ATW_BUFF_ELEMENT _Token, Class _Class){
		TableElement* _tToken = _SymbolTable->getElement(_Token._Lex);
		if(_tToken->_Class != CLASSE_VAZIO){
			void* _Param[2] = {(void*)_Token._LINE,(void*)_Token._Lex};
			_eManager->callHandlers(this->getGroupID(), ALREADY_DECLARED_ID , _Param);
		}else{
			_tToken->_Class = _Class;
		}
	}
	//-----------------------------------------------------------------------------------------
	//COMPATIBILIDADE DE CLASSES
	void classVerify(ATW_BUFF_ELEMENT _Token, Class _Class){
		TableElement* _tToken = _SymbolTable->getElement(_Token._Lex);
		if(_tToken->_Class != _Class){
			void* _Param[2] = {(void*)_Token._LINE,(void*)_Token._Lex};
			_eManager->callHandlers(this->getGroupID(), IMCOMPATIBLE_CLASS, _Param);
		}
	}
	//-----------------------------------------------------------------------------------------
	//COMPATIBILIDADE DE CLASSES
	void nClassVerify(ATW_BUFF_ELEMENT _Token, int _cS, Class _Class[]){
		if(_Class == NULL)
			_eManager->callHandlers(PROGRAM_GROUP, FATAL_ERROR, NULL);

		TableElement* _tToken = _SymbolTable->getElement(_Token._Lex);

		for(int i = 0; i < _cS; i++){
			if(_tToken->_Class == _Class[i])
				return;
		}

		void* _Param[2] = {(void*)_Token._LINE,(void*)_Token._Lex};
		_eManager->callHandlers(this->getGroupID(), IMCOMPATIBLE_CLASS, _Param);
	}
	//-----------------------------------------------------------------------------------------
	//TIPOS
	void setType(ATW_BUFF_ELEMENT _Token, Type _Tipo){
		TableElement* _tToken = _SymbolTable->getElement(_Token._Lex);
		_tToken->_Type = _Tipo;
	}
	//-----------------------------------------------------------------------------------------
	//TIPOS
	Type getType(ATW_BUFF_ELEMENT _Token){
		TableElement* _tToken = _SymbolTable->getElement(_Token._Lex);
		return _tToken->_Type;
	}
	//-----------------------------------------------------------------------------------------
	//TIPOS
	void sSetType(ATW_BUFF_ELEMENT _TokenA, ATW_BUFF_ELEMENT _TokenB){
		TableElement* _tTokenA = _SymbolTable->getElement(_TokenA._Lex);

		if(_TokenB._Token != CONSTANT){
			TableElement* _tTokenB = _SymbolTable->getElement(_TokenB._Lex);
			_tTokenA->_Type = _tTokenB->_Type;
		}else{
			_tTokenA->_Type = _TokenB._Tipo;
		}
	}
	//-----------------------------------------------------------------------------------------
	//COMPATIBILIDADE DE TIPOS
	void nTypeVerify(ATW_BUFF_ELEMENT _Token, Type _A, int _cS, Type _tArray[]){
		for(int i = 0; i < _cS; i++){
			if(_A == _tArray[i])
				return;
		}

		void* _Param[1] = {(void*)_Token._LINE};
		_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
	}
	//-----------------------------------------------------------------------------------------
	//COMPATIBILIDADE DE TIPOS
	void TypeVerify(ATW_BUFF_ELEMENT _Token, Type _A, Type _B){
		if(_A == _B)
			return;

		void* _Param[1] = {(void*)_Token._LINE};
		_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
	}
	//-----------------------------------------------------------------------------------------
	//COMPATIBILIDADE DE TIPOS
	void tTypeVerify(ATW_BUFF_ELEMENT _Token, Type _A){
		TableElement* _tTokenA = _SymbolTable->getElement(_Token._Lex);
		if(_A == _tTokenA->_Type)
			return;

		void* _Param[1] = {(void*)_Token._LINE};
		_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
	}
	//-----------------------------------------------------------------------------------------
	//COMPATIBILIDADE DE TIPOS
	void FullNTypeVerify(ATW_BUFF_ELEMENT _Token, Type _A, int _cS, Type _tArray[]){
		for(int i = 0; i < _cS; i++){
			if(_A != _tArray[i]){
				void* _Param[1] = {(void*)_Token._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}
	}
	//-----------------------------------------------------------------------------------------
	bool ValRestriction(ATW_BUFF_ELEMENT _Token, double _ValRestriction, VROperation _OP){
		string _Compare = _Token._Lex;
		double _valCompare = atof(_Compare.c_str());
		bool _res;

		switch(_OP){
		case VR_EQ:
			_res = _valCompare == _ValRestriction;
			break;
		case VR_GREATER:
			_res = _valCompare > _ValRestriction;
			break;
		case VR_GREATEREQ:
			_res = _valCompare >= _ValRestriction;
			break;
		case VR_LESS:
			_res = _valCompare < _ValRestriction;
			break;
		case VR_LESSEQ:
			_res = _valCompare <= _ValRestriction;
			break;
		}

		if(!_res)
			return true;

		void* _Param[1] = {(void*)_Token._LINE};
		_eManager->callHandlers(this->getGroupID(), VAL_RESTRICTION, _Param);

		return false;
	}
	//-----------------------------------------------------------------------------------------
	const char* updateIDAddress(const char* _lex, unsigned int _Address){
		_SymbolTable->setEnd(_lex, _Address);
		char _buffBAA[255];
		_itoa_s(_Address, _buffBAA, 10);
		string _return = _buffBAA;
		return _return.c_str();
	}
	//-----------------------------------------------------------------------------------------
private:
	SymbolTable* _SymbolTable;
};

#endif