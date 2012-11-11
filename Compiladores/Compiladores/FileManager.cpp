#include "FileManager.h"
#include "ERRO_DEF.h"
#include "Globals.h"
//---------------------------------------------------------------------------------------------------------------------
FileManager::FileManager()
{
}
//---------------------------------------------------------------------------------------------------------------------
FileManager::~FileManager(void)
{
	if(_eManager != NULL)
		_eManager = NULL;//TODO:RELACAO DE DEPENDENCIA, NAO PRECISA DELETAR
}
//---------------------------------------------------------------------------------------------------------------------
void FileManager::initialize(int _Argc, void** _Argv){
	this->setGroupID(PROGRAM_GROUP);
	this->_eManager = EventsManager::getInstance();
	_nLineR = 0;
	_LinePos = 0;
	_nLineW = 0;
	_fReadResult = false;
	_rBuffer;
}
//---------------------------------------------------------------------------------------------------------------------
void FileManager::dispose(int _Argc, void** _Argv){
	closeFile();
	//TODO:implements
}
//---------------------------------------------------------------------------------------------------------------------
void FileManager::openFile(const char* _Name, DWORD dwDesiredAccess, DWORD dwCreationDisposition, DWORD flagsAndAttributes){
	//Usado para converter char* para wchar_t
	int _NameLenght = lstrlenA(_Name);
	BSTR unicodestr = SysAllocStringLen(NULL, _NameLenght);
	MultiByteToWideChar(CP_ACP, 0, _Name, _NameLenght, unicodestr, _NameLenght);

	_File = CreateFile(
		unicodestr,
		dwDesiredAccess,
		(FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE),
		FILE_SECURITY_DESCRIPTOR,
		dwCreationDisposition,
		flagsAndAttributes,
		FILE_TEMPLATE_FILE );

	if(_File == INVALID_HANDLE_VALUE)
		_eManager->callHandlers(this->getGroupID(), FILE_NOT_FOUND_EXCEPTION, NULL);

	//TODO:fix _eManager->callHandlers(this->getGroupID(), SUCCEED);

	SysFreeString(unicodestr);
}
//---------------------------------------------------------------------------------------------------------------------
BOOL FileManager::closeFile(){
#if _WIN32
	//TODO:comentar
	return CloseHandle(_File);
#else
	//TODO:implementar logica do linux
#endif
}
//---------------------------------------------------------------------------------------------------------------------
vector<FileOffset> FileManager::getOffsetsVector(const char* _Name, int _tCount){
	vector<FileOffset> _OffsetsVector;
	return _OffsetsVector;
}
//---------------------------------------------------------------------------------------------------------------------
void FileManager::writeInFile(const char* _value){
	if(_value == NULL)
		_eManager->callHandlers(this->getGroupID(), FATAL_ERROR, NULL);

	int _strLen = strlen(_value);

	 WriteFile(
		_File,
		_value,
		_strLen,
		&_nLineW,
		LINE_WRITE_OVERLAP);

	 if(_nLineW <= 0)
		 printf("Writing erro at file\n");
}
//---------------------------------------------------------------------------------------------------------------------
BOOL FileManager::loadBuffer(){
	return ReadFile(
		_File,
		_rBuffer,
		sizeof(_rBuffer),
		&_nLineR,
		LINE_READ_OVERLAP);
}
//---------------------------------------------------------------------------------------------------------------------
char FileManager::getNextChar(){
	if (!((unsigned)_LinePos < _nLineR)){
		loadBuffer();
		if (_nLineR){
			_LinePos = 0;
			return _rBuffer[_LinePos++];
		}
		else
			return NULL;
	}
	else		
		return _rBuffer[_LinePos++];
}
//---------------------------------------------------------------------------------------------------------------------
int FileManager::ungetNextChar(){
	_LinePos --;
	return _LinePos;//ERRO se negativo for retornado
}