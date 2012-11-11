#ifndef ARSTOKEN_H
#define ARSTOKEN_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <process.h>
#include <iostream>
#include <Windows.h>
#include "ATWLex.h"
#include "ARS_ConsoleColorHandler.h"
#include "EventsManager.h"

using namespace ARS_COLOR_HANDLER;

namespace ARS_PRINTER_HELPER{

#define DEFAULT_CONSOLE_WIDTH 80

#if _WIN32
	static char* _tPrinterHelper[42] = {
		"ID", "CONSTANT", "INICIO", "FIM", 
		"ENQUANTO", "FACA", "SE", "ENTAO", 
		"SENAO", "NAO", "E", "OU", "INTEIRO", 
		"REAL", "ESCALA", "PAUSA", "ROTTRANS", 
		"LUZ", "DPONTO", "DCONST", "DFACE", 
		"DOBJETO", "DCOR", "DLUZ", "DVAR", 
		"LPAREN", "RPAREN", "EQ", "DEQ", 
		"TIMES", "OVER", "MINUS", "PLUS", 
		"DIFF", "LT", "GT", "LE", "GE", 
		"COMMA", "EXP_END"};

		static void ARS_printfToken(Token tok);
		static void ARS_printTokenLexeme(Token _CurrentToken, string _Lexeme);
		static void ARS_drawStick(int Color); 

		//simple print
		static void ARS_printToken(Token tok){
			if(tok < 2)
				ARS_setTextColor(_LightBlue);
			else
				ARS_setTextColor(_LightAqua);

			if(tok == EXP_END){
				ARS_setTextColor(_LightYellow);
				printf("%-10s", _tPrinterHelper[tok]);
				ARS_drawStick(_Red);
			}
			else
				printf("%-10s", _tPrinterHelper[tok]);

			ARS_defaultTextAttributes();
		}

		//TODO:make printf clone with colors support

		static void ARS_printTokenLexeme(Token _CurrentToken, string _Lexeme){
			printf("\nToken: ");
			ARS_printToken(_CurrentToken);
			printf("%-30sLexeme: %s\n", "", _Lexeme);
		}

		static void ARS_drawStick(int Color){
			ARS_setTextColor(Color);
			printf("\n");
			string _Var = "";
			for(int i = 0;i<DEFAULT_CONSOLE_WIDTH; i++)
				_Var += '-';
			printf("%s\n",_Var.c_str());
		}
#endif
}
#endif