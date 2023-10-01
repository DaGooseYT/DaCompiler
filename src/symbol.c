#include "lexer.h"

Token* readSymbol(const char* p, int* pos) {
    Token* token = calloc(1, sizeof(Token));

    const char first = p[*pos];
    (*pos)++;
    const char second = p[*pos];
    
    switch (first) {
        case '[': {
            token->type = TK_LEFT_SQUARE;
            return token;
        }
        case ']': {
            token->type = TK_RIGHT_SQUARE;
            return token;
        }
        case '(': {
            token->type = TK_LEFT_PARENTHESIS;
            return token;
        }
        case ')': {
            token->type = TK_RIGHT_PARENTHESIS;
            return token;
        }
        case '{': {
            token->type = TK_LEFT_BRACKET;
            return token;
        }
        case '}': {
            token->type = TK_RIGHT_BRACKET;
            return token;
        }
        case '*': {
            if (second == '=') {
                token->type = TK_MULTIPLY_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_ASTERISK;
            }

            return token;
        }
        case '/': {
            if (second == '=') {
                token->type = TK_DIVIDE_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_SLASH;
            }

            return token;
        }
        case '+': {
            if (second == '+') {
                token->type = TK_INCREMENT;
                (*pos)++;
            }
            else if (second == '=') {
                token->type = TK_ADD_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_PLUS;
            }

            return token;
        }
        case '-': {
            if (second == '-') {
                token->type = TK_DECREMENT;
                (*pos)++;
            }
            else if (second == '=') {
                token->type = TK_MINUS_EQUAL;
                (*pos)++;
            }
            else if (second == '>') {
                token->type = TK_POINTER;
                (*pos)++;
            }
            else {
                token->type = TK_MINUS;
            }

            return token;
        }
        case '%': {
            if (second == '=') {
                token->type = TK_MODULO_EQUAL;
            }
            else {
                token->type = TK_PERCENT;
            }

            return token;
        }
        case '=': {
            if (second == '=') {
                token->type = TK_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_ASSIGNMENT;
            }

            return token;
        }
        case '!': {
            if (second == '=') {
                token->type = TK_NOT_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_EXCLAMATION;
            }

            return token;
        }
        case '?': {
            token->type = TK_QUESTION;
            return token;
        }
        case '&': {
            if (second == '&') {
                token->type = TK_AND;
                (*pos)++;
            }
            else {
                token->type = TK_AMP;
            }

            return token;
        }
        case ';': {
            token->type = TK_SEMICOLON;
            return token;
        }
        case ':': {
            if (second == ':') {
                token->type = TK_SCOPE;
                (*pos)++;
            }
            else {
                token->type = TK_COLON;
            }

            return token;
        }
        case '<': {
            if (second == '=') {
                token->type = TK_LESS_OR_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_LEFT_ANGLE;
            }
            
            return token;
        }
        case '>': {
            if (second == '=') {
                token->type = TK_GREATER_OR_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_RIGHT_ANGLE;
            }

            return token;
        }
        case '|': {
            if (second == '|') {
                token->type = TK_OR;
                (*pos)++;
            }
            else {
                token->type = TK_PIPE;
            }

            return token;
        }
        case ',': {
            token->type = TK_COMMA;
            return token;
        }
        case '.': {
            token->type = TK_DOT;
            return token;
        }
        case '\\': {
            token->type = TK_BACKSLASH;
            return token;
        }
        default: {
            return NULL;
        }
    }
}

bool isSymbol(char p) {
    switch (p) {
        case '[':
        case ']':
        case '(':
        case ')':
        case '{':
        case '}':
        case '*':
        case '/':
        case '+':
        case '-':
        case '%':
        case '=':
        case '!':
        case '?': 
        case '&':
        case ';':
        case ':':
        case '<':
        case '>':
        case '|':
        case ',':
        case '.':
        case '\\': {
            return true;
        }
        default: {
            return false;
        }
    }
}