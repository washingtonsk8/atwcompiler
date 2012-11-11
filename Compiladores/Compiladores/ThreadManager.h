#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <Windows.h>
#include "CHBuffer.h"
#include <vector>

#define DEFAULT_MAX_THREADS 2
#define DEFAULT_SINGLE_WAIT_TIMEOUT INFINITE
#define DEFAULT_COLETIVE_WAIT_TIMEOUT INFINITE

class ThreadManager
{
public:
	static ThreadManager* getInstance(){
		static ThreadManager _instance;
		_instance.initialize();
		return &_instance;
	}

	int resumeAllThreads();
	int resumeThread(unsigned int ThreadID);
	int stopAllThreads();
	int stopThread(unsigned int ThreadID);

	int createThread(LPTHREAD_START_ROUTINE _Method, LPVOID _tParameters, unsigned int ThreadID);
	void closeUnusedThreads();
	int closeThread(unsigned int ThreadID);
	void closeAllThreads();

	int waitThread(unsigned ThreadID);
	int waitAllThreads();

	~ThreadManager(void);

private:
	void initialize(int ThreadsCount = DEFAULT_MAX_THREADS);
	HANDLE _Threads[DEFAULT_MAX_THREADS];

	/*CONTROLL*/
	bool initialized;
	ThreadManager(void);
};
#endif
