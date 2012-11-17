#ifndef ATW_MEMMANAGER_H
#define ATW_MEMMANAGER_H
//--------------------------------------------------------------------------------------------------------
#include "SymbolTable.h"
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
	int ATWMalloc(Type _Type = TIPO_VAZIO){
		int _PreviousMemAddress = _LastMemAddress;

		if (_Type == TIPO_REAL)
			_LastMemAddress += 2;
		else if (_Type == TIPO_INTEIRO || _Type == TIPO_LOGICO)
			_LastMemAddress ++;

		_TotalMemAllocation = _LastMemAddress;
		return _PreviousMemAddress;
	}
	//--------------------------------------------------------------------------------------------------------
	int ATWNovoTemp(){
		_LastTempAddress ++;
		return _LastTempAddress-1;
	}
	//--------------------------------------------------------------------------------------------------------
	void ATWResetTemp(){
		_LastTempAddress = _TempBaseAddress;
	}
	//--------------------------------------------------------------------------------------------------------
	int getPC(){
		return _PC;
	}
	//--------------------------------------------------------------------------------------------------------
	int incrementPC (int _Value = 1){
		return _PC += _Value;
	}
	//--------------------------------------------------------------------------------------------------------
	void GCXSetBaseTempAddress(){
		_TempBaseAddress = _LastMemAddress;
	}
	//--------------------------------------------------------------------------------------------------------
private:
	int	_PC;
	int _TotalMemAllocation;
	int _MemBaseAddress;
	int	_LastMemAddress;
	int	_LastTempAddress;
	int	_TempBaseAddress;
};
#endif

namespace MemoryManager{
	extern ATWMemory* _memory;
}
