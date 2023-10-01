#include "lexer.h"

bool determineTokenType(const char* str, const char* pos, int stringLength) {
    if (strncmp(str, pos, stringLength) == 0) {
        return true;
    }
    else {
        return false;
    }
}

Token* readKeyword(const char* p, int* pos) {
    Token* token = malloc(sizeof(Token));

    int len = 0;
    
    while (isalpha(p[*pos + len]) || isdigit(p[*pos + len]) || p[*pos + len] == '_') {
        len++;
    }

    if (determineTokenType("asm", &p[*pos], 3)) {
        token->type = TK_ASM;
    }
    else if (determineTokenType("B8", &p[*pos], 2)) {
        token->type = TK_B8;
    }
    else if (determineTokenType("case", &p[*pos], 4)) {
        token->type = TK_CASE;
    }
    else if (determineTokenType("class", &p[*pos], 5)) {
        token->type = TK_CLASS;
    }
    else if (determineTokenType("const", &p[*pos], 5)) {
        token->type = TK_CONSTANT;
    }
    else if (determineTokenType("continue", &p[*pos], 8)) {
        token->type = TK_CONTINUE;
    }
    else if (determineTokenType("FP64", &p[*pos], 4)) { 
        token->type = TK_FP64;
    }
    else if (determineTokenType("do", &p[*pos], 2)) {
        token->type = TK_DO;
    }
    else if (determineTokenType("default", &p[*pos], 7)) {
        token->type = TK_DEFAULT;
    }
    else if (determineTokenType("else", &p[*pos], 4)) {
        token->type = TK_ELSE;
    }
    else if (determineTokenType("extern", &p[*pos], 6)) {
        token->type = TK_EXTERNAL;
    }
    else if (determineTokenType("for", &p[*pos], 3)) {
        token->type = TK_FOR;
    }
    else if (determineTokenType("false", &p[*pos], 5)) {
        token->type = TK_FALSE;
    }
    else if (determineTokenType("flock", &p[*pos], 5)) {
        token->type = TK_FLOCK;
    }
    else if (determineTokenType("gaggle", &p[*pos], 6)) { 
        token->type = TK_GAGGLE; 
    }
    else if (determineTokenType("if", &p[*pos], 2)) {
        token->type = TK_IF;
    }
    else if (determineTokenType("match", &p[*pos], 5)) {
        token->type = TK_MATCH;
    }
    else if (determineTokenType("return", &p[*pos], 6)) {
        token->type = TK_RETURN;   
    }
    else if (determineTokenType("size", &p[*pos], 4)) {
        token->type = TK_SIZE;
    }
    else if (determineTokenType("stop", &p[*pos], 4)) {
        token->type = TK_STOP;
    }
    else if (determineTokenType("SC8", &p[*pos], 3)) {
        token->type = TK_SC8;
    }
    else if (determineTokenType("SI32", &p[*pos], 4)) {
        token->type = TK_SI32;
    }
    else if (determineTokenType("true", &p[*pos], 4)) {
        token->type = TK_TRUE;
    }
    else if (determineTokenType("UI0", &p[*pos], 3)) { 
        token->type = TK_UI0;
    }
    else if (determineTokenType("while", &p[*pos], 5)) {
        token->type = TK_WHILE;
    }
    else {
        token->type = TK_IDENTIFIER;
    }

    if (token->type == TK_IDENTIFIER) {
        token->string    = malloc(sizeof(char) * len + 1);
        token->strlen = len + 1;
        strncpy(token->string, &p[*pos], len);
        token->string[len] = '\0';
    }

    *(pos) += len;

    return token;
}