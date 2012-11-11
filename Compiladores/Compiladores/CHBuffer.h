#ifndef CHBUFFER_H
#define CHBUFFER_H
//---------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <stdio.h>
//---------------------------------------------------------------------------------------------------------------------
#define OK      0
#define FULL    666
//---------------------------------------------------------------------------------------------------------------------
#define DEFAULT_SIGNAL_WAITING_TIME INFINITE
//---------------------------------------------------------------------------------------------------------------------
template <class T>
class CHBuffer
{
	//---------------------------------------------------------------------------------------------------------------------
public:
	CHBuffer(int _eSize = 1){
		_eCount = 0;
		_cap = _eSize;
		_nextITR = 0;//NEXT INDEX TO REMOVE
		_freePosition = 0;
		_elements = new T*[_eSize];
		memset(_elements, 0, sizeof(T*)*_eSize);

		/*MUTUAL EXCLUSION CONTROLLING*/
		/*BUFFER WRINTING SEMAPHORE CONTROLLER*/
		_BUFFER_WRITE = CreateSemaphore(
			NULL,
			_eSize-1,
			_eSize-1,
			NULL);

		/*BUFFER READING SEMAPHORE CONTROLLER*/
		_BUFFER_READ = CreateSemaphore(
			NULL,
			0,
			_eSize-1,
			NULL);

		/*MUTEX CREATION*/
		_WR_CRITICAL_SECTION = CreateMutex(
			NULL,
			0,
			NULL);
	}
	//---------------------------------------------------------------------------------------------------------------------
	~CHBuffer(void){
		/*CLOSE OPEN HANDLE'S*/
		CloseHandle(_BUFFER_WRITE);
		CloseHandle(_BUFFER_READ);
		CloseHandle(_WR_CRITICAL_SECTION);
		/*ENDING TO CLOSE HANDLER's*/

		free(_elements);
	}
	//---------------------------------------------------------------------------------------------------------------------
	int insertElement(const T& _Element){
		/*WAITING WRITING SEMAPHORE*/
		WaitForSingleObject(_BUFFER_WRITE, DEFAULT_SIGNAL_WAITING_TIME);

		if(_state == FULL)
			return FULL;

		/*WAITING MUTEX OWNERSHIP FOR WRITING OPERATION*/
		DWORD dwWaitResult = WaitForSingleObject( 
			_WR_CRITICAL_SECTION,    
			INFINITE);

		switch (dwWaitResult) 
		{
		case WAIT_OBJECT_0: 
			__try {   
				_elements[_freePosition] = new T();
				(*_elements[_freePosition]) = _Element;

				_eCount++;
			} 

			__finally { 
				if (!ReleaseMutex(_WR_CRITICAL_SECTION)) 
				{ 
					//::TODO::MUTEX RELEASING ERROR
				} 
			} 
			break; 

		case WAIT_ABANDONED: 
			return FALSE; 
		}
		/*WAITING MUTEX OWNERSHIP FOR WRITING OPERATION*/

		/*CALCULATING NEXT FREE POSITION*/
		if(_freePosition+1 < _cap){
				_freePosition++;
			//else{ _state = FULL; return FULL;}//ALGORITHMIC MUTUAL EXCLUSION CONTROLL
		}else{
			_freePosition = 0;
		}//else{ _state = FULL; return FULL;}//ALGORITHMIC MUTUAL EXCLUSION CONTROLL
		/*CALCULATING NEXT FREE POSITION*/

		/*RELEASING READING SEMAPHORE*/
		ReleaseSemaphore(_BUFFER_READ, 1, NULL);
		return OK;
	}
	//---------------------------------------------------------------------------------------------------------------------
	T removeElement(){
		/*WAITING READING SEMAPHORE*/
		WaitForSingleObject(_BUFFER_READ, DEFAULT_SIGNAL_WAITING_TIME);

		if(_eCount < 1)
			exit(-1);//TRATAR ERRO COM EMANAGER

		/*WAITING MUTEX OWNERSHIP FOR WRITING OPERATION*/
		DWORD dwWaitResult = WaitForSingleObject( 
			_WR_CRITICAL_SECTION,    
			INFINITE);

		/*RETURNABLE ELEMENT*/
		T _return;

		switch (dwWaitResult) 
		{
		case WAIT_OBJECT_0: 
			__try{
				_return = (*_elements[_nextITR]);

				delete _elements[_nextITR];
				_elements[_nextITR] = 0x0;
			}

			__finally { 
				if (!ReleaseMutex(_WR_CRITICAL_SECTION)) 
				{ 
					//::TODO::MUTEX RELEASING ERROR
				} 
			} 
			break; 

		case WAIT_ABANDONED: 
			exit(-1);//TRATAR ERRO COM EMANAGER
		}//MUTEX CONTROLL END

		/*CALCULATING NEXT REMOVING POSITION*/
		if(_nextITR+1<_cap)
			_nextITR++;
		else
			_nextITR = 0;
		
		_eCount--;

		//if(_state == FULL)
		//	_state = OK;//ALGORITHMIC MUTUAL EXCLUSION CONTROLL
		/*CALCULATING NEXT REMOVING POSITION*/

		/*RELEASING WRITING SEMAPHORE*/
		ReleaseSemaphore(_BUFFER_WRITE, 1, NULL);
		return _return;
	}
	//---------------------------------------------------------------------------------------------------------------------
	/*DEBUG PRINTER, WORKS ONLY FOR INTEGERS*/
	void print(){
		for(int i = 0; i < _cap; i++){
			if(_elements[i] != 0x0)
				printf("%i ", (*_elements[i]));
		}
	}
	//---------------------------------------------------------------------------------------------------------------------
private:
	/*BUFFER ACESS CONTROLL*/
	HANDLE _BUFFER_WRITE;
	HANDLE _BUFFER_READ;
	HANDLE _WR_CRITICAL_SECTION;
	/*BUFFER ACESS CONTROLL*/

	int _eCount;
	int _state;
	int _cap;
	int _nextITR;
	int _freePosition;
	T** _elements;
};

#endif

