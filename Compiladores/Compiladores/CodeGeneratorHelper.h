#ifndef CODEGENERATORHELPER_H
#define CODEGENERATORHELPER_H
const char* ATWgetCStr(int _val){
	char _conversion[255];
	_itoa_s(_val, _conversion, 10);
	string _return  = _conversion;
	return _return.c_str();
}
#endif