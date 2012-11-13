#ifndef ATW_MEMMANAGER_H
#define ATW_MEMMANAGER_H
//--------------------------------------------------------------------------------------------------------
#define MEM_BASE_address 0
#define ZERO            0
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
		_PC                 = ZERO;
		_TotalMemAllocation = ZERO;
		_MemBaseaddress      = MEM_BASE_address
		_LastMemaddress      = _MemBaseaddress;
		_LastTempaddress     = _LastMemaddress;
		_TempBaseaddress     = _LastMemaddress;
	}
	//--------------------------------------------------------------------------------------------------------
	~ATWMemory(void){}
	//--------------------------------------------------------------------------------------------------------
	int ATWMalloc(Type _Type = TIPO_VAZIO){
		int _PreviousMemaddress = _LastMemaddress;

		if (_Type == TIPO_REAL)
			_LastMemaddress += 2;
		else if (_Type == TIPO_INTEIRO || _Type == TIPO_LOGICO)
			_LastMemaddress ++;

		_TotalMemAllocation = _LastMemaddress;
		return _PreviousMemaddress;
	}
	//--------------------------------------------------------------------------------------------------------
	int ATWNovoTemp(){
		_LastTempaddress ++;
		return _LastTempaddress-1;
	}
	//--------------------------------------------------------------------------------------------------------
	void ATWResetTemp(){
		_LastTempaddress = _TempBaseaddress;
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
private:
	int	_PC;
	int _TotalMemAllocation;
	int _MemBaseaddress;
	int	_LastMemaddress;
	int	_LastTempaddress;
	int	_TempBaseaddress;
};
#endif