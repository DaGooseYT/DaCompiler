#pragma once

#include "util.h"

// identifiers
#define TK_FLOCK               1
#define TK_GAGGLE              2
#define TK_CONSTANT            3
#define TK_UI0                 4
#define TK_SC8                 5
#define TK_SI32                6
#define TK_FP64                7
#define TK_B8                  8
#define TK_CLASS               9
#define TK_TRUE               10
#define TK_FALSE              11
#define TK_SIZE               12
#define TK_ASM                13

// statements
#define TK_IF                 14
#define TK_ELSE               15
#define TK_FOR                16
#define TK_WHILE              17
#define TK_DO                 18
#define TK_MATCH              19
#define TK_CASE               20
#define TK_DEFAULT            21
#define TK_STOP               22
#define TK_CONTINUE           23
#define TK_RETURN             24

// constants
#define TK_NUMBER             25
#define TK_BYTE               26
#define TK_STRING             27  
#define TK_IDENTIFIER         28

// operators
#define TK_PLUS               29
#define TK_MINUS              30
#define TK_ASTERISK           31
#define TK_SCOPE              32
#define TK_SLASH              33
#define TK_BACKSLASH          34
#define TK_PERCENT            35
#define TK_ASSIGNMENT         36
#define TK_SEMICOLON          37
#define TK_COLON              38
#define TK_LEFT_PARENTHESIS   39
#define TK_RIGHT_PARENTHESIS  40 
#define TK_LEFT_BRACKET       41
#define TK_RIGHT_BRACKET      42
#define TK_LEFT_SQUARE        43
#define TK_RIGHT_SQUARE       44
#define TK_LEFT_ANGLE         45
#define TK_RIGHT_ANGLE        46
#define TK_EXCLAMATION        47
#define TK_QUESTION           48
#define TK_AMP                49
#define TK_TILDE              50
#define TK_PIPE               51
#define TK_COMMA              52
#define TK_DOT                53
#define TK_POINTER            54
#define TK_EQUAL              55
#define TK_NOT_EQUAL          56
#define TK_LESS_OR_EQUAL      57
#define TK_GREATER_OR_EQUAL   58
#define TK_OR                 59
#define TK_AND                60
#define TK_INCREMENT          61
#define TK_DECREMENT          62
#define TK_MULTIPLY_EQUAL     63
#define TK_DIVIDE_EQUAL       64
#define TK_MODULO_EQUAL       65
#define TK_ADD_EQUAL          66
#define TK_MINUS_EQUAL        67

// Scope
#define TK_EXTERNAL           68

typedef struct Token {
    int   type;
    int   num;
    char* string;
    int   strlen;
    bool  hasValue;
} Token;

Token* readKeyword(const char* p, int* pos);
Token* readString(const char* p, int* pos);
Token* readDirective(const char* p, int* pos);
Token* readCharacter(const char* p, int* pos);
bool isComment(const char* p, int pos);
Token* readSymbol(const char* p, int* pos);
Token* readNumber(const char* p, int* pos);
int getTokenType(const Vector* vec, int index);
bool isSymbol(char p);

Vector* lex(char* addr);
