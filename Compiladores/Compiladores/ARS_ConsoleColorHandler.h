#ifndef CONSOLECOLORHANDLER_H
#define CONSOLECOLORHANDLER_H

#include <iostream>
#include <Windows.h>

namespace ARS_COLOR_HANDLER{

#if _WIN32
#define _Black 0 
#define _Blue 1
#define _Green 2 
#define _Aqua 3	
#define _Red 4	 
#define _Purple 5	
#define _Yellow 6
#define _White 7
#define _Gray 8
#define _LightBlue 9
#define _LightGreen 10
#define _LightAqua 11
#define _LightRed 12
#define _LightPurple 13
#define _LightYellow 14
#define _BrightWhite 15

	static HANDLE _Console = GetStdHandle(STD_OUTPUT_HANDLE);
	static WORD DEFAULT_ATT_CHECK = _BrightWhite;//(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	static void ARS_breakLine(int count){
		string _Var = "";
		for(int i = 0; i<count; i++)
			_Var += "\n";
		printf("%s",_Var.c_str());
	}

	static void ARS_setTextColor(WORD wAttribute){
		SetConsoleTextAttribute(_Console, wAttribute); 
	}

	static void ARS_defaultTextAttributes(){
		SetConsoleTextAttribute(_Console, DEFAULT_ATT_CHECK);
	}
#else LINUX
#endif
}
#endif