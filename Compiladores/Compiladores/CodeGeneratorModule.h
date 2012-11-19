#ifndef CODEGENERATORMODULE_H
#define CODEGENERATORMODULE_H
//---------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "FileManager.h"
#include "ERRO_DEF.h"
#include "Globals.h"
#include "IInitializable.h"
#include "IDisposable.h"
#include "ATW_LabelTable.h"
//---------------------------------------------------------------------------------------------------------------------
#define MAX_WRITING_BUFFER 10000
#define WORD_SIZE		   2
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
	void insertCodeToWriteBin(int _Element);
	void insertCodeToWriteMac(char* _Code, int _codeIndex = -1, bool _Overlap = true);
	void fixCode(int _Address, char* _value);
	string getCode(int _codeIndex = -1);
	string transformToBin(int _Number){return "";}
	void flush();
	void flushBin(){}
	//---------------------------------------------------------------------------------------------------------------------
	/*
	* Funções utilizadas para a escrita do código no arquivo
	*/
	int ADD(char* _RegD, char* _RegO, char* _Comment = "");
	int ADDF(char* _RegD, char* _RegO, char* _Comment = "");
	int ADI(char* _RegD, char* _Imed, char* _Comment = "");
	int ADIF(char* _RegD, char* _Imed, char* _Comment = "");
	int BNG(char* _Reg, int _Desl, char* _Comment = "");
	int BNGF(char* _Reg, int _Desl, char* _Comment = "");
	int BNN(char* _Reg, int _Desl, char* _Comment = "");
	int BNNF(char* _Reg, int _Desl, char* _Comment = "");
	int BNP(char* _Reg, int _Desl, char* _Comment = "");
	int BNPF(char* _Reg, int _Desl, char* _Comment = "");
	int BNZ(char* _Reg, int _Desl, char* _Comment = "");
	int BNZF(char* _Reg, int _Desl, char* _Comment = "");
	int BPS(char* _Reg, int _Desl, char* _Comment = "");
	int BPSF(char* _Reg, int _Desl, char* _Comment = "");
	int BZR(char* _Reg, int _Desl, char* _Comment = "");
	int BZRF(char* _Reg, int _Desl, char* _Comment = "");
	int CNV(char* _RegD, char* _RegO, char* _Comment = "");
	int DIV(char* _RegD, char* _RegO, char* _Comment = "");
	int ESC(char* _Reg1, char* _Reg2, char* _Comment = "");
	int HLT(char* _Comment = "");
	int JMP(char* _Label, char* _Comment = "");//TODO:Label necessita conversão para o inteiro correspondente
	int LDI(char* _RegD, char* _Imed, char* _Comment = "");
	int LDIF(char* _RegD, char* _Imed, char* _Comment = "");
	int LGT(char* _Reg, char* _Comment = "");
	int LOD(char* _RegD, int _Desl, char* _Comment = "");
	int LODF(char* _RegD, int _Desl, char* _Comment = "");
	int MVE(char* _RegD, char* _RegO, char* _Comment = "");
	int MVEF(char* _RegD, char* _RegO, char* _Comment = "");
	int MUL(char* _RegD, char* _RegO, char* _Comment = "");
	int MULF(char* _RegD, char* _RegO, char* _Comment = "");
	int NEG(char* _Reg, char* _Comment = "");
	int NEGF(char* _Reg, char* _Comment = "");
	int RTR(char* _Comment = "");
	int STI(char* _Imed, int _Desl, char* _Comment = "");
	int STIF(char* _Imed, int _Desl, char* _Comment = "");
	int STO(char* _Reg, int _Desl, char* _Comment = "");
	int STOF(char* _Reg, int _Desl, char* _Comment = "");
	int SUB(char* _RegD, char* _RegO, char* _Comment = "");
	int SUBF(char* _RegD, char* _RegO, char* _Comment = "");
	int TME(char* _Reg, char* _Comment = "");
	//---------------------------------------------------------------------------------------------------------------------
private:
	int _currentI;//QUEUE BEHAVIOR
	int _lIIBF;//LAST INSERTED INDEX BEFORE FLUSHING
	int _hIIBF;//HIGH INSERTED INDEX BEFORE FLUSHING
	int _InstIndex;//Índice para utilizar o buffer de instruções
	int _Value;
	string _wBuffer[MAX_WRITING_BUFFER];
	string _bBuffer[WORD_SIZE];
	FileManager* fManager;
};
#endif

