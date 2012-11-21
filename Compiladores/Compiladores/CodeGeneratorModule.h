#ifndef CODEGENERATORMODULE_H
#define CODEGENERATORMODULE_H
//---------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <list>
#include <bitset>
#include <iterator>
//---------------------------------------------------------------------------------------------------------------------
#include "FileManager.h"
#include "ERRO_DEF.h"
#include "Globals.h"
#include "IInitializable.h"
#include "IDisposable.h"
#include "ATW_LabelTable.h"
//---------------------------------------------------------------------------------------------------------------------
#include <fstream>
//---------------------------------------------------------------------------------------------------------------------
#define MAX_WRITING_BUFFER 10000
#define WORD_SIZE		   2
//---------------------------------------------------------------------------------------------------------------------
namespace BinaryGen{
	typedef enum{
		NO_INSTRUCTION, ADD, ADDF, ADI, ADIF, BNG, BNGF, BNN, BNNF, BNP, BNPF,
		BNZ, BNZF, BPS, BPSF, BZR, BZRF, CNV, DIV, ESC, HLT,
		JMP, LDI, LDIF, LGT, LOD, LODF, MVE, MVEF, MUL, MULF,
		NEG, NEGF, RTR, STI, STIF, STO, STOF, SUB, SUBF, TME
	}Instruction;

	typedef enum{
		NO_REGISTER, A, B, C, D, E, F
	}Register;
}
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
	void insertCodeToWriteBin(int _Element, char* _Number =	NULL);
	void insertCodeToWriteAsm(char* _Code, int _codeIndex = -1, bool _Overlap = true);
	void fixCode(int _Address, char* _value);
	string getCode(int _codeIndex = -1);
	string binaryVerify(string _Number);
	void flush();
	void flushBin();
	//---------------------------------------------------------------------------------------------------------------------
	/*
	* Funções utilizadas para a escrita do código no arquivo
	*/
	void write(char* _String);
	void writeRot(char* _String);
	int ADD(char* _RegD, char* _RegO, char* _Comment = "");
	int ADDF(char* _RegD, char* _RegO, char* _Comment = "");
	int ADI(char* _RegD, char* _Imed, char* _Comment = "");
	int ADIF(char* _RegD, char* _Imed, char* _Comment = "");
	int BNG(char* _Reg, char* _Rot, char* _Comment = "");
	int BNGF(char* _Reg, char* _Rot, char* _Comment = "");
	int BNN(char* _Reg, char* _Rot, char* _Comment = "");
	int BNNF(char* _Reg, char* _Rot, char* _Comment = "");
	int BNP(char* _Reg, char* _Rot, char* _Comment = "");
	int BNPF(char* _Reg, char* _Rot, char* _Comment = "");
	int BNZ(char* _Reg, char* _Rot, char* _Comment = "");
	int BNZF(char* _Reg, char* _Rot, char* _Comment = "");
	int BPS(char* _Reg, char* _Rot, char* _Comment = "");
	int BPSF(char* _Reg, char* _Rot, char* _Comment = "");
	int BZR(char* _Reg, char* _Rot, char* _Comment = "");
	int BZRF(char* _Reg, char* _Rot, char* _Comment = "");
	int CNV(char* _RegD, char* _RegO, char* _Comment = "");
	int DIV(char* _RegD, char* _RegO, char* _Comment = "");
	int ESC(char* _Reg1, char* _Reg2, char* _Comment = "");
	int HLT(char* _Comment = "");
	int JMP(char* _Label, char* _Comment = "");//TODO:Label necessita conversão para o inteiro correspondente
	int LDI(char* _RegD, char* _Imed, char* _Comment = "");
	int LDIF(char* _RegD, char* _Imed, char* _Comment = "");
	int LGT(char* _Reg, char* _Comment = "");
	int LOD(char* _RegD, Address _Desl, char* _Comment = "");
	int LODF(char* _RegD, Address _Desl, char* _Comment = "");
	int MVE(char* _RegD, char* _RegO, char* _Comment = "");
	int MVEF(char* _RegD, char* _RegO, char* _Comment = "");
	int MUL(char* _RegD, char* _RegO, char* _Comment = "");
	int MULF(char* _RegD, char* _RegO, char* _Comment = "");
	int NEG(char* _Reg, char* _Comment = "");
	int NEGF(char* _Reg, char* _Comment = "");
	int RTR(char* _Comment = "");
	int STI(char* _Imed, Address  _Desl, char* _Comment = "");
	int STIF(char* _Imed, Address  _Desl, char* _Comment = "");
	int STO(char* _Reg, Address  _Desl, char* _Comment = "");
	int STOF(char* _Reg, Address  _Desl, char* _Comment = "");
	int SUB(char* _RegD, char* _RegO, char* _Comment = "");
	int SUBF(char* _RegD, char* _RegO, char* _Comment = "");
	int TME(char* _Reg, char* _Comment = "");
	//---------------------------------------------------------------------------------------------------------------------
	void clearBitSetBuffer(){
		list <bitset<16>*>::iterator it = _memoryPositions.begin();
		for(; it != _memoryPositions.end(); it++){
			if((*it) != 0x0){
				delete (*it);
				(*it) = 0x0;
			}
		}

		_memoryPositions.clear();
	}
	//---------------------------------------------------------------------------------------------------------------------
	void insertIntoBitSetBuffer(bitset<16>* _element = 0x0){
		if(_element == 0x0)
			return;

		//printf("DEBUG: insert into bitset vector\n");

		_memoryPositions.push_back(_element);
	}
	//---------------------------------------------------------------------------------------------------------------------
private:
	int _currentI;//QUEUE BEHAVIOR
	int _lIIBF;//LAST INSERTED INDEX BEFORE FLUSHING
	int _hIIBF;//HIGH INSERTED INDEX BEFORE FLUSHING
	int _InstIndex;//Índice para utilizar o buffer de instruções
	string      _wBuffer[MAX_WRITING_BUFFER];
	string      _bBuffer[WORD_SIZE];
	list        <bitset<16>*> _memoryPositions;
	FileManager* fManager;
	ofstream     binaryFile;
	//FileManager* binaryFile;
	
};
#endif

