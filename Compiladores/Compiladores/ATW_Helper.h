#ifndef ATWHELPER
#define ATWHELPER
//---------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string>
//---------------------------------------------------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------------------------------------------------
namespace ATW_HELPER{
	extern const char* ATWgetCCStr(int _val);
	extern char* ATWgetCStr(int _val);
	extern char* ATWgetCStrBin(int _val);
	extern int ATWgetInt(string _val);
}
#endif

