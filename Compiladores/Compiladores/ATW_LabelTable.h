#ifndef ATWLABEL
#define ATWLABEL
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "GeneralHashFunctions.h"
//--------------------------------------------------------------------------------------------------------
#define MAX_TOTAL_LABELS 100000
//--------------------------------------------------------------------------------------------------------
typedef struct LabelElement{
	int    _PCAddress;
	char* _Label;

	bool BuildElement(char* _labelP, int _PCAddressP){
		_Label     = _labelP;
		_PCAddress = _PCAddressP;
		return true;
	}
}LabelElement;
//--------------------------------------------------------------------------------------------------------
class ATWLabelTable{
public:
	//--------------------------------------------------------------------------------------------------------
	static ATWLabelTable* getInstance(){
		static ATWLabelTable* _instance;
		return _instance;
	}
	//--------------------------------------------------------------------------------------------------------
	ATWLabelTable(void){
		_labelsTable = new LabelElement*[MAX_TOTAL_LABELS];
		for(int i = 0; i < MAX_TOTAL_LABELS; i++){
			_labelsTable[i] = 0x0;
		}
	}
	//--------------------------------------------------------------------------------------------------------
	~ATWLabelTable(void){
		unallocLabelTable();
	}
	//--------------------------------------------------------------------------------------------------------
	void unallocLabelTable(){
		if(_labelsTable != 0x0){
			for(int i = 0; i < MAX_TOTAL_LABELS; i++){
				if(_labelsTable[i] != 0x0){
					delete _labelsTable[i];
					_labelsTable[i] = 0x0;
				}
			}
			delete _labelsTable;
			_labelsTable = 0x0;
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void pushLabelAddress(char* _element, unsigned int _PCAddress){
		unsigned int _hashing = SDBMHash(_element) % MAX_TOTAL_LABELS;

		if(_labelsTable[_hashing] != 0x0)
			exit(-88);//TODO TRATAR EXCEPTION

		LabelElement* _newElement = new LabelElement();
		_newElement->BuildElement(_element, _PCAddress);

		_labelsTable[_hashing] = _newElement;
	}
	//--------------------------------------------------------------------------------------------------------
	unsigned int getLabelAddress (char* _element){
		unsigned int _hashing = SDBMHash(_element) % MAX_TOTAL_LABELS;

		if(_labelsTable[_hashing] == 0x0)
			exit(-88);//TODO TRATAR EXCEPTION

		return _labelsTable[_hashing]->_PCAddress;
	}
	//--------------------------------------------------------------------------------------------------------
private:
	LabelElement** _labelsTable;
};
#endif