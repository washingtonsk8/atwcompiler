#ifndef GLOBALSTRUCTSITE_H
#define GLOBALSTRUCTSITE_H
#include "CHBuffer.h"
#include "globaEnumSite.h"

class ATWSim;
class ATWLex;

/*USED BY COMPILERCOMPONENT*/
typedef struct{
	int _LINE;
	Token _Token;
	Type _Tipo;
	int _valI;
	float _valF;
	char _Lex[25];

	void copyStr(const char* _Value){
		memset(_Lex, 0, 25);
		int cpySize = M_MIN(24,strlen(_Value));
		strncpy_s(_Lex, _Value, cpySize);
	}

	void Build(int _LineP, Token _TokenP, const char* _LexP, Type _TipoP = NULO, int _valIP = 0, float _valFP = 0.0f){
		_LINE = _LineP;
		_Token = _TokenP;
		copyStr(_LexP);
		_Tipo = _TipoP;
		_valI = _valIP;
		_valF = _valFP;
	}
}ATW_BUFF_ELEMENT;
/*USED BY COMPILERCOMPONENT*/

/*USED BY ATWLEX N' ATWSIM FOR PARALLEL OPERATIONS*/
typedef struct{
	ATWLex* _Lex;
	ATWSim* _Sin;
	CHBuffer<ATW_BUFF_ELEMENT>* _tBuffer;

	void create(CHBuffer<ATW_BUFF_ELEMENT>* _tBufferP, ATWLex* _LexP, ATWSim* _SinP){
		_Lex = _LexP;
		_Sin = _SinP;
		_tBuffer = _tBufferP;
	}
}NORMAL_BUNDLE;
/*USED BY ATWLEX N' ATWSIM FOR PARALLEL OPERATIONS*/
#endif