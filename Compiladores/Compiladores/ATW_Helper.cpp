#include "ATW_Helper.h"
//---------------------------------------------------------------------------------------------------------------------
namespace ATW_HELPER{
	//---------------------------------------------------------------------------------------------------------------------
	const char* ATWgetCCStr(int _val){
		char* _conversion = (char*)malloc(sizeof(char)*255);
		memset(_conversion, 0, sizeof(char));
		_itoa(_val, _conversion, 10);
		string _return = _conversion;
		return _return.c_str();
	}
	//---------------------------------------------------------------------------------------------------------------------
	char* ATWgetCStr(int _val){
		char* _conversion = (char*)malloc(sizeof(char)*255);
		memset(_conversion, 0, sizeof(char));
		_itoa(_val, _conversion, 10);
		return _conversion;
	}
	//---------------------------------------------------------------------------------------------------------------------
	char* ATWgetCStrBin(int _val){
		char* _conversion = (char*)malloc(sizeof(char)*255);
		memset(_conversion, 0, sizeof(char));
		_itoa(_val, _conversion, 2);
		return _conversion;
	}
	//---------------------------------------------------------------------------------------------------------------------
	int ATWgetInt(string _val){
		return atoi(_val.c_str());
	}
	//---------------------------------------------------------------------------------------------------------------------
}