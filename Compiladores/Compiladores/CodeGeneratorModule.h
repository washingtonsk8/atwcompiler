#ifndef CODEGENERATORMODULE_H
#define CODEGENERATORMODULE_H
#include <stdio.h>
#include <stdlib.h>
#include "FileManager.h"
#include "ERRO_DEF.h"
#include "Globals.h"
#include "IInitializable.h"
#include "IDisposable.h"
#include "CodeGeneratorHelper.h"

//---------------------------------------------------------------------------------------------------------------------
#define MAX_WRITING_BUFFER 100
//---------------------------------------------------------------------------------------------------------------------
typedef enum{
	NO_INSTRUCTION, ADD, ADDF, ADI, ADIF, BNG, BNGF, BNN, BNNF, BNP, BNPF,
	BNZ, BNZF, BPS, BPSF, BZR, BZRF, CNV, DIV, ESC, HLT,
	JNP, LDI, LDIF, LGT, LOD, LODF, MVE, MVEF, MUL, MULF,
	NEG, NEGF, RTR, STI, STIF, STO, STOF, SUB, SUBF, TME
}Instruction;
//---------------------------------------------------------------------------------------------------------------------
class CodeGeneratorModule : public ProgramComponent, public IInitializable, public IDisposable
{
public:
	CodeGeneratorModule(void);
	~CodeGeneratorModule(void);

	void initialize(const char* _icFile);
	void initialize(int _Argc, void** _Argv);
	void dispose(int _Argc, void** _Argv);
	void clearWritingBufferFULL();
	void clearWritingBuffer();
	void insertCodeToWrite(char* _Code, int _codeIndex = -1, bool _Overlap = true);
	string getCode(int _codeIndex = -1);
	void flush();
	void STI(int A, int B, char* _Comment = ""){
		int _InstIndexBase = _InstIndex;
		insertCodeToWrite("STI #", _InstIndex++);
		insertCodeToWrite(ATWgetCStr(A), _InstIndex++);
		insertCodeToWrite(", ", _InstIndex++);
		insertCodeToWrite(ATWgetCStr(B), _InstIndex++); 
		insertCodeToWrite("(DS)\n", _InstIndex++);
		insertCodeToWrite(_Comment, _InstIndex++);

		return _InstIndexBase;
	}

private:
	int _currentI;//QUEUE BEHAVIOR
	int _lIIBF;//LAST INSERTED INDEX BEFORE FLUSHING
	int _hIIBF;//HIGH INSERTED INDEX BEFORE FLUSHING
	int _InstIndex;//Índice para utilizar o buffer de instruções
	string _wBuffer[MAX_WRITING_BUFFER];
	FileManager* fManager;
};
#endif
