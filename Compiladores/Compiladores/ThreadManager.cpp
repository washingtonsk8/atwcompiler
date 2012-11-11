#include "ThreadManager.h"
//---------------------------------------------------------------------------------------------------------------------
void ThreadManager::initialize(int threadsCount){
	if(initialized)
		return;

	initialized = true;
}
//---------------------------------------------------------------------------------------------------------------------
ThreadManager::ThreadManager(void)
{
	//TODO:nothing
}
//---------------------------------------------------------------------------------------------------------------------
ThreadManager::~ThreadManager(void)
{
	//TODO:nothing
}
//---------------------------------------------------------------------------------------------------------------------
int ThreadManager::createThread(LPTHREAD_START_ROUTINE _Method, LPVOID _tParameters, unsigned int ThreadID){
	if(ThreadID >= DEFAULT_MAX_THREADS)
		return -1;

	if(_Threads[ThreadID] == INVALID_HANDLE_VALUE)
		return -1;

	HANDLE newThread = CreateThread(
		NULL,
		0,
		_Method,
		_tParameters,
		CREATE_SUSPENDED,
		NULL);

	_Threads[ThreadID] = newThread;

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int ThreadManager::resumeAllThreads(){
	for(int i = 0; i < DEFAULT_MAX_THREADS ;i++){
		HANDLE _current = _Threads[i];
		if(_current == INVALID_HANDLE_VALUE)
			continue;
		ResumeThread(_current);
	}
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int ThreadManager::resumeThread(unsigned int ThreadID){
	if(ThreadID >= DEFAULT_MAX_THREADS)
		return -1;

	if(_Threads[ThreadID] == INVALID_HANDLE_VALUE)
		return -1;

	ResumeThread(_Threads[ThreadID]);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
void ThreadManager::closeUnusedThreads(){
	for(int i = 0; i < DEFAULT_MAX_THREADS ;i++){
		HANDLE _current = _Threads[i];
		if(_current == INVALID_HANDLE_VALUE)
			continue;
		DWORD _wRes = WaitForSingleObject(_current, 0);
		if(_wRes = WAIT_OBJECT_0)
			CloseHandle(_current);
	}
}
//---------------------------------------------------------------------------------------------------------------------
/*FECHA DIRETO INDEPENDENTE DA EXECUÇÃO*/
int ThreadManager::closeThread(unsigned int ThreadID){
	if(ThreadID >= DEFAULT_MAX_THREADS)
		return -1;

	if(_Threads[ThreadID] == INVALID_HANDLE_VALUE)
		return -1;

	HANDLE _current = _Threads[ThreadID];
	TerminateThread(_current, 0);
	CloseHandle(_current);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
void ThreadManager::closeAllThreads(){
	for(int i = 0; i < DEFAULT_MAX_THREADS ;i++){
		HANDLE _current = _Threads[i];
		TerminateThread(_current, 0);
		CloseHandle(_current);
	}
}
//---------------------------------------------------------------------------------------------------------------------
/*return 1 - esperando*/
/*return 0 - ja acabou*/
int ThreadManager::waitThread(unsigned int ThreadID){
	if(ThreadID >= DEFAULT_MAX_THREADS)
		return -1;

	if(_Threads[ThreadID] == INVALID_HANDLE_VALUE)
		return -1;

	HANDLE _current = _Threads[ThreadID];

	DWORD _wRes = WaitForSingleObject(_current, DEFAULT_SINGLE_WAIT_TIMEOUT);

	return _wRes;
}
//---------------------------------------------------------------------------------------------------------------------
int ThreadManager::waitAllThreads(){
	DWORD _wRes;
	_wRes = WaitForMultipleObjects(DEFAULT_MAX_THREADS,
		_Threads,
		TRUE,
		DEFAULT_COLETIVE_WAIT_TIMEOUT);

	return _wRes;
}
