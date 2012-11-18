#include "CodeGeneratorModule.h"
#include "ATW_Helper.h"
//---------------------------------------------------------------------------------------------------------------------
using namespace ATW_HELPER;
//---------------------------------------------------------------------------------------------------------------------
CodeGeneratorModule::CodeGeneratorModule(void){}
//---------------------------------------------------------------------------------------------------------------------
CodeGeneratorModule::~CodeGeneratorModule(void){}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::fixCode(int _Address, char* _value){
	_wBuffer[_Address] = _value;
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::initialize(const char* _icFile){
	this->setGroupID(COMPILER_GROUP);
	clearWritingBuffer();
	_lIIBF = 0;
	_hIIBF = 0;
	_currentI = 0;

	fManager = new FileManager();
	fManager->initialize(0, NULL);
	fManager->openFile(_icFile, GENERIC_WRITE, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::initialize(int _Argc, void** _Argv){
	if(_Argc < 1)
		_eManager->callHandlers(this->getGroupID(), INSUFFICIENT_ARGUMENTS, NULL);

	this->setGroupID(COMPILER_GROUP);
	clearWritingBufferFULL();
	_lIIBF = 0;
	_hIIBF = 0;
	_currentI = 0;
	_InstIndex = 0;

	fManager = new FileManager();
	fManager->initialize(0, NULL);
	fManager->openFile((char*)_Argv[0], GENERIC_WRITE, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::dispose(int _Argc, void** _Argv){
	fManager->dispose(0, NULL);
	delete fManager;
	fManager = 0x0;
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::clearWritingBufferFULL(){
	for(int i = 0; i < MAX_WRITING_BUFFER; i++)
		_wBuffer[i] = "";
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::clearWritingBuffer(){
	for(int i = 0; i < _hIIBF; i++)
		_wBuffer[i] = "";
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::insertCodeToWriteBin(int _Element){
	if(_Element < 255){
		_bBuffer[0] = "00000000";
		_bBuffer[1] = transformToBin(_Element);
	}
	else{
		_bBuffer[0] = transformToBin(_Element/255);
		_bBuffer[1] = transformToBin(_Element%255);
	}
	flushBin(); 
} 
//---------------------------------------------------------------------------------------------------------------------
//string transformToBin(int _Number){
//	char* buffer[255];
//	itoa (_Number,buffer,2);
//	return buffer;
//}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::insertCodeToWriteMac(char* _Code, int _codeIndex, bool _Overlap){
	if(_codeIndex > MAX_WRITING_BUFFER || _codeIndex < 0)
		_eManager->callHandlers(this->getGroupID(), FATAL_ERROR, NULL);

	if(_codeIndex == -1){
		_codeIndex = _currentI;

		if(_currentI < MAX_WRITING_BUFFER)
			_currentI ++;
		else
			return;
	}

	if(_wBuffer[_codeIndex] == ""){
		_wBuffer[_codeIndex] += _Code;
	}else if(_Overlap){
		_wBuffer[_codeIndex] = _Code;
	}else
		return;

	if(_codeIndex > _hIIBF)
		_hIIBF = _codeIndex;
	_lIIBF = _codeIndex;
}
//---------------------------------------------------------------------------------------------------------------------
string CodeGeneratorModule::getCode(int _codeIndex){
	if(_codeIndex > MAX_WRITING_BUFFER || _codeIndex < 0)
		_eManager->callHandlers(this->getGroupID(), FATAL_ERROR, NULL);

	if(_codeIndex == -1)
		_codeIndex = _lIIBF;

	return _wBuffer[_codeIndex];
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::flush(){
	for(int i = 0; i <= _hIIBF; i++){
		if(_wBuffer[i] != ""){
			fManager->writeInFile(_wBuffer[i].c_str());
			_wBuffer[i] = "";
		}
	}

	_currentI = 0;
	_lIIBF = 0;
	_hIIBF = 0;
}
//---------------------------------------------------------------------------------------------------------------------
//void CodeGeneratorModule::flushBin(){
//	char* temp = _bBuffer[0];
//	fManager->writeInFile(temp);
//	_bBuffer[0] = (char)0;
//	temp = _bBuffer[1];
//	fManager->writeInFile(temp);
//	_bBuffer[1] = 0;
//	
//}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADD(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("ADD #", _InstIndex++);
	insertCodeToWriteBin(1);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADDF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;	
	insertCodeToWriteMac("ADDF ", _InstIndex++);
	insertCodeToWriteBin(2);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADI(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("ADI ", _InstIndex++);
	insertCodeToWriteBin(3);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", #", _InstIndex++);
	insertCodeToWriteMac(_Imed, _InstIndex++);
	itoa(_Value,_Imed,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADIF(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("ADIF ", _InstIndex++);
	insertCodeToWriteBin(4);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", #", _InstIndex++);
	insertCodeToWriteMac(_Imed, _InstIndex++);
	itoa(_Value,_Imed,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNG(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNG ", _InstIndex++);
	insertCodeToWriteBin(5);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNGF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNGF ", _InstIndex++);
	insertCodeToWriteBin(6);
	insertCodeToWriteMac(_Reg, _InstIndex++);	
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNN(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNN ", _InstIndex++);
	insertCodeToWriteBin(7);
	insertCodeToWriteMac(_Reg, _InstIndex++);	
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNNF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNNF ", _InstIndex++);
	insertCodeToWriteBin(8);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNP(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNP ", _InstIndex++);
	insertCodeToWriteBin(9);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNPF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNPF ", _InstIndex++);
	insertCodeToWriteBin(10);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNZ(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNZ ", _InstIndex++);
	insertCodeToWriteBin(11);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNZF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BNZF ", _InstIndex++);
	insertCodeToWriteBin(12);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BPS(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BPS ", _InstIndex++);
	insertCodeToWriteBin(13);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BPSF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BPSF", _InstIndex++);
	insertCodeToWriteBin(14);
	insertCodeToWriteMac(_Reg, _InstIndex++);	
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BZR(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BZR ", _InstIndex++);
	insertCodeToWriteBin(15);
	insertCodeToWriteMac(_Reg, _InstIndex++);	
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BZRF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("BZRF ", _InstIndex++);
	insertCodeToWriteBin(16);
	insertCodeToWriteMac(_Reg, _InstIndex++);	
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(CS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::CNV(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("CNV ", _InstIndex++);
	insertCodeToWriteBin(17);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::DIV(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("DIV ", _InstIndex++);
	insertCodeToWriteBin(18);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ESC(char* _Reg1, char* _Reg2, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("ESC ", _InstIndex++);
	insertCodeToWriteBin(19);
	insertCodeToWriteMac(_Reg1, _InstIndex++);
	itoa(_Value,_Reg1,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_Reg2, _InstIndex++);
	itoa(_Value,_Reg2,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::HLT(char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("HLT", _InstIndex++);
	insertCodeToWriteBin(20);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 1 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::JMP(char* _Label, char* _Comment)//TODO:Label necessita conversão para o inteiro correspondente
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("JMP ", _InstIndex++);
	insertCodeToWriteBin(21);
	insertCodeToWriteMac(_Label, _InstIndex++);
	itoa(_Value,_Label,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 3 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LDI(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("LDI ", _InstIndex++);
	insertCodeToWriteBin(22);
	insertCodeToWriteMac(_RegD, _InstIndex++);	
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", #", _InstIndex++);
	insertCodeToWriteMac(_Imed, _InstIndex++);
	itoa(_Value,_Imed,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LDIF(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("LDIF ", _InstIndex++);
	insertCodeToWriteBin(23);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", #", _InstIndex++);
	insertCodeToWriteMac(_Imed, _InstIndex++);
	itoa(_Value,_Imed,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LGT(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("LGT ", _InstIndex++);
	insertCodeToWriteBin(24);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LOD(char* _RegD, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("LOD ", _InstIndex++);
	insertCodeToWriteBin(25);
	insertCodeToWriteMac(_RegD, _InstIndex++);	
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LODF(char* _RegD, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("LODF ", _InstIndex++);
	insertCodeToWriteBin(26);
	insertCodeToWriteMac(_RegD, _InstIndex++);	
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MVE(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("MVE ", _InstIndex++);
	insertCodeToWriteBin(27);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MVEF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("MVEF ", _InstIndex++);
	insertCodeToWriteBin(28);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MUL(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("MUL ", _InstIndex++);
	insertCodeToWriteBin(29);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MULF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("MULF ", _InstIndex++);
	insertCodeToWriteBin(30);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::NEG(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("NEG ", _InstIndex++);
	insertCodeToWriteBin(31);
	insertCodeToWriteMac(_Reg, _InstIndex++);	
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::NEGF(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("NEGF ", _InstIndex++);
	insertCodeToWriteBin(32);
	insertCodeToWriteMac(_Reg, _InstIndex++);	
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::RTR(char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("RTR", _InstIndex++);
	insertCodeToWriteBin(33);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize =

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STI(char* _Imed, int _Desl, char* _Comment){
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("STI #", _InstIndex++);
	insertCodeToWriteBin(34);
	insertCodeToWriteMac(_Imed, _InstIndex++);
	itoa(_Value,_Imed,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++); 
	itoa(_Value,ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STIF(char* _Imed, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("STIF #", _InstIndex++);
	insertCodeToWriteBin(35);
	insertCodeToWriteMac(_Imed, _InstIndex++);
	itoa(_Value,_Imed,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++); 
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}

	insertCodeToWriteMac("\n", _InstIndex++);
	
	//BlockSize = 5 + 1



	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STO(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("STO ", _InstIndex++);
	insertCodeToWriteBin(36);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STOF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("STOF ", _InstIndex++);
	insertCodeToWriteBin(37);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(ATWgetCStr(_Desl), _InstIndex++);
	itoa(_Value, ATWgetCStr(_Desl),10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::SUB(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("SUB ", _InstIndex++);
	insertCodeToWriteBin(38);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::SUBF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("SUBF ", _InstIndex++);
	insertCodeToWriteBin(39);
	insertCodeToWriteMac(_RegD, _InstIndex++);
	itoa(_Value,_RegD,10);
	insertCodeToWriteBin(_Value);
	insertCodeToWriteMac(", ", _InstIndex++);
	insertCodeToWriteMac(_RegO, _InstIndex++);
	itoa(_Value,_RegO,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::TME(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteMac("TME ", _InstIndex++);
	insertCodeToWriteBin(40);
	insertCodeToWriteMac(_Reg, _InstIndex++);
	itoa(_Value,_Reg,10);
	insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteMac(_sPrint, _InstIndex++);}
	insertCodeToWriteMac("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------