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
	int ATWMalloc(Type _Type){
		int _PreviousMemAddress = _LastMemAddress;

		if (_Type == TIPO_REAL)
			_LastMemAddress += 2;
		else
			_LastMemAddress ++;

		_TotalMemAllocation = _LastMemAddress;
		return _PreviousMemAddress;
	}
	//--------------------------------------------------------------------------------------------------------
	int ATWNovoTemp(Type _Type){
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
	int	_PC;//Contador de programa
	int _TotalMemAllocation;//Possui o total de memória alocado no momento
	int _MemBaseAddress;//Possui o endereço de memória base do início da memória de dados
	int	_LastMemAddress;//Possui o primeiro endereço de memória vazio
	int	_LastTempAddress;//Possui o primeiro endereço de memória vazio após 0 ou mais temporários
	int	_TempBaseAddress;//Possui o endereço de memória base do início dos temporários
};
#endif

namespace MemoryManager{
	extern ATWMemory* _memory;
}
