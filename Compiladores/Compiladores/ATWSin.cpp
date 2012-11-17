#include "ATW_MemoryManager.h"
#include "ATWSin.h"
#include "Globals.h"
#include "FileHelper.h"
#include "ERRO_DEF.h"
#include "ARS_PrinterHelper.h"
#include <Windows.h>
#include "ATW_Helper.h"
//---------------------------------------------------------------------------------------------------------------------
using namespace ARS_PRINTER_HELPER;
using namespace MemoryManager;
using namespace ATW_HELPER;
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
	void* _args[1] = {"saida.asm"};
	_cg->initialize(1, _args);
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
				_memory->GCXSetBaseTempAddress();//45
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
		char _IdLex[255];
		strcpy_s(_IdLex, _PreviousToken._Lex);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_PONTO);//(2) - SEMÂNTICO

		CT(EQ);
		CT(LPAREN);

		char* _op = "";
		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
			_op = "-";
		}

		CT(CONSTANT);

		char _str[255];
		strcpy_s(_str, _op);
		strcat_s(_str, _PreviousToken._Lex);
		_cg->STIF(_str, _Sem->updateIDAddress(_IdLex, _memory->ATWMalloc(TIPO_REAL)), _IdLex);//36
		memset(_str, 0, sizeof(char)*255);

		CT(COMMA);

		_op = "";
		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
			_op = "-";
		}

		CT(CONSTANT);
	
		strcpy_s(_str, _op);
		strcat_s(_str, _PreviousToken._Lex);
		_cg->STIF(_str, _memory->ATWMalloc(TIPO_REAL));//37
		memset(_str, 0, sizeof(char)*255);

		CT(COMMA);

		_op = "";
		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
			_op = "-";
		}

		CT(CONSTANT);

		strcpy_s(_str, _op);
		strcat_s(_str, _PreviousToken._Lex);
		_cg->STIF(_str, _memory->ATWMalloc(TIPO_REAL));//37
		memset(_str, 0, sizeof(char)*255);

		CT(RPAREN);
		CT(EXP_END);
		_cg->flush();
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DLuzD(){
	do{
		CT(ID);
		char _IdLex[255];
		strcpy_s(_IdLex, _PreviousToken._Lex);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_LUZ);//(2) - SEMÂNTICO

		CT(EQ);
		CT(LPAREN);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);

		_cg->STIF(_PreviousToken._Lex, _Sem->updateIDAddress(_IdLex, _memory->ATWMalloc(TIPO_REAL)), _IdLex);//36

		CT(COMMA);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);
		
		_cg->STIF(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_REAL));//37

		CT(COMMA);

		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
		}

		CT(CONSTANT);

		_cg->STIF(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_REAL));//37

		CT(RPAREN);
		CT(EXP_END);
		_cg->flush();
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DColorD(){
	do{
		CT(ID);
		char _IdLex[255];
		strcpy_s(_IdLex, _PreviousToken._Lex);
		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_COR);//(2) - SEMÂNTICO
		
		CT(EQ);
		CT(CONSTANT);

		_Sem->TypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_INTEIRO);//(34) - SEMÂNTICO
		_cg->STI(_PreviousToken._Lex, _Sem->updateIDAddress(_IdLex, _memory->ATWMalloc(TIPO_INTEIRO)), _IdLex);//38

		CT(COMMA);
		CT(CONSTANT);

		_Sem->TypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_INTEIRO);//(34) - SEMÂNTICO
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - SEMÂNTICO
		_cg->STI(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO));//39


		CT(COMMA);
		CT(CONSTANT);

		_Sem->TypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_INTEIRO);//(34) - SEMÂNTICO
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - SEMÂNTICO
		_cg->STI(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO));//39

		CT(COMMA);
		CT(CONSTANT);

		_Sem->TypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_INTEIRO);//(34) - SEMÂNTICO
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - SEMÂNTICO
		_cg->STI(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO));//39

		CT(COMMA);
		CT(CONSTANT);

		_Sem->TypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_INTEIRO);//(34) - SEMÂNTICO
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - SEMÂNTICO
		_cg->STI(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO));//39

		CT(COMMA);
		CT(CONSTANT);

		_Sem->TypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_INTEIRO);//(34) - SEMÂNTICO
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - SEMÂNTICO
		_cg->STI(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO));//39

		CT(COMMA);
		CT(CONSTANT);

		_Sem->TypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_INTEIRO);//(34) - SEMÂNTICO
		_Sem->ValRestriction(_PreviousToken, 15, VR_GREATER);//(35) - SEMÂNTICO
		_cg->STI(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO));//39

		CT(EXP_END);
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DFaceD(){
	do{
		int _pointCount = -1;//40
		int _cAddress   = 0;//43

		CT(ID);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_FACE);//(2) - SEMÂNTICO
		_cAddress = _cg->STI("LIXO", _Sem->updateIDAddress(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO)), _PreviousToken._Lex);//40

		CT(EQ);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_COR);//(5) - SEMÂNTICO
		_cg->STI(ATWgetCStr(_PreviousToken._End), _memory->ATWMalloc(TIPO_INTEIRO));//41
		_pointCount++;

		CT(COMMA);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - SEMÂNTICO
		_cg->STI(ATWgetCStr(_PreviousToken._End), _memory->ATWMalloc(TIPO_INTEIRO));//41
		_pointCount++;

		CT(COMMA);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - SEMÂNTICO
		_cg->STI(ATWgetCStr(_PreviousToken._End), _memory->ATWMalloc(TIPO_INTEIRO));//41
		_pointCount++;

		CT(COMMA);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - SEMÂNTICO
		_cg->STI(ATWgetCStr(_PreviousToken._End), _memory->ATWMalloc(TIPO_INTEIRO));//41
		_pointCount++;

		while(_CurrentToken._Token == COMMA){
			CT(COMMA);
			CT(ID);

			_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
			_Sem->classVerify(_PreviousToken, CLASSE_PONTO);//(3) - SEMÂNTICO
			_cg->STI(ATWgetCStr(_PreviousToken._End), _memory->ATWMalloc(TIPO_INTEIRO));//41
			_pointCount++;
		}
		CT(EXP_END);
		_cg->fixCode(_cAddress+1, ATWgetCStr(_pointCount));//42
		_cg->flush();
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::ObjD(){
	do{
		int _objectCount = 0;
		int _cAddress    = 0;

		CT(ID);
		char _IdLex[255];
		strcpy_s(_IdLex, _PreviousToken._Lex);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_OBJETO);//(2) - SEMÂNTICO
		_cAddress = _cg->STI("LIXO", _Sem->updateIDAddress(_PreviousToken._Lex, _memory->ATWMalloc(TIPO_INTEIRO)), _IdLex);//43
		_cg->STIF("1.0", _memory->ATWMalloc(TIPO_REAL));//43

		CT(EQ);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_FACE);//(6) - SEMÂNTICO
		_cg->STI(ATWgetCStr(_PreviousToken._End), _memory->ATWMalloc(TIPO_INTEIRO));//41
		_objectCount++;

		while(_CurrentToken._Token == COMMA){
			CT(COMMA);
			CT(ID);
			
			_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
			_Sem->classVerify(_PreviousToken, CLASSE_FACE);//(6) - SEMÂNTICO
			_cg->STI(ATWgetCStr(_PreviousToken._End), _memory->ATWMalloc(TIPO_INTEIRO));//41
			_objectCount++;
		}
		CT(EXP_END);
		_cg->fixCode(_cAddress+1, ATWgetCStr(_objectCount));//44
		_cg->flush();
	}while(_CurrentToken._Token == ID);
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DVarD(){
	while(_CurrentToken._Token == INTEIRO || _CurrentToken._Token == REAL){
		Type _tipo = TIPO_VAZIO; //(10) - SEMÂNTICO

		if(_CurrentToken._Token == INTEIRO){
			CT(INTEIRO);
			_tipo = TIPO_INTEIRO; //(10) - SEMÂNTICO
		}
		else{
			CT(REAL);
			_tipo = TIPO_REAL; //(11) - SEMÂNTICO
		}
		
		CT(ID);

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_VAR);//(2) - SEMÂNTICO
		_Sem->setType(_PreviousToken, _tipo); //(12) - SEMÂNTICO
		_Sem->updateIDAddress(_PreviousToken._Lex, _memory->ATWMalloc(_tipo));//12

		while(_CurrentToken._Token == COMMA){
			CT(COMMA);
			CT(ID);

			_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_VAR);//(2) - SEMÂNTICO
			_Sem->setType(_PreviousToken, _tipo); //(12) - SEMÂNTICO
			_Sem->updateIDAddress(_PreviousToken._Lex, _memory->ATWMalloc(_tipo));//12
		}
		CT(EXP_END);
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::DConstD(){
	while(_CurrentToken._Token == ID){
		CT(ID);
		char _IdLex[255];
		strcpy_s(_IdLex, _PreviousToken._Lex);

		ATW_BUFF_ELEMENT _IdRef = _PreviousToken; // (13) - SEMÂNTICO - AUXILIAR

		_Sem->unicidadeAlreadyDeclared(_PreviousToken, CLASSE_CONST);//(2) - SEMÂNTICO

		CT(EQ);

		char* _op = "";
		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
			_op = "-";
		}

		CT(CONSTANT);
		_Sem->sSetType(_IdRef, _PreviousToken);//(13) - SEMÂNTICO
		int _memDesloc = _Sem->updateIDAddress(_IdLex, _memory->ATWMalloc(_PreviousToken._Tipo));//13

		char _str[255];
		strcpy_s(_str, _op);
		strcat_s(_str, _PreviousToken._Lex);

		Type _type = _PreviousToken._Tipo;
		if(_type == TIPO_INTEIRO)
			_cg->STI(_str, _memDesloc, _IdLex);
		else
			_cg->STIF(_str, _memDesloc, _IdLex);

		CT(EXP_END);
		_cg->flush();
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
	Type _vType[2] = {TIPO_INTEIRO, TIPO_REAL};//(32)(34) - SEMÂNTICO
	ATW_BUFF_ELEMENT _idAux;//(32) - SEMÂNTICO

	switch(_CurrentToken._Token){
	case ID:
		CT(ID);
		
		_idAux = _PreviousToken;//(32) - SEMÂNTICO
		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_VAR);//(8) - SEMÂNTICO

		CT(EQ);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(31) - SEMÂNTICO
		_Sem->tTypeVerify(_idAux, _auxInfo._Type);//(31) - SEMÂNTICO

		CT(EXP_END);
		break;
	case ENQUANTO:
		CT(ENQUANTO);

		_memory->ATWResetTemp();//46
		 Exp(&_auxInfo);//(32) - SEMÂNTICO
		_Sem->TypeVerify(_PreviousToken, _auxInfo._Type, TIPO_LOGICO);//(32) - SEMÂNTICO

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

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_OBJETO);//(7) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(34) - SEMÂNTICO

		CT(EXP_END);
		break;
	case PAUSA:
		CT(PAUSA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(34) - SEMÂNTICO
		
		CT(EXP_END);
		break;
	case LUZ:
		CT(LUZ);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_LUZ);//(4) - SEMÂNTICO

		CT(EXP_END);
		break;
	case ROTTRANS:
		CT(ROTTRANS);
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_OBJETO);//(7) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - SEMÂNTICO
		
		CT(COMMA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - SEMÂNTICO
		
		CT(COMMA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - SEMÂNTICO
		
		CT(COMMA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(33) - SEMÂNTICO
		_Sem->nTypeVerify(_PreviousToken, _auxInfo._Type, 2, _vType);//(33) - SEMÂNTICO

		CT(EXP_END);
		break;
	case SE:
		CT(SE);

		_memory->ATWResetTemp();//46
		Exp(&_auxInfo);//(32) - SEMÂNTICO
		_Sem->TypeVerify(_PreviousToken, _auxInfo._Type, TIPO_LOGICO);//(32) - SEMÂNTICO

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
void ATWSin::Exp(InfoBundle* _inf, const int& _fAddress){
	EXPS(_inf);//(29) - SEMÂNTICO

	if(_CurrentToken._Token == LT || _CurrentToken._Token == GT || _CurrentToken._Token == LE || 
		_CurrentToken._Token == GE || _CurrentToken._Token == EQ || _CurrentToken._Token == DIFF){
			InfoBundle _Exp1S;//(30) - SEMÂNTICO
			_Exp1S.build_element(-1,"");//(30) - SEMÂNTICO

			R();
			EXPS(&_Exp1S);

			_Sem->TypeVerify(_PreviousToken, _inf->_Type, TIPO_LOGICO);
			_Sem->TypeVerify(_PreviousToken, _Exp1S._Type, TIPO_LOGICO);

			_inf->_Type = TIPO_LOGICO;//(30) - SEMÂNTICO
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
void ATWSin::EXPS(InfoBundle* _inf, const int& _fAddress){
	Token _tOp = PLUS;

	if(_CurrentToken._Token == PLUS){
		CT(PLUS);
	}
	else if(_CurrentToken._Token == MINUS){
		CT(MINUS);
		_tOp = MINUS;
	}

	T(_inf);//(23) - SEMÂNTICO

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

		//(27) - SEMÂNTICO --------------------------------------------------------------------------------
		//(27) - SEMÂNTICO --------------------------------------------------------------------------------
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
		//(27) - SEMÂNTICO --------------------------------------------------------------------------------
		//(27) - SEMÂNTICO --------------------------------------------------------------------------------
	}//END WHILE
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::T(InfoBundle* _inf, const int& _fAddress){	
	 F(_inf);//(18) - SEMÂNTICO

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

		//(22) - SEMÂNTICO --------------------------------------------------------------------------------
		//(22) - SEMÂNTICO --------------------------------------------------------------------------------
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
		//(22) - SEMÂNTICO --------------------------------------------------------------------------------
		//(22) - SEMÂNTICO --------------------------------------------------------------------------------
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::F(InfoBundle* _inf, int& _fAddress){
	Class _class[2] = {CLASSE_CONST, CLASSE_VAR};//(9) - SEMÂNTICO
	ATW_BUFF_ELEMENT _tAux = _PreviousToken;//(16) - SEMÂNTICO

	switch(_CurrentToken._Token){
	case LPAREN:
		CT(LPAREN);
		
		_memory->ATWResetTemp();
		Exp(_inf);//(17) - SEMÂNTICO

		CT(RPAREN);
		break;
	case NAO:
		CT(NAO);
	
		F(_inf);//(16) - SEMÂNTICO
		_Sem->TypeVerify(_tAux, _inf->_Type, TIPO_LOGICO);//(16) - SEMÂNTICO

		break;
	case ID:
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->nClassVerify(_PreviousToken, 2, _class);//(8) - SEMÂNTICO
		_inf->_Type = _Sem->getType(_PreviousToken);//(14) - SEMÂNTICO

		break;
	case CONSTANT:
		CT(CONSTANT);

		_inf->_Type = _PreviousToken._Tipo;//(15) - SEMÂNTICO
		_fAddress = _memory->ATWNovoTemp();//15 - COD

		break;
	default:
		void* _Param[2] = {(void*)_CurrentToken._LINE,(void*) _CurrentToken._Lex};
		_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
		break;
	}
}