#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <Windows.h>
#include "EventsManager.h"
#include "ProgramComponent.h"
#include "IInitializable.h"

using namespace std;

#define FILE_SECURITY_DESCRIPTOR NULL
#define FILE_TEMPLATE_FILE       NULL
#define LINE_READ_OVERLAP        NULL
#define LINE_WRITE_OVERLAP       NULL
#define RETURN_CHAR_SIZE         0
#define DEFAULT_BUF_SIZE         1024

typedef struct FileOffset
{
	int _oStart;
	int _oEnd;
}FileOffset;

class FileManager : public ProgramComponent, public IInitializable, public IDisposable
{
public:
	FileManager();
	~FileManager(void);

	void initialize(int _Argc, void** _Argv);
	void dispose(int _Argc, void** _Argv);

	void openFile(const char* _Name, 
	DWORD dwDesiredAccess = GENERIC_READ,
	DWORD dwCreationDisposition = OPEN_EXISTING,
	DWORD flagsAndAttributes = FILE_ATTRIBUTE_READONLY);
	BOOL closeFile();
	BOOL loadBuffer();
	void writeInFile(const char* _value);
	vector<FileOffset> getOffsetsVector(const char* _Name, int _tCount);
	char getNextChar();
	int ungetNextChar();

private:
	EventsManager* _eManager;
	INT _LinePos;

#if _WIN32
	HANDLE& getFile();
#else
	int getFile();
#endif

private:
#if _WIN32
	HANDLE _File;
	DWORD  _nLineR;
	DWORD  _nLineW;
	char _rBuffer[DEFAULT_BUF_SIZE];
	BOOL _fReadResult;
#else
	int _FilDes;
#endif
};

#endif