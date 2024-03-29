#ifndef ATWSin_H
#define ATWSin_H
#include "ProgramComponent.h"
#include "ATWLex.h"
#include "IInitializable.h"
#include "GlobalStructSite.h"
#include "CHBuffer.h"
#include "ATWSem.h"
#include "CodeGeneratorModule.h"
//----------------------------------------------------
//TODO::COLOCANDO A��ES SEMANTICAS E GERA��O DE C�DIGO
//----------------------------------------------------

/*
 *NOTAS GERAIS DO ANALISADOR SINT�TICO
 */

//----------------------------------------------------------------------------------------------------
//_Sem->updateIDAddress(_IdLex, _memory->ATWMalloc(TIPO_INTEIRO))-------------------------------------
//----------------------------------------------------------------------------------------------------
//Gera Instru��o j� armazenando endere�o de ID para a primeira posi��o v�lida usando updateIDAddress--
//----------------------------------------------------------------------------------------------------

/*
 *NOTAS GERAIS DO ANALISADOR SINT�TICO
 */

//---------------------------------------------------------------------------------------------------------------------
typedef struct InfoBundle{
	int	_TokenID;
	char _Lexeme[16];
	unsigned int _End;
	Class _Class;
	Type _Type;
	void* _val;

	void copyStr(const char* _Value){
		memset(_Lexeme, 0, 16);
		int cpySize = M_MIN(15,strlen(_Value));
		strncpy_s(_Lexeme, _Value, cpySize);
	}

	void build_element(
		int _TokenIDP,
		const char* _LexemeP,
		Address _EndP = 0x0,
		Class classeP = CLASSE_VAZIO,
		Type tipoP = TIPO_VAZIO){
			_TokenID = _TokenIDP;
			copyStr(_LexemeP);
			_End = _EndP;
			_Class = classeP;
			_Type = tipoP;
	}
}InfoBundle;

class ATWSin : public ProgramComponent, public IInitializable{
public:
	ATWSin(void);
	~ATWSin(void);

	void initialize(int _Argc, void** _Argv);
	void Run(int _Flag, NORMAL_BUNDLE* _nBundleP);

private:
	CodeGeneratorModule* _cg;
	ATWLex*              _LexAnalyzer;
	ATWSem*              _Sem;	

	//FileHelper* _fHelper;
	ATW_BUFF_ELEMENT _CurrentToken, _PreviousToken, _tToken;

	/*PARALELL CONTROLL*/
	int _Parallel;
	CHBuffer<ATW_BUFF_ELEMENT>* _tBuffer;

	void CT(Token _Token);
	void Start();
	void Block();
	void DPontoD();
	void DLuzD();
	void DColorD();
	void DLightD();
	void DFaceD();
	void DObjD();
	void DVarD();
	void DConstD();
	void Command();

	//--PARA EXPRESS�ES--
	void Exp(Type& _ExpType, Address& _ExpAdr);
	void R(Token& _ROp);
	void EXPS(Type& _ExpSType, Address& _ExpSAdr);
	void T(Type& _TType, Address& _TAdr);
	void F(Type& _FType, Address& _FAdr);
};
#endif
