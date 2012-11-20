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
	if(_Number != NULL){//Se é para gravar imediato
		string _ValueDecimal = "", _ValueInteger = "";
		bool _isFloat = false;

		//Copiar o valor do Number para dois temporarios se necessário
		while((*_Number) != '\0'){
			if((*_Number) == '.'){
				_isFloat = true;
				_Number ++;
			}

			//Concatenando a parte inteira e real
			if(!_isFloat)
				_ValueInteger += *_Number;
			else
				_ValueDecimal += *_Number;

			_Number++;
		}//end while
		if(_isFloat){
			_ValueInteger = ATWgetCStrBin(ATWgetInt(_ValueInteger));
			_ValueDecimal = ATWgetCStrBin(ATWgetInt(_ValueDecimal));
			_ValueInteger = binaryVerify(_ValueInteger);
			_ValueDecimal = binaryVerify(_ValueDecimal);
			_bBuffer[0] = _ValueInteger;
			_bBuffer[1] = _ValueDecimal;
			//cout << "Valor gravado inteiro: " << _ValueInteger << endl;			
			//cout << "Valor gravado decimal: " << _ValueDecimal << endl;
		}//end if
		else{
			_ValueInteger = ATWgetCStrBin(ATWgetInt(_ValueInteger));
			_ValueInteger = binaryVerify(_ValueInteger);
			_bBuffer[0] = _ValueInteger;
			//cout << "Valor gravado: " << _ValueInteger << endl;
		}//end else
	}
	else{
		string _Value = ATWgetCStrBin(_Element);
		_Value = binaryVerify(_Value);
		_bBuffer[0] = _Value;
		//cout << "Valor gravado: " << _Value << endl;
	}
} 
//---------------------------------------------------------------------------------------------------------------------
string CodeGeneratorModule::binaryVerify(string _Number){
	string _Temp = "0";
	int _Quantity = _Number.size(), i;
	//Completando o número para conter 16 bits completos
	for(i = _Quantity; i < 15; i++)
		_Temp.append("0");
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
void CodeGeneratorModule::flushBin(){
	for(int i = 0; i <= _hIIBF; i++){
		if(_wBuffer[i] != ""){
			fManager->writeInFile(_wBuffer[i].c_str());
			_wBuffer[i] = "";
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::write(char* _String)
{
	insertCodeToWriteAsm(_String, _InstIndex++);
}
//----------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::writeRot(char* _String)
{
	char _Rot[255] = "\n";
	strcat(_Rot, _String);
	strcat(_Rot, ":\n");
	insertCodeToWriteAsm(_Rot, _InstIndex++);
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
	insertCodeToWriteBin(1);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);
	flushBin(); 

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADDF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;	
	insertCodeToWriteAsm("ADDF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(2);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADI(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("ADI ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(3);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_Imed);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ADIF(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("ADIF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);

	if(strcmp(_Comment, "") != 0){if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(4);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_Imed);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNG(char* _Reg, char* _Rot, char* _Comment){
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNG ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1	
	insertCodeToWriteBin(5);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNGF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNGF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(6);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNN(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNN ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(7);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNNF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNNF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(8);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNP(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNP ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(9);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNPF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNPF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(10);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNZ(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNZ ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(11);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BNZF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BNZF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(12);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BPS(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BPS ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(13);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BPSF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BPSF", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(14);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BZR(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BZR ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(15);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::BZRF(char* _Reg, char* _Rot, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("BZRF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(16);
	insertCodeToWriteBin(-23,_Reg);
	insertCodeToWriteBin(-23,_Rot);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::CNV(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("CNV ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(17);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::DIV(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("DIV ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(18);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::ESC(char* _Reg1, char* _Reg2, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("ESC ", _InstIndex++);
	insertCodeToWriteAsm(_Reg1, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Reg2, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(19);
	insertCodeToWriteBin(-23,_Reg1);
	insertCodeToWriteBin(-23,_Reg2);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::HLT(char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("HLT", _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 1 + 1
	insertCodeToWriteBin(20);

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
	insertCodeToWriteBin(21);
	insertCodeToWriteBin(-23,_Label);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LDI(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LDI ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(22);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_Imed);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LDIF(char* _RegD, char* _Imed, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LDIF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1	
	insertCodeToWriteBin(23);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_Imed);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LGT(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LGT ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1
	insertCodeToWriteBin(24);
	insertCodeToWriteBin(-23,_Reg);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LOD(char* _RegD, Address _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LOD ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);

	insertCodeToWriteAsm("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(25);
	insertCodeToWriteBin(-23,_RegD);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::LODF(char* _RegD, Address _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("LODF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(26);
	insertCodeToWriteBin(-23,_RegD);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MVE(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MVE ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(27);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MVEF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MVEF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1	
	insertCodeToWriteBin(28);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MUL(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MUL ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1	
	insertCodeToWriteBin(29);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::MULF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("MULF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(30);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::NEG(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("NEG ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1	
	insertCodeToWriteBin(31);
	insertCodeToWriteBin(-23,_Reg);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::NEGF(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("NEGF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1	
	insertCodeToWriteBin(32);
	insertCodeToWriteBin(-23,_Reg);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::RTR(char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("RTR", _InstIndex++);
	
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize =
	insertCodeToWriteBin(33);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STI(char* _Imed, Address _Desl, char* _Comment){
	int _InstIndexBase = _InstIndex;

	insertCodeToWriteAsm("STI #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(34);
	insertCodeToWriteBin(-23,_Imed);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STIF(char* _Imed, Address _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("STIF #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);
	
	//BlockSize = 5 + 1
	insertCodeToWriteBin(35);
	insertCodeToWriteBin(-23,_Imed);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STO(char* _Reg, Address _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("STO ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);

	insertCodeToWriteAsm("(DS)", _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(36);
	insertCodeToWriteBin(-23,_Reg);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::STOF(char* _Reg, Address _Desl, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("STOF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(37);
	insertCodeToWriteBin(-23,_Reg);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::SUB(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("SUB ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(38);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::SUBF(char* _RegD, char* _RegO, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("SUBF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(39);
	insertCodeToWriteBin(-23,_RegD);
	insertCodeToWriteBin(-23,_RegO);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
int CodeGeneratorModule::TME(char* _Reg, char* _Comment)
{
	int _InstIndexBase = _InstIndex;
	insertCodeToWriteAsm("TME ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	if(strcmp(_Comment, "") != 0){char _sPrint[255];sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);insertCodeToWriteAsm(_sPrint, _InstIndex++);}
	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1
	insertCodeToWriteBin(40);
	insertCodeToWriteBin(-23,_Reg);

	return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------