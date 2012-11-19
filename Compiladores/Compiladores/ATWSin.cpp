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
				DObjD();
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
void ATWSin::DObjD(){
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
	Type _ExpType;
	int _ExpAdr = 0, _Exp1Adr = 0, _Exp2Adr = 0, _Exp3Adr = 0, _Exp4Adr = 0, _Exp5Adr = 0;//(59) - COD
	ATW_BUFF_ELEMENT _idAux;//(32) - SEMÂNTICO

	switch(_CurrentToken._Token){
	case ID:
		CT(ID);
		
		_idAux = _PreviousToken;//(32) - SEMÂNTICO
		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_VAR);//(8) - SEMÂNTICO

		CT(EQ);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _ExpAdr);
		
		//(31) - SEMÂNTICO ------------------------------------------------------------------
		//(31) - COD ------------------------------------------------------------------------		
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//Se ExpTipo != tipo-logico entao...

		if(_idAux._Tipo == TIPO_INTEIRO)
		{
			_Sem->TypeVerify(_PreviousToken, _ExpType, TIPO_INTEIRO);//Se ExpTipo = tipo-inteiro entao...
			_cg->LOD("A", _ExpAdr, "ID = EXP");
			_cg->STO("A", _idAux._End);
		}//end if
		else
		{
			if(_ExpType == TIPO_INTEIRO)
			{
				_cg->LOD("A", _ExpAdr, "ID = EXP");
				_cg->CNV("A", "A");
				_cg->STOF("A", _idAux._End);
			}
			else//TIPO_REAL
			{
				_cg->LODF("A", _ExpAdr, "ID = EXP");
				_cg->STOF("A", _idAux._End);
			}
		}//end else
		//(31) - COD ------------------------------------------------------------------------
		//(31) - SEMÂNTICO ------------------------------------------------------------------
		CT(EXP_END);
		break;
	case ENQUANTO:
		CT(ENQUANTO);

		_memory->ATWResetTemp();//(46) - COD
		 Exp(_ExpType, _ExpAdr);//(32) - SEMÂNTICO
		_Sem->TypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(32) - SEMÂNTICO

		CT(FACA);

		if(_CurrentToken._Token == INICIO)
			Block();
		else if(_CurrentToken._Token == ID || _CurrentToken._Token == ENQUANTO || _CurrentToken._Token == ESCALA || 
			_CurrentToken._Token == PAUSA || _CurrentToken._Token == ROTTRANS || _CurrentToken._Token == SE)
			Command();
		else
		{
			void* _Param[2] = {(void*)_CurrentToken._LINE,(void*)_CurrentToken._Lex};
			_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
		}
		break;
	case ESCALA:
		CT(ESCALA);
		CT(ID);

		_idAux = _PreviousToken;//(57) - COD
		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_OBJETO);//(7) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _ExpAdr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO

		//(57) - COD -------------------------------------------------------------------------
		_cg->LOD("A", _idAux._End, "ESCALA ID EXP");
		if(_ExpType == TIPO_INTEIRO)
		{
			_cg->LOD("B", _ExpAdr);
			_cg->CNV("B", "B");
		}//end if
		else
		{
			_cg->LODF("B", _ExpAdr);
		}//end else

		_cg->ESC("A", "B");
		//(57) - COD -------------------------------------------------------------------------

		CT(EXP_END);
		break;
	case PAUSA:
		CT(PAUSA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _ExpAdr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO

		//(58) - COD -------------------------------------------------------------------------		
		if(_ExpType == TIPO_INTEIRO)
		{
			_cg->LOD("A", _ExpAdr, "PAUSA EXP");
			_cg->CNV("A", "A");
		}//end if
		else
		{
			_cg->LOD("A", _ExpAdr, "PAUSA EXP");
		}//end else

		_cg->TME("A");
		//(58) - COD -------------------------------------------------------------------------
		
		CT(EXP_END);
		break;
	case LUZ:
		CT(LUZ);
		CT(ID);

		_idAux = _PreviousToken;//(56) - COD
		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_LUZ);//(4) - SEMÂNTICO
		
		//(56) - SEMANTICO -------------------------------------------------------------------
		//(56) - COD -------------------------------------------------------------------------
		_Sem->DiffTypeVerify(_PreviousToken, _PreviousToken._Tipo, TIPO_REAL);
		_cg->LOD("A", _idAux._End, "LUZ ID");
		_cg->LGT("A");
		//(56) - COD -------------------------------------------------------------------------
		//(56) - SEMANTICO -------------------------------------------------------------------

		CT(EXP_END);
		break;
	case ROTTRANS:
		CT(ROTTRANS);
		CT(ID);

		_idAux = _PreviousToken;//(59) - COD

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->classVerify(_PreviousToken, CLASSE_OBJETO);//(7) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _ExpAdr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _Exp1Adr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO
		
		CT(COMMA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _Exp2Adr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO
		
		CT(COMMA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _Exp3Adr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO
		
		CT(COMMA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _Exp4Adr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO

		CT(COMMA);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _Exp5Adr);//(33) - SEMÂNTICO
		_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(33) - SEMÂNTICO

		//(59) - COD -------------------------------------------------------------------------
		_cg->LOD("A", _idAux._End, "ROTTRANS");
		_cg->LODF("A", _ExpAdr);
		_cg->LODF("B", _Exp1Adr);
		_cg->LODF("C", _Exp2Adr);
		_cg->LODF("D", _Exp3Adr);
		_cg->LODF("E", _Exp4Adr);
		_cg->LODF("F", _Exp5Adr);
		_cg->RTR();
		//(56) - COD -------------------------------------------------------------------------

		CT(EXP_END);
		break;
	case SE:
		CT(SE);

		_memory->ATWResetTemp();//(46) - COD
		Exp(_ExpType, _ExpAdr);//(32) - SEMÂNTICO
		_Sem->TypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(32) - SEMÂNTICO

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
	_cg->flush();
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::Exp(Type& _ExpType, int& _ExpAdr){
	Type _ExpSType, _ExpS1Type;
	int _ExpSAdr = 0, _ExpS1Adr = 0;

	EXPS(_ExpSType, _ExpSAdr);//(29) - SEMÂNTICO
	_ExpType = _ExpSType;
	_ExpAdr = _ExpSAdr;

	if(_CurrentToken._Token == LT || _CurrentToken._Token == GT || _CurrentToken._Token == LE || 
		_CurrentToken._Token == GE || _CurrentToken._Token == EQ || _CurrentToken._Token == DIFF){
			Token _ROp;	
			R(_ROp);

			_Sem->DiffTypeVerify(_PreviousToken, _ExpType, TIPO_LOGICO);//(30) - SEMÂNTICO

			EXPS(_ExpS1Type, _ExpS1Adr);

			_Sem->DiffTypeVerify(_PreviousToken, _ExpS1Type, TIPO_LOGICO);//(28) - SEMÂNTICO

			//(28) - COD -----------------------------------------------------------------------------
			if(_ExpType == TIPO_INTEIRO)
			{
				_cg->LOD("A", _ExpAdr);
				if(_ExpSType == TIPO_INTEIRO)
				{
					_cg->LOD("B", _ExpSAdr);
					_ExpType = TIPO_INTEIRO;
				}
				else
				{
					_cg->CNV("A", "A");
					_cg->LODF("B", _ExpSAdr);
					_ExpType = TIPO_REAL;
				}
			}//end if
			else
			{
				_cg->LODF("A", _ExpAdr);
				if(_ExpSType == TIPO_INTEIRO)
				{
					_cg->LOD("B", _ExpSAdr);
					_cg->CNV("B", "B");
					_ExpType = TIPO_REAL;
				}
				else
				{
					_cg->LODF("B", _ExpSAdr);
					_ExpType = TIPO_REAL;
				}
			}//end else

			if(_ExpType == TIPO_INTEIRO)
			{
				_cg->SUB("A","B");
				//TODO: Ação 28 - Geração de código de Saltos
			}//end if
			else
			{
				_cg->SUBF("A","B");
				//TODO: Ação 28 - Geração de código de Saltos
			}//end else
			//(28) - COD -----------------------------------------------------------------------------
			_ExpType = TIPO_LOGICO;
	}
	_cg->flush();
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::R(Token& _ROp){
	switch(_CurrentToken._Token){
	case LT:
		CT(LT);
		_ROp = LT;//(50) - COD
		break;
	case GT:
		CT(GT);
		_ROp = GT;//(51) - COD
		break;
	case LE:
		CT(LE);
		_ROp = LE;//(52) - COD
		break;
	case GE:
		CT(GE);
		_ROp = GE;//(53) - COD
		break;
	case EQ:
		CT(EQ);
		_ROp = EQ;//(54) - COD
		break;
	case DIFF:
		CT(DIFF);
		_ROp = DIFF;//(55) - COD
		break;
	}
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::EXPS(Type& _ExpSType, int & _ExpSAdr){
	Token _ExpSOp = PLUS;
	int _TAdr = 0, _T1Adr = 0;
	Type _TType, _T1Type;

	if(_CurrentToken._Token == PLUS){
		CT(PLUS);
	}
	else if(_CurrentToken._Token == MINUS){
		CT(MINUS);
		_ExpSOp = MINUS;
	}

	T(_TType, _TAdr);//(23) - SEMÂNTICO
	_ExpSType = _TType;//(23) - SEMÂNTICO

	//(23) - COD --------------------------------------------------------------------
	if (_ExpSOp == MINUS)
	{
		_ExpSAdr = _memory->ATWNovoTemp(_ExpSType);
		if (_TType == TIPO_INTEIRO)
		{
			_cg->LOD("A", _TAdr);
			_cg->NEG("A");
			_cg->STO("A", _ExpSAdr);
		}//end if
		else
		{
			_cg->LODF("A", _TAdr);
			_cg->NEGF("A");
			_cg->STOF("A", _ExpSAdr);
		}//end else
	}
	else
		_ExpSAdr = _TAdr;
	//(23) - COD --------------------------------------------------------------------

	while(_CurrentToken._Token == PLUS || _CurrentToken._Token == MINUS || _CurrentToken._Token == OU){
		if(_CurrentToken._Token == PLUS){
			CT(PLUS);
			_ExpSOp = PLUS;//(24) - SEMÂNTICO
		}
		else if(_CurrentToken._Token == MINUS){
			CT(MINUS);
			_ExpSOp = MINUS;//(25) - SEMÂNTICO
		}
		else if(_CurrentToken._Token == OU){
			CT(OU);
			_ExpSOp = OU;//(26) - SEMÂNTICO
		}

		T(_T1Type, _T1Adr);

		//(27) - SEMÂNTICO --------------------------------------------------------------------------------
		//(27) - COD --------------------------------------------------------------------------------------

		if(_ExpSOp == PLUS){
			if(_ExpSType == TIPO_INTEIRO && _T1Type == TIPO_INTEIRO)
			{
				_ExpSType = TIPO_INTEIRO;
				_cg->LOD("A", _ExpSAdr);
				_cg->LOD("B", _T1Adr);
				_cg->ADD("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_INTEIRO);
				_cg->STO("A", _ExpSAdr);
			}
			else if(_ExpSType == TIPO_INTEIRO && _T1Type == TIPO_REAL)
			{
				_ExpSType = TIPO_REAL;
				_cg->LOD("A", _ExpSAdr);
				_cg->CNV("A", "A");
				_cg->LODF("B", _T1Adr);
				_cg->ADDF("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _ExpSAdr);
			}
			else if(_ExpSType == TIPO_REAL && _T1Type == TIPO_INTEIRO)
			{
				_ExpSType = TIPO_REAL;
				_cg->LODF("A", _ExpSAdr);
				_cg->LOD("B", _T1Adr);
				_cg->CNV("B", "B");
				_cg->ADDF("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _ExpSAdr);
			}
			else if(_ExpSType == TIPO_REAL && _T1Type == TIPO_REAL)
			{
				_ExpSType = TIPO_REAL;
				_cg->LODF("A", _ExpSAdr);
				_cg->LODF("B", _T1Adr);
				_cg->ADDF("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _ExpSAdr);
			}
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}//end else
		}//end if
		else if (_ExpSOp == MINUS)
		{
			if(_ExpSType == TIPO_INTEIRO && _T1Type == TIPO_INTEIRO)
			{
				_ExpSType = TIPO_INTEIRO;
				_cg->LOD("A", _ExpSAdr);
				_cg->LOD("B", _T1Adr);
				_cg->SUB("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_INTEIRO);
				_cg->STO("A", _ExpSAdr);
			}
			else if(_ExpSType == TIPO_INTEIRO && _T1Type == TIPO_REAL)
			{
				_ExpSType = TIPO_REAL;
				_cg->LOD("A", _ExpSAdr);
				_cg->CNV("A", "A");
				_cg->LODF("B", _T1Adr);
				_cg->SUBF("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _ExpSAdr);
			}
			else if(_ExpSType == TIPO_REAL && _T1Type == TIPO_INTEIRO)
			{
				_ExpSType = TIPO_REAL;
				_cg->LODF("A", _ExpSAdr);
				_cg->LOD("B", _T1Adr);
				_cg->CNV("B", "B");
				_cg->SUBF("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _ExpSAdr);
			}
			else if(_ExpSType == TIPO_REAL && _T1Type == TIPO_REAL)
			{
				_ExpSType = TIPO_REAL;
				_cg->LODF("A", _ExpSAdr);
				_cg->LODF("B", _T1Adr);
				_cg->SUBF("A", "B");
				_ExpSAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _ExpSAdr);
			}
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}//end else
		}//end elseif
		else
		{
			if (_ExpSType == TIPO_LOGICO && _T1Type == TIPO_LOGICO)
			{
				_ExpSType = TIPO_LOGICO;
				_cg->LOD("A", _ExpSAdr);
				_cg->LOD("B", _T1Adr);
				_cg->ADD("A", "B");
				//TODO: Terminar ação 27 de código (Gerar Rótulo faltando)
			}
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}//end else
		//(27) - COD --------------------------------------------------------------------------------------
		//(27) - SEMÂNTICO --------------------------------------------------------------------------------
	}//end while
	_cg->flush();
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::T(Type& _TType, int& _TAdr){
	int _FAdr = 0, _F1Adr = 0;
	Type _FType, _F1Type;
	 
	F(_FType, _FAdr);//(18) - SEMÂNTICO
	_TType = _FType;//(18) - SEMÂNTICO
	_TAdr = _FAdr;//(18) - COD

	while(_CurrentToken._Token == TIMES || _CurrentToken._Token == OVER || _CurrentToken._Token == E){
		Token _TOp;

		if(_CurrentToken._Token == TIMES){
			CT(TIMES);
			_TOp = TIMES;//(19) - SEMÂNTICO
		}else if(_CurrentToken._Token == OVER){
			CT(OVER);
			_TOp = OVER;//(20) - SEMÂNTICO
		}
		else{
			CT(E);
			_TOp = E;//(21) - SEMÂNTICO
		}

		F(_F1Type, _F1Adr);

		//(22) - SEMÂNTICO --------------------------------------------------------------------------------
		//(22) - COD -------------------------------------------------------------------------------------

		if(_TOp == TIMES)
		{
			if(_TType == TIPO_INTEIRO && _F1Type == TIPO_INTEIRO)
			{
				_TType = TIPO_INTEIRO;
				_cg->LOD("A", _TAdr);
				_cg->LOD("B", _F1Adr);
				_cg->MUL("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_INTEIRO);
				_cg->STO("A", _TAdr);
			}
			else if(_TType == TIPO_INTEIRO && _F1Type == TIPO_REAL)
			{
				_TType = TIPO_REAL;
				_cg->LOD("A", _TAdr);
				_cg->CNV("A","A");
				_cg->LODF("B", _F1Adr);
				_cg->MULF("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _TAdr);
			}
			else if(_TType == TIPO_REAL && _F1Type == TIPO_INTEIRO)
			{
				_TType = TIPO_REAL;
				_cg->LODF("A", _TAdr);
				_cg->LOD("B", _F1Adr);
				_cg->CNV("B","B");
				_cg->MULF("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _TAdr);
			}
			else if(_TType == TIPO_REAL && _F1Type == TIPO_REAL)
			{
				_TType = TIPO_REAL;
				_cg->LODF("A", _TAdr);
				_cg->LODF("B", _F1Adr);
				_cg->MULF("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _TAdr);
			}
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}else if(_TOp == OVER){
			if(_TType == TIPO_INTEIRO && _F1Type == TIPO_INTEIRO)
			{
				_TType = TIPO_REAL;
				_cg->LOD("A", _TAdr);
				_cg->CNV("A","A");
				_cg->LOD("B", _F1Adr);
				_cg->CNV("B","B");
				_cg->DIV("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _TAdr);
			}
			else if(_TType == TIPO_INTEIRO && _F1Type == TIPO_REAL)
			{
				_TType = TIPO_REAL;
				_cg->LOD("A", _TAdr);
				_cg->CNV("A","A");
				_cg->LODF("B", _F1Adr);
				_cg->DIV("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _TAdr);
			}
			else if(_TType == TIPO_REAL && _F1Type == TIPO_INTEIRO)
			{
				_TType = TIPO_REAL;
				_cg->LODF("A", _TAdr);
				_cg->LOD("B", _F1Adr);
				_cg->CNV("B","B");
				_cg->DIV("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _TAdr);
			}
			else if(_TType == TIPO_REAL && _F1Type == TIPO_REAL)
			{
				_TType = TIPO_REAL;
				_cg->LODF("A", _TAdr);
				_cg->LODF("B", _F1Adr);
				_cg->DIV("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_REAL);
				_cg->STOF("A", _TAdr);
			}
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}else{
			if (_TType == TIPO_LOGICO && _F1Type == TIPO_LOGICO)
			{
				_TType = TIPO_LOGICO;
				_cg->LOD("A", _TAdr);
				_cg->LOD("B", _F1Adr);
				_cg->MUL("A", "B");
				_TAdr = _memory->ATWNovoTemp(TIPO_INTEIRO);
				_cg->STO("A", _TAdr);
			}
			else{
				void* _Param[1] = {(void*)_PreviousToken._LINE};
				_eManager->callHandlers(this->getGroupID(), INCOMPATILBE_TYPES, _Param);
			}
		}
		//(22) - COD --------------------------------------------------------------------------------------
		//(22) - SEMÂNTICO --------------------------------------------------------------------------------
	}
	_cg->flush();
}
//---------------------------------------------------------------------------------------------------------------------
void ATWSin::F(Type& _FType, int& _FAdr){//Fend = _FAdr
	Class _class[2] = {CLASSE_CONST, CLASSE_VAR};//(9) - SEMÂNTICO
	ATW_BUFF_ELEMENT _tAux = _PreviousToken;//(16) - SEMÂNTICO
		Type _ExpType;
		int _ExpAdr = 0, _F1Adr = 0;
		Type _F1Type;

	switch(_CurrentToken._Token){
	case LPAREN:
		CT(LPAREN);
		
		_memory->ATWResetTemp();
		Exp(_ExpType, _ExpAdr);//(17) - SEMÂNTICO
		_FType = _ExpType;
		_FAdr = _ExpAdr;//(17) - COD

		CT(RPAREN);
		break;
	case NAO:
		CT(NAO);
		
		F(_F1Type, _F1Adr);//(16) - SEMÂNTICO
		_FType = _F1Type;//(16) - SEMÂNTICO
		_Sem->TypeVerify(_tAux, _F1Type, TIPO_LOGICO);//(16) - SEMÂNTICO
		
		//(16) - COD-------------------------------------------------------------------
		_FAdr = _memory->ATWNovoTemp(_FType);

		_cg->LOD("A",_F1Adr);
		_cg->NEG("A");
		_cg->ADI("A", "1");
		_cg->STO("A", _FAdr);
		//(16) - COD-------------------------------------------------------------------
		break;
	case ID:
		CT(ID);

		_Sem->unicidadeNotDeclared(_PreviousToken);//(1) - SEMÂNTICO
		_Sem->nClassVerify(_PreviousToken, 2, _class);//(8) - SEMÂNTICO
		_FType = _Sem->getType(_PreviousToken);//(14) - SEMÂNTICO
		_FAdr = _PreviousToken._End;//(14) - COD

		break;
	case CONSTANT:
		CT(CONSTANT);

		_FType = _PreviousToken._Tipo;//(15) - SEMÂNTICO

		//(15) - COD-------------------------------------------------------------------
		_FAdr = _memory->ATWNovoTemp(_FType);
		
		if (_FType == TIPO_INTEIRO)
			_cg->STI(_PreviousToken._Lex,_FAdr);
		else
			_cg->STIF(_PreviousToken._Lex,_FAdr);
		//(15) - COD-------------------------------------------------------------------
		break;
	default:
		void* _Param[2] = {(void*)_CurrentToken._LINE,(void*) _CurrentToken._Lex};
		_eManager->callHandlers(this->getGroupID(), UNEXPECTED_TOKEN, _Param);
		break;
	}
	_cg->flush();
}