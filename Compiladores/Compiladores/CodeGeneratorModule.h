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
#include "ATWLabelManager.h"
#include "ATW_MemoryManager.h"
//---------------------------------------------------------------------------------------------------------------------
#include <fstream>
//---------------------------------------------------------------------------------------------------------------------
#define MAX_WRITING_BUFFER 10000
#define WORD_SIZE		   2
//---------------------------------------------------------------------------------------------------------------------
using namespace LabelController;
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
	void fixCode(Address _AddressAsm, Address _AddressBin, char* _value);
	void fixCodeOnlyBin(Address _AddressBin, Address _value);
	string getCode(int _codeIndex = -1);
	void writeS16(short value);
	void flush();
	void flushBin();
	//---------------------------------------------------------------------------------------------------------------------
	/*
	* Funções utilizadas para a escrita do código no arquivo
	*/
	void write(char* _String);
	void writeRot(char* _String, Address _FixCode = -1);
	Address* ADD(char* _RegD, char* _RegO, char* _Comment = "");
	Address* ADDF(char* _RegD, char* _RegO, char* _Comment = "");
	Address* ADI(char* _RegD, char* _Imed, char* _Comment = "");
	Address* ADIF(char* _RegD, char* _Imed, char* _Comment = "");
	Address* BNG(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BNGF(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BNN(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BNNF(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BNP(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BNPF(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BNZ(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BNZF(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BPS(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BPSF(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BZR(char* _Reg, char* _Rot, char* _Comment = "");
	Address* BZRF(char* _Reg, char* _Rot, char* _Comment = "");
	Address* CNV(char* _RegD, char* _RegO, char* _Comment = "");
	Address* DIV(char* _RegD, char* _RegO, char* _Comment = "");
	Address* ESC(char* _Reg1, char* _Reg2, char* _Comment = "");
	Address* HLT(char* _Comment = "");
	Address* JMP(char* _Label, char* _Comment = "");//TODO:Label necessita conversão para o Address*eiro correspondente
	Address* LDI(char* _RegD, char* _Imed, char* _Comment = "");
	Address* LDIF(char* _RegD, char* _Imed, char* _Comment = "");
	Address* LGT(char* _Reg, char* _Comment = "");
	Address* LOD(char* _RegD, Address _Desl, char* _Comment = "");
	Address* LODF(char* _RegD, Address _Desl, char* _Comment = "");
	Address* MVE(char* _RegD, char* _RegO, char* _Comment = "");
	Address* MVEF(char* _RegD, char* _RegO, char* _Comment = "");
	Address* MUL(char* _RegD, char* _RegO, char* _Comment = "");
	Address* MULF(char* _RegD, char* _RegO, char* _Comment = "");
	Address* NEG(char* _Reg, char* _Comment = "");
	Address* NEGF(char* _Reg, char* _Comment = "");
	Address* RTR(char* _Comment = "");
	Address* STI(char* _Imed, Address _Desl, char* _Comment = "");
	Address* STIF(char* _Imed, Address _Desl, char* _Comment = "");
	Address* STO(char* _Reg, Address _Desl, char* _Comment = "");
	Address* STOF(char* _Reg, Address _Desl, char* _Comment = "");
	Address* SUB(char* _RegD, char* _RegO, char* _Comment = "");
	Address* SUBF(char* _RegD, char* _RegO, char* _Comment = "");
	Address* TME(char* _Reg, char* _Comment = "");
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
	int _BinIndex;//Índice para utilizar o buffer de binários
	string      _wBuffer[MAX_WRITING_BUFFER];
	string      _bBuffer[WORD_SIZE];
	list        <bitset<16>*> _memoryPositions;
	FileManager* fManager;
	ofstream     binaryFile;
	ATWMemory*	_Memory;
	
	//FileManager* binaryFile;
	
};
#endif

