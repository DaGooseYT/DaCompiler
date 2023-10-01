#include "lexer.h"

Vector* lex(char* addr) {
    Vector* vector = buildVectorList();

    int pos = 0;
    const char* p = addr;
    Token* token = NULL;

    while (p[pos]) {
        if (isspace(p[pos])) {
            pos++;
        }
        else if (isComment(p, pos)) {
            while (p[pos] != '\n') {
                pos++;
            } 
        }
        else if (p[pos] == '~') {
            token = readDirective(p, &pos);

            if (token == NULL) {
                printf("Failed to read directive.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (p[pos] == '\'') {
            token = readCharacter(p, &pos);

            if (token == NULL) {
                printf("Failed to read character.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (p[pos] == '"') {
            token = readString(p, &pos);

            if (token == NULL) {
                printf("Failed to read string.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (isSymbol(p[pos])) {
            token = readSymbol(p, &pos);

            if (token == NULL) {
                printf("Failed to read symbol.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (isalpha(p[pos]) || p[pos] == '_') {
            token = readKeyword(p, &pos);

            if (token == NULL) {
                printf("Failed to read identifier.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (isdigit(p[pos])) {
            token = readNumber(p, &pos);

            if (token == NULL) {
                printf("Failed to read number.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else {
            printf("Invalid character[%d]='%c'\n", pos, p[pos]);
            return NULL;
        }
    }

    return vector;
}

Token* readDirective(const char* p, int* pos) {
    (*pos)++;
    int len = 0;

    while (p[*pos + len] != ' ' && p[*pos + len] != '\n') {
        len++;
    }

    Token* token = calloc(1, sizeof(Token));
    token->type = TK_TILDE;
    token->strlen = len;
    token->string = malloc(sizeof(char) * token->strlen + 1);
    strncpy(token->string, &p[*pos], token->strlen);
    token->string[token->strlen] = '\0';

    *pos += len;
    int a = 0;

    while (p[*pos + a] == ' ') {
        a++;
    }

    while (isalpha(p[*pos + a]) || p[*pos + a] == '_' || isdigit(p[*pos + a])) {
        a++;
    }

    while (p[*pos + a] == ' ') {
        a++;
    }

    if (p[*pos + a] != '\n') {
        token->hasValue = true;
    }

    return token;
}

Token* readCharacter(const char* p, int* pos) {
    (*pos)++;
    char c = p[*pos];
    (*pos)++;

    if (c == '\\') {
        const char s = p[*pos];
        (*pos)++;

        switch (s) {
            case '0': {
                c = '\0';
                break; 
            }
            case 'n': {
                c = '\n';
                break;
            }
            case '\'': {
                c = '\'';
                break;
            }
            default: {
                break;
            }
        }        
    }

    if (p[*pos] != '\'') {
        printf("char is not closed.\n");
        return NULL;
    }

    (*pos)++;

    Token* token = calloc(1, sizeof(Token));
    token->type = TK_BYTE;
    token->num  = c;
    return token;
}

Token* readString(const char* p, int* pos) {
    (*pos)++;
    int len = 0;
    bool escape = false;

    while (true) {
        if (!escape && p[*pos + len] == '"') {
            break;
        }

        if (p[*pos + len] == '\\') {
            escape = true;
            len++;
        }
        else {
            escape = false;
            len++;
        }
    }

    Token* token = calloc(1, sizeof(Token));
    token->type = TK_STRING;
    token->strlen = len;
    token->string = malloc(sizeof(char) * token->strlen + 1);
    strncpy(token->string, &p[*pos], token->strlen);
    token->string[token->strlen] = '\0';

    *pos += (len + 1);
    return token;
}

Token* readNumber(const char* p, int* pos) {
    Token* token = calloc(1, sizeof(Token));
    token->type = TK_NUMBER;

    int len = 0;

    while (isdigit(p[*pos + len])) {
        len++;
    }

    char buf[32];
    strncpy(buf, &p[*pos], 32);
    buf[len] = '\0';

    token->num = atoi(buf);
    *pos += len;
    return token;
}

bool isComment(const char* p, int pos) {
    return(strncmp(&p[pos], "#", 1) == 0);
}

int getTokenType(const Vector* vec, int index) {
    Token* token = vec->contents[index];
    return token->type;
}