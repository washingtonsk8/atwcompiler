#ifndef GLOBALSTRUCTSITE_H
#define GLOBALSTRUCTSITE_H
#include "CHBuffer.h"
#include "GlobalEnumSite.h"

class ATWSin;
class ATWLex;

/*USED BY COMPILERCOMPONENT*/
typedef struct{
	int _LINE;
	Token _Token;
	Type _Tipo;
	char _Lex[25];
	unsigned int _End;

	void copyStr(const char* _Value){
		memset(_Lex, 0, 25);
		int cpySize = M_MIN(24,strlen(_Value));
		strncpy_s(_Lex, _Value, cpySize);
	}

	void Build(int _LineP, Token _TokenP, const char* _LexP, Type _TipoP = TIPO_VAZIO, unsigned int _Address = 0){
		_LINE = _LineP;
		_Token = _TokenP;
		copyStr(_LexP);
		_Tipo = _TipoP;
		_End = _Address;
	}
}ATW_BUFF_ELEMENT;
/*USED BY COMPILERCOMPONENT*/

/*USED BY ATWLEX N' ATWSin FOR PARALLEL OPERATIONS*/
typedef struct{
	ATWLex* _Lex;
	ATWSin* _Sin;
	CHBuffer<ATW_BUFF_ELEMENT>* _tBuffer;

	void create(CHBuffer<ATW_BUFF_ELEMENT>* _tBufferP, ATWLex* _LexP, ATWSin* _SinP){
		_Lex = _LexP;
		_Sin = _SinP;
		_tBuffer = _tBufferP;
	}
}NORMAL_BUNDLE;
/*USED BY ATWLEX N' ATWSin FOR PARALLEL OPERATIONS*/
#endif