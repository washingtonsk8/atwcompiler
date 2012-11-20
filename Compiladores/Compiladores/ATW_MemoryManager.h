#ifndef ATW_MEMMANAGER_H
#define ATW_MEMMANAGER_H
//--------------------------------------------------------------------------------------------------------
#include "SymbolTable.h"
#include "Globals.h"
//--------------------------------------------------------------------------------------------------------
#define MEM_BASE_address 0
#define ZERO             0
//--------------------------------------------------------------------------------------------------------
class ATWMemory{
public:
	//--------------------------------------------------------------------------------------------------------
	static ATWMemory* getInstance(){
		static ATWMemory _instance;
		return &_instance;
	}
	//--------------------------------------------------------------------------------------------------------
	ATWMemory(void){
		_PC                  = ZERO;
		_TotalMemAllocation  = ZERO;
		_MemBaseAddress      = MEM_BASE_address;
		_LastMemAddress      = MEM_BASE_address;
		_LastTempAddress     = MEM_BASE_address;
		_TempBaseAddress     = MEM_BASE_address;
	}
	//--------------------------------------------------------------------------------------------------------
	~ATWMemory(void){}
	//--------------------------------------------------------------------------------------------------------
	Address ATWMalloc(Type _Type){
		int _PreviousMemAddress = _LastMemAddress;

		if (_Type == TIPO_REAL)
			_LastMemAddress += 2;
		else
			_LastMemAddress ++;

		_TotalMemAllocation = _LastMemAddress;
		return _PreviousMemAddress;
	}
	//--------------------------------------------------------------------------------------------------------
	Address ATWNovoTemp(Type _Type){
		int _PreviousTempAddress = _LastTempAddress;

		if (_Type == TIPO_REAL)
			_LastTempAddress += 2;
		else
			_LastTempAddress ++;

		return _PreviousTempAddress;
	}
	//--------------------------------------------------------------------------------------------------------
	void ATWResetTemp(){
		_LastTempAddress = _TempBaseAddress;
	}
	//--------------------------------------------------------------------------------------------------------
	Address getPC(){
		return _PC;
	}
	//--------------------------------------------------------------------------------------------------------
	Address incrementPC (int _Value = 1){
		return _PC += _Value;
	}
	//--------------------------------------------------------------------------------------------------------
	void GCXSetBaseTempAddress(){
		_TempBaseAddress = _LastMemAddress;
	}
	//--------------------------------------------------------------------------------------------------------
private:
	Address	_PC;//Contador de programa
	Address _TotalMemAllocation;//Possui o total de mem�ria alocado no momento
	Address _MemBaseAddress;//Possui o endere�o de mem�ria base do in�cio da mem�ria de dados
	Address	_LastMemAddress;//Possui o primeiro endere�o de mem�ria vazio
	Address	_LastTempAddress;//Possui o primeiro endere�o de mem�ria vazio ap�s 0 ou mais tempor�rios
	Address	_TempBaseAddress;//Possui o endere�o de mem�ria base do in�cio dos tempor�rios
};
#endif

namespace MemoryManager{
	extern ATWMemory* _memory;
}
