#ifndef ENUM_SITE
#define ENUM_SITE

typedef enum{
	/*Marcas Multicaracteres*/
	ID = 0, CONSTANT = 1,

	/*Palavras Reservadas*/
	INICIO = 2, FIM = 3, ENQUANTO = 4, FACA = 5, SE = 6, ENTAO = 7,
	SENAO = 8, NAO = 9, E = 10, OU = 11, INTEIRO = 12, REAL = 13, ESCALA = 14,
	PAUSA = 15, ROTTRANS = 16, LUZ = 17, DPONTO = 18, DCONST = 19, DFACE = 20,
	DOBJETO = 21, DCOR = 22, DLUZ = 23, DVAR = 24,

	/*Símbolos Especiais*/
	LPAREN = 25, RPAREN = 26, EQ = 27, DEQ = 28, TIMES = 29, OVER = 30,
	MINUS = 31, PLUS = 32, DIFF = 33, LT = 34, GT = 35, LE = 36, GE = 37,
	COMMA = 38, EXP_END = 39,

	/*Marcas de Controle*/
	ENDFILE = 40, DEFAULT_TOKEN = 41
}Token;

typedef enum{
	START, INID, INATRIB, INCOMMENT, INLT, INGT,
	INNUM1, INNUM2, INNUM3, INNUM4, INDOT0,
	INDOT0NUM, INDOT1, INDOT2, INDOT3, INDOT4,
	INBREAK, INBREAKCOMMENT, DONE
}StateType;

#endif