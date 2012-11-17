#ifndef ATWGCXSimul
#define ATWGCXSimul
//----------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string>
//----------------------------------------------------------------------------------------------------------------------------
#include "CodeGeneratorModule.h"
#include "EventsManager.h"
#include "FileManager.h"
//----------------------------------------------------------------------------------------------------------------------------
#define OPTIMIZED_FILE_NAME "Optimized.asm"
#define HOLE_SIZE           2
//----------------------------------------------------------------------------------------------------------------------------
class ATW_GCXSimulator
{
public:
	ATW_GCXSimulator(void){}
	//------------------------------------------------------------------------------------------------------------------------
	~ATW_GCXSimulator(void){}
	//------------------------------------------------------------------------------------------------------------------------
	bool initialize(CodeGeneratorModule* _cgModule){
		this->_cgModule = _cgModule;
		this->_eManager = EventsManager::getInstance();
		//FMANAGER INITIALIZATION
		this->_fManager = new FileManager();
		_fManager->initialize(0, NULL);
		_fManager->openFile(OPTIMIZED_FILE_NAME, GENERIC_WRITE, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
		//FMANAGER INITIALIZATION
	}
	//------------------------------------------------------------------------------------------------------------------------
	bool unalloc(){
		if(_fManager != 0x0){
			_fManager->dispose(0, NULL);
			delete _fManager;
			_fManager = 0x0;
		}
	}
	//------------------------------------------------------------------------------------------------------------------------
	void printRegContent(){
		printf("A: %f B: %f C: %f D: %f E: %f F: %f\n", A,B,C,D,E,F);
	}
	//------------------------------------------------------------------------------------------------------------------------
	bool flushToSimulator(){

	}
	//------------------------------------------------------------------------------------------------------------------------
	void flushOptimized(){

	}
	//------------------------------------------------------------------------------------------------------------------------
	void simulate(){

	}
	//------------------------------------------------------------------------------------------------------------------------
private:
	long double A, B, C, D, E, F;//REG's
	CodeGeneratorModule* _cgModule;//CODE GENERATOR MODULE
	EventsManager*       _eManager;//EVENTS MANAGER
	FileManager*         _fManager;
};
#endif

