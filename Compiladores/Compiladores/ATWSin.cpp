#include "ATW_MemoryManager.h"
#include "ATWSin.h"
#include "Globals.h"
#include "FileHelper.h"
#include "ERRO_DEF.h"
#include "ARS_PrinterHelper.h"
#include <Windows.h>
//---------------------------------------------------------------------------------------------------------------------
using namespace ARS_PRINTER_HELPER;
using namespace MemoryManager;
//---------------------------------------------------------------------------------------------------------------------
ATWSin::ATWSin(void){
}
//---------------------------------------------------------------------------------------------------------------------
ATWSin::~ATWSin(void){
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::CT(Token _Token){
	if(_LexAnalyzer==NULL)
		_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);

	Token _cTok = _CurrentToken._Token;
	string _cLex = _CurrentToken._Lex;
	int _cLexLine = _CurrentToken._LINE;

	if(_cTok != _Token){

		{//ERROs
			void* _Param[2] = {(void*)_cLexLine,(void*)_cLex.c_str()};
			if (_cTok == ENDFILE)
				_eManager->callHandlers(this->getGroupID(), UNEXPECTED_FILE_END, (void**)_cLexLine);//Fim de arquivo não esperado
			_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);//Erro de token não esperado
		}
	}

	//------------------------------------------------------------
	/*REF ANTERIOR PARA POUPAR VARIÁVEIS*/
	_PreviousToken = _CurrentToken;
	/*REF ANTERIOR PARA POUPAR VARIÁVEIS*/
	//------------------------------------------------------------
	
	if(_Parallel ==  PARALLEL)
		_CurrentToken = _tBuffer->removeElement();
	else
		_CurrentToken = _LexAnalyzer->getToken();
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::initialize(int _Argc, void** _Argv){
	if(_Argc < 3)
		_eManager->callHandlers(this->getGroupID(), INSUFFICIENT_ARGUMENTS, NULL);

	this->setGroupID(COMPILER_GROUP);
	_LexAnalyzer = (ATWLex*)_Argv[0];
	_Sem = (ATWSem*)_Argv[1];
	_cg = new CodeGeneratorModule();
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::Run(int _Flag, NORMAL_BUNDLE* _nBundleP){
	_Parallel = _Flag;

	if(_LexAnalyzer==NULL)
		_eManager->callHandlers(this->getGroupID(), NULL_ARGUMENT, NULL);

	if(_Parallel == PARALLEL){
		NORMAL_BUNDLE* _nBundle = _nBundleP;
		_tBuffer = _nBundle->_tBuffer;

		_CurrentToken = _tBuffer->removeElement();
	}
	else{	
		_CurrentToken = _LexAnalyzer->getToken();
	}

	_PreviousToken = _CurrentToken;
	
	Start();

	if(_CurrentToken._Token != ENDFILE){
		void* _Param[2] = {(void*)_CurrentToken._LINE,(void*)_CurrentToken._Lex};
		_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::Start(){
	while(_CurrentToken._Token == DPONTO || _CurrentToken._Token ==  DCOR || _CurrentToken._Token ==  DLUZ ||
		_CurrentToken._Token ==  DFACE || _CurrentToken._Token ==  DOBJETO || _CurrentToken._Token ==  DVAR ||
		_CurrentToken._Token ==  DCONST || _CurrentToken._Token == EXP_END ||  _CurrentToken._Token == INICIO ||
		_CurrentToken._Token == ENDFILE){
			switch(_CurrentToken._Token){
			case DPONTO:
				CT(DPONTO);
				CT(EXP_END);
				DPontoD();
				break;
			case DCOR:
				CT(DCOR);
				CT(EXP_END);
				DColorD();
				break;
			case DLUZ:
				CT(DLUZ);
				CT(EXP_END);
				DLuzD();
				break;
			case DFACE:
				CT(DFACE);
				CT(EXP_END);
				DFaceD();
				break;
			case DOBJETO:
				CT(DOBJETO);
				CT(EXP_END);
				ObjD();
				break;
			case DVAR:
				CT(DVAR);
				CT(EXP_END);
				DVarD();
				break;
			case DCONST:
				CT(DCONST);
				CT(EXP_END);
				DConstD();
				break;
			case EXP_END:
				CT(EXP_END);
				break;
			case INICIO:
				Block();
				return;
			case ENDFILE:
				exit (0);//Sucesso!
				break;
			}//end swtich
	}//end while
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DPontoD(){
	do{
		CT(ID);
		string _idLex = _PreviousToken._Lex;

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_PONTO);//(2) - SEMÂNTICO

		CT(EQ);
		CT(LPAREN);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);


		CT(COMMA);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);
		
		CT(COMMA);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);

		CT(RPAREN);
		CT(EXP_END);
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DLuzD(){
	do{
		CT(ID);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_PONTO);//(2) - VERMELHO

		CT(EQ);
		CT(LPAREN);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);
		
		CT(COMMA);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);
		CT(COMMA);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);

		CT(RPAREN);
		CT(EXP_END);
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DColorD(){
	do{
		CT(ID);
		
		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_COR);//(2) - VERMELHO

		CT(EQ);
		CT(CONSTANT);
			
		_Sem->tTypeVerify(_PreviousToken, TIPO_INTEIRO);//(34) - AZUL

		CT(COMMA);
		CT(CONSTANT);

		_Sem->tTypeVerify(_PreviousToken, TIPO_INTEIRO);//(34) - AZUL
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - AZUL

		CT(COMMA);
		CT(CONSTANT);

		_Sem->tTypeVerify(_PreviousToken, TIPO_INTEIRO);//(34) - AZUL
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - AZUL

		CT(COMMA);
		CT(CONSTANT);

		_Sem->tTypeVerify(_PreviousToken, TIPO_INTEIRO);//(34) - AZUL
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - AZUL

		CT(COMMA);
		CT(CONSTANT);

		_Sem->tTypeVerify(_PreviousToken, TIPO_INTEIRO);//(34) - AZUL
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - AZUL

		CT(COMMA);
		CT(CONSTANT);

		_Sem->tTypeVerify(_PreviousToken, TIPO_INTEIRO);//(34) - AZUL
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - AZUL

		CT(COMMA);
		CT(CONSTANT);

		_Sem->tTypeVerify(_PreviousToken, TIPO_INTEIRO);//(34) - AZUL
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - AZUL

		CT(EXP_END);
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DFaceD(){
	do{
		CT(ID);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_FACE);//(2) - VERMELHO

		CT(EQ);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_COR);//(5) - VERDE

		CT(COMMA);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - VERDE

		CT(COMMA);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - VERDE

		CT(COMMA);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - VERDE

		while(_CurrentToken._Token == COMMA){
			CT(COMMA);
			CT(ID);

			_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
			_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - VERDE
		}
		CT(EXP_END);
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::ObjD(){
	do{
		CT(ID);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_OBJETO);//(2) - VERMELHO

		CT(EQ);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_FACE);//(6) - VERDE

		while(_CurrentToken._Token == COMMA){
			CT(COMMA);
			CT(ID);
			
			_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
			_Sem->classVerify(_PreviousToken, CLASSE_FACE);//(6) - VERDE
		}
		CT(EXP_END);
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DVarD(){
	while(_CurrentToken._Token == INTEIRO || _CurrentToken._Token == REAL){
		Type _tipo = TIPO_VAZIO; //(10) - AZUL

		if(_CurrentToken._Token == INTEIRO){
			CT(INTEIRO);
			_tipo = TIPO_INTEIRO; //(10) - AZUL
		}
		else{
			CT(REAL);
			_tipo = TIPO_REAL; //(11) - AZUL
		}
		
		CT(ID);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_VAR);//(2) - VERMELHO
		_Sem->setType(_PreviousToken, _tipo); //(12) - AZUL

		while(_CurrentToken._Token == COMMA){
			CT(COMMA);
			CT(ID);

			_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_VAR);//(2) - VERMELHO
			_Sem->setType(_PreviousToken, _tipo); //(12) - AZUL
		}
		CT(EXP_END);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DConstD(){
	while(_CurrentToken._Token == ID){
		CT(ID);

		ATW_BUFF_ELEMENT _IdRef = _PreviousToken; // (13) - AZUL - AUXILIAR

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_CONST);//(2) - VERMELHO

		CT(EQ);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);
		_Sem->sSetType(_IdRef, _PreviousToken);// (13) - AZUL

		CT(EXP_END);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::Block(){
	if(_CurrentToken._Token == INICIO){
		CT(INICIO);
		CT(EXP_END);
		while( _CurrentToken._Token == EXP_END || _CurrentToken._Token == ID || _CurrentToken._Token == ENQUANTO || 
			_CurrentToken._Token == ESCALA || _CurrentToken._Token == PAUSA || _CurrentToken._Token == ROTTRANS ||
			_CurrentToken._Token == SE || _CurrentToken._Token == LUZ){
				Command();
		}
		CT(FIM);
		CT(EXP_END);
	}else{
		void* _Param[2] = {(void*)_CurrentToken._LINE,(void*)_CurrentToken._Lex};
		_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::Command(){
	InfoBundle _auxInfo;//PARA VERIFICAÇÃO DO QUE SEJA NECESSÁRIO
	Type _vType[2] = {TIPO_INTEIRO, TIPO_REAL};//(32)(34) - AZUL
	ATW_BUFF_ELEMENT _idAux;//(32) - AZUL

	switch(_CurrentToken._Token){
	case ID:
		CT(ID);
		
		_idAux = _PreviousToken;//(32) - AZUL
		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_VAR);//(8) - VERDE

		CT(EQ);

		Exp(&_auxInfo);
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(31) - AZUL
		_Sem->tTypeVerify(_idAux, _auxInfo._Type);//(31) - AZUL

		CT(EXP_END);
		break;
	case ENQUANTO:
		CT(ENQUANTO);

		 Exp(&_auxInfo);//(32) - AZUL
		_Sem->TypeVerify(_PreviousToken, _auxInfo._Type, TIPO_LOGICO);//(32) - AZUL

		CT(FACA);

		if(_CurrentToken._Token == INICIO)
			Block();
		else if(_CurrentToken._Token == ID || _CurrentToken._Token == ENQUANTO || _CurrentToken._Token == ESCALA || 
			_CurrentToken._Token == PAUSA || _CurrentToken._Token == ROTTRANS || _CurrentToken._Token == SE)
			Command();
		else {
			void* _Param[2] = {(void*)_CurrentToken._LINE,(void*)_CurrentToken._Lex};
			_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
		}
		break;
	case ESCALA:
		CT(ESCALA);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_OBJETO);//(7) - VERDE

		CT(COMMA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(34) - AZUL

		CT(EXP_END);
		break;
	case PAUSA:
		CT(PAUSA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(34) - AZUL
		
		CT(EXP_END);
		break;
	case LUZ:
		CT(LUZ);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_LUZ);//(4) - VERDE

		CT(EXP_END);
		break;
	case ROTTRANS:
		CT(ROTTRANS);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->classVerify(_PreviousToken, CLASSE_OBJETO);//(7) - VERDE

		CT(COMMA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - AZUL

		CT(COMMA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - AZUL
		
		CT(COMMA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - AZUL
		
		CT(COMMA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - AZUL
		
		CT(COMMA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - AZUL

		CT(COMMA);

		Exp(&_auxInfo);//(33) - AZUL
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - AZUL

		CT(EXP_END);
		break;
	case SE:
		CT(SE);

		Exp(&_auxInfo);//(32) - AZUL
		_Sem->TypeVerify(_PreviousToken, _auxInfo._Type, TIPO_LOGICO);//(32) - AZUL

		CT(ENTAO);

		if(_CurrentToken._Token == INICIO)
			Block();
		else if(_CurrentToken._Token == ID || _CurrentToken._Token == ENQUANTO || _CurrentToken._Token == ESCALA ||
			_CurrentToken._Token == PAUSA || _CurrentToken._Token == ROTTRANS || _CurrentToken._Token == SE)
			Command();
		else {
			void* _Param[2] = {(void*)_CurrentToken._LINE,(void*)_CurrentToken._Lex};
			_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
		}

		if(_CurrentToken._Token == SENAO){
			CT(SENAO);

			if(_CurrentToken._Token == INICIO)
				Block();
			else if(_CurrentToken._Token == ID || _CurrentToken._Token == ENQUANTO || _CurrentToken._Token == ESCALA || 
				_CurrentToken._Token == PAUSA || _CurrentToken._Token == ROTTRANS || _CurrentToken._Token == SE)
				Command();
			else{
				void* _Param[2] = {(void*)_CurrentToken._LINE,(void*) _CurrentToken._Lex};
				_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
			}
		}
		break;
	case EXP_END:
		CT(EXP_END);
		break;
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::Exp(InfoBundle* _inf){
	EXPS(_inf);//(29) - AZUL

	if(_CurrentToken._Token == LT || _CurrentToken._Token == GT || _CurrentToken._Token == LE || 
		_CurrentToken._Token == GE || _CurrentToken._Token == EQ || _CurrentToken._Token == DIFF){
			InfoBundle _Exp1S;//(30) - AZUL
			_Exp1S.build_element(-1,"");//(30) - AZUL

			R();
			EXPS(&_Exp1S);

			_Sem->TypeVerify(_PreviousToken, _inf->_Type, TIPO_LOGICO);
			_Sem->TypeVerify(_PreviousToken, _Exp1S._Type, TIPO_LOGICO);

			_inf->_Type = TIPO_LOGICO;//(30) - AZUL
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::R(){
	switch(_CurrentToken._Token){
	case LT:
		CT(LT);
		break;
	case GT:
		CT(GT);
		break;
	case LE:
		CT(LE);
		break;
	case GE:
		CT(GE);
		break;
	case EQ:
		CT(EQ);
		break;
	case DIFF:
		CT(DIFF);
		break;
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::EXPS(InfoBundle* _inf){
	Token _tOp = PLUS;

	if(_CurrentToken._Token == PLUS){
		CT(PLUS);
	}
	else if(_CurrentToken._Token == MINUS){
		CT(MINUS);
		_tOp = MINUS;
	}

	T(_inf);//(23) - AZUL

	while(_CurrentToken._Token == PLUS || _CurrentToken._Token == MINUS || _CurrentToken._Token == OU){
		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
			_tOp = PLUS;
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
			_tOp = MINUS;
		}
		else if(_CurrentToken._Token == OU){
			CT(OU);
			_tOp = OU;
		}

		InfoBundle _auxInf;
		_auxInf.build_element(-1, "");
		T(&_auxInf);

		//(27) - AZUL --------------------------------------------------------------------------------
		//(27) - AZUL --------------------------------------------------------------------------------
		Type _ExpsT = _inf->_Type;
		Type _T1    = _auxInf._Type;

		if(_tOp == PLUS || _tOp == MINUS){
			if(_ExpsT == TIPO_INTEIRO && _T1 == TIPO_INTEIRO)
				_inf->_Type = TIPO_INTEIRO;
			else if(_ExpsT == TIPO_INTEIRO && _T1 == TIPO_REAL)
				_inf->_Type = TIPO_REAL;
			else if(_ExpsT == TIPO_REAL && _T1 == TIPO_INTEIRO)
				_inf->_Type = TIPO_REAL;
			else if(_ExpsT == TIPO_REAL && _T1 == TIPO_REAL)
				_inf->_Type = TIPO_REAL;
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}else{
			if (_ExpsT == TIPO_LOGICO && _T1 == TIPO_LOGICO)
				_inf->_Type = TIPO_LOGICO;
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}
		//(27) - AZUL --------------------------------------------------------------------------------
		//(27) - AZUL --------------------------------------------------------------------------------
	}//END WHILE
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::T(InfoBundle* _inf){	
	 F(_inf);//(18) - AZUL

	while(_CurrentToken._Token == TIMES || _CurrentToken._Token == OVER || _CurrentToken._Token == E){
		Token _tOp;

		if(_CurrentToken._Token == TIMES){
			CT(TIMES);
			_tOp = TIMES;
		}else if(_CurrentToken._Token == OVER){
			CT(OVER);
			_tOp = OVER;
		}
		else{
			CT(E);
			_tOp = E;
		}

		InfoBundle _auxInf;
		_auxInf.build_element(-1, "");
		F(&_auxInf);

		//(22) - AZUL --------------------------------------------------------------------------------
		//(22) - AZUL --------------------------------------------------------------------------------
		Type _T     = _inf->_Type;
		Type _F1    = _auxInf._Type;

		if(_tOp == TIMES){
			if(_T == TIPO_INTEIRO && _F1 == TIPO_INTEIRO)
				_inf->_Type = TIPO_INTEIRO;
			else if(_T == TIPO_INTEIRO && _F1 == TIPO_REAL)
				_inf->_Type = TIPO_REAL;
			else if(_T == TIPO_REAL && _F1 == TIPO_INTEIRO)
				_inf->_Type = TIPO_REAL;
			else if(_T == TIPO_REAL && _F1 == TIPO_REAL)
				_inf->_Type = TIPO_REAL;
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}else if(_tOp == OVER){
			if(_T == TIPO_INTEIRO && _F1 == TIPO_INTEIRO)
				_inf->_Type = TIPO_INTEIRO;
			else if(_T == TIPO_INTEIRO && _F1 == TIPO_REAL)
				_inf->_Type = TIPO_REAL;
			else if(_T == TIPO_REAL && _F1 == TIPO_INTEIRO)
				_inf->_Type = TIPO_REAL;
			else if(_T == TIPO_REAL && _F1 == TIPO_REAL)
				_inf->_Type = TIPO_REAL;
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}else{
			if (_T == TIPO_LOGICO && _F1 == TIPO_LOGICO)
				_inf->_Type = TIPO_LOGICO;
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}
		//(22) - AZUL --------------------------------------------------------------------------------
		//(22) - AZUL --------------------------------------------------------------------------------
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::F(InfoBundle* _inf){
	Class _class[2] = {CLASSE_CONST, CLASSE_VAR};//(9) - VERDE
	ATW_BUFF_ELEMENT _tAux = _PreviousToken;//(16) - AZUL

	switch(_CurrentToken._Token){
	case LPAREN:
		CT(LPAREN);
		
		Exp(_inf);//(17) - AZUL

		CT(RPAREN);
		break;
	case NAO:
		CT(NAO);
	
		F(_inf);//(16) - AZUL
		_Sem->TypeVerify(_tAux, _inf->_Type, TIPO_LOGICO);//(16) - AZUL

		break;
	case ID:
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - VERMELHO
		_Sem->nClassVerify(_PreviousToken, 2, _class);//(8) - VERDE
		_inf->_Type = _Sem->getType(_PreviousToken);//(14) - AZUL

		break;
	case CONSTANT:
		CT(CONSTANT);

		_inf->_Type = _PreviousToken._Tipo;//(15) - AZUL

		break;
	default:
		void* _Param[2] = {(void*)_CurrentToken._LINE,(void*) _CurrentToken._Lex};
		_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
		break;
	}
}