#ifndef ERRO_DEF_H
#define ERRO_DEF_H
//---------------------------------------------------------------------------------------------------------------------
/*GHOST GROUP*/
#define NULL_ARGUMENT 999
//---------------------------------------------------------------------------------------------------------------------
/*GENERAL ERROS*/
#define FATAL_ERROR 666
#define INSUFFICIENT_ARGUMENTS 667
//---------------------------------------------------------------------------------------------------------------------
/*PROGRAM OPs*/
#define NORMAL_ERROR -1
#define SUCCEED       0
#define FILE_NOT_FOUND_EXCEPTION 2
#define OPERATION_DONE 3
#define PRINTED 4
//---------------------------------------------------------------------------------------------------------------------
/*LEX ERRORS*/
#define COMPILING 49
#define INVALID_CHARACTER 50
#define LEX_COMMON_ERROR   51
#define CONSTANT_DETECTION_ERRO 52
#define UNEXPECTED_FILE_END 53
#define LEXEME_ERROR 54
//---------------------------------------------------------------------------------------------------------------------
/*SIN ERRORS*/
#define UNEXPECTED_TOKEN 100
//---------------------------------------------------------------------------------------------------------------------
/*SEM ERRORS*/
#define UNDECLARED_ID 200
#define ALREADY_DECLARED_ID 201
#define IMCOMPATIBLE_CLASS 202
#define INCOMPATILBE_TYPES 203
#define VAL_RESTRICTION 204
//---------------------------------------------------------------------------------------------------------------------
#endif

