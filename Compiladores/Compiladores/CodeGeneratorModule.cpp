#include "CodeGeneratorModule.h"
//---------------------------------------------------------------------------------------------------------------------
CodeGeneratorModule::CodeGeneratorModule(void){}
//---------------------------------------------------------------------------------------------------------------------
CodeGeneratorModule::~CodeGeneratorModule(void){}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::pushInstruction(const char* _Instruction, const char* A, const char* B,  const char* _BaseAddress){
	char _buffWritter[255];
	sprintf_s(_buffWritter, "%s %s %s %s", _Instruction, A, B, _BaseAddress);
	insertCodeToWrite(_buffWritter);
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
void CodeGeneratorModule::insertCodeToWrite(char* _Code, int _codeIndex, bool _Overlap){
	if(_codeIndex > MAX_WRITING_BUFFER)
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
	if(_codeIndex > MAX_WRITING_BUFFER)
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
//----------------------------------------------------------------------------------------------------------------------