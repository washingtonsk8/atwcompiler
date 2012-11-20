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
void CodeGeneratorModule::insertCodeToWriteBin(int _Element, char* _Number){
	if(_Number == NULL){
		bool _isFloat = false;
		while((*_Number) != '\0'){
			if((*_Number) == '.')
				_isFloat = true;
			_Number++;
		}//end while
		if(_isFloat){
			char* _Temp = _Number;
			string _ValueDecimal = "";
			string _ValueInteger = "";
			_ValueDecimal = strtok (_Temp,".");
			while (_Temp != NULL)
			{
				_ValueInteger = strtok (NULL, ".");
			}
			_ValueInteger = ATWgetCStrBin(ATWgetInt(_ValueInteger));
			_ValueDecimal = ATWgetCStrBin(ATWgetInt(_ValueDecimal));
			_ValueInteger = binaryVerify(_ValueInteger);
			_ValueDecimal = binaryVerify(_ValueDecimal);
			_bBuffer[0] = _ValueInteger;
			_bBuffer[1] = _ValueDecimal;
		}//end if
		else{
			string _Value = _Number;
			_Value = ATWgetCStrBin(ATWgetInt(_Value));
			_Value = binaryVerify(_Value);
			_bBuffer[0] = _Value;
		}//end else
	}
	else{
		string _Value = ATWgetCStrBin(ATWgetInt(_Number));
		_Value = binaryVerify(_Value);
		_bBuffer[0] = _Value;
	}
} 
//---------------------------------------------------------------------------------------------------------------------
string CodeGeneratorModule::binaryVerify(string _Number){
	string _Temp = "0";
	while(_Number.size() <= 8)
		_Temp.append(_Number);
	return _Temp;
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::insertCodeToWriteAsm(char* _Code, int _codeIndex, bool _Overlap){
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
/*void CodeGeneratorModule::flushBin(){
//	for(int i = 0; i <= _hIIBF; i++){
		if(_wBuffer[i] != ""){
			fManager->writeInFile(_wBuffer[i].c_str());
			_wBuffer[i] = "";
		}
	}
//	
//}*/
//----------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::write(char* _String)
{
	insertCodeToWriteAsm(_String, _InstIndex++);
}
//----------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::writeRot(char* _String)
{
	strcat(_String, ":");
	insertCodeToWriteAsm(_String, _InstIndex++);
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADD(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("ADD ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	
	//binary
	//insertCodeToWriteBin(1);
	//insertCodeToWriteBin(-23,_RegD);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);	
	flushBin(); 

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADDF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;	
	insertCodeToWriteAsm("ADDF ", _InstIndex++);
	//insertCodeToWriteBin(2);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADI(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("ADI ", _InstIndex++);
	//insertCodeToWriteBin(3);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	//itoa(_Value,_Imed,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADIF(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("ADIF ", _InstIndex++);
	//insertCodeToWriteBin(4);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	//itoa(_Value,_Imed,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNG(char* _Reg, char* _Rot, char* _Comment){
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNG ", _InstIndex++);
	//insertCodeToWriteBin(5);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNGF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNGF ", _InstIndex++);
	//insertCodeToWriteBin(6);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNN(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNN ", _InstIndex++);
	//insertCodeToWriteBin(7);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNNF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNNF ", _InstIndex++);
	//insertCodeToWriteBin(8);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNP(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNP ", _InstIndex++);
	//insertCodeToWriteBin(9);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNPF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNPF ", _InstIndex++);
	//insertCodeToWriteBin(10);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNZ(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNZ ", _InstIndex++);
	//insertCodeToWriteBin(11);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNZF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNZF ", _InstIndex++);
	//insertCodeToWriteBin(12);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BPS(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BPS ", _InstIndex++);
	//insertCodeToWriteBin(13);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BPSF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BPSF", _InstIndex++);
	//insertCodeToWriteBin(14);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BZR(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BZR ", _InstIndex++);
	//insertCodeToWriteBin(15);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BZRF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BZRF ", _InstIndex++);
	//insertCodeToWriteBin(16);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::CNV(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("CNV ", _InstIndex++);
	//insertCodeToWriteBin(17);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::DIV(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("DIV ", _InstIndex++);
	//insertCodeToWriteBin(18);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ESC(char* _Reg1, char* _Reg2, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("ESC ", _InstIndex++);
	//insertCodeToWriteBin(19);
	insertCodeToWriteAsm(_Reg1, _InstIndex++);
	//itoa(_Value,_Reg1,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Reg2, _InstIndex++);
	//itoa(_Value,_Reg2,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::HLT(char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("HLT", _InstIndex++);
	//insertCodeToWriteBin(20);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 1 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::JMP(char* _Label, char* _Comment)//TODO:Label necessita conversão para o inteiro correspondente
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("JMP ", _InstIndex++);
	//insertCodeToWriteBin(21);
	insertCodeToWriteAsm(_Label, _InstIndex++);
	//itoa(_Value,_Label,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 3 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LDI(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LDI ", _InstIndex++);
	//insertCodeToWriteBin(22);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	//itoa(_Value,_Imed,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LDIF(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LDIF ", _InstIndex++);
	//insertCodeToWriteBin(23);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	//itoa(_Value,_Imed,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LGT(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LGT ", _InstIndex++);
	//insertCodeToWriteBin(24);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LOD(char* _RegD, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LOD ", _InstIndex++);
	//insertCodeToWriteBin(25);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LODF(char* _RegD, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LODF ", _InstIndex++);
	//insertCodeToWriteBin(26);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MVE(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MVE ", _InstIndex++);
	//insertCodeToWriteBin(27);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MVEF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MVEF ", _InstIndex++);
	//insertCodeToWriteBin(28);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MUL(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MUL ", _InstIndex++);
	//insertCodeToWriteBin(29);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MULF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MULF ", _InstIndex++);
	//insertCodeToWriteBin(30);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::NEG(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("NEG ", _InstIndex++);
	//insertCodeToWriteBin(31);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::NEGF(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("NEGF ", _InstIndex++);
	//insertCodeToWriteBin(32);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::RTR(char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("RTR", _InstIndex++);
	//insertCodeToWriteBin(33);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize =

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STI(char* _Imed, int _Desl, char* _Comment){
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("STI #", _InstIndex++);
	//insertCodeToWriteBin(34);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	//itoa(_Value,_Imed,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++); 
	//itoa(_Value,ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STIF(char* _Imed, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("STIF #", _InstIndex++);
	//insertCodeToWriteBin(35);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	//itoa(_Value,_Imed,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++); 
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}

	insertCodeToWriteAsm("\n", _InstIndex++);
	
	//BlockSize = 5 + 1



	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STO(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("STO ", _InstIndex++);
	//insertCodeToWriteBin(36);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STOF(char* _Reg, int _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("STOF ", _InstIndex++);
	//insertCodeToWriteBin(37);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	//itoa(_Value, ATWgetCStr(_Desl),10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::SUB(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("SUB ", _InstIndex++);
	//insertCodeToWriteBin(38);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::SUBF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("SUBF ", _InstIndex++);
	//insertCodeToWriteBin(39);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	//itoa(_Value,_RegD,10);
	//insertCodeToWriteBin(_Value);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	//itoa(_Value,_RegO,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::TME(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("TME ", _InstIndex++);
	//insertCodeToWriteBin(40);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	//itoa(_Value,_Reg,10);
	//insertCodeToWriteBin(_Value);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------