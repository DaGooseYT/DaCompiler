#include "preprocessor.h"

StringMap* defineMap;

int enabledRead(const Vector* inputVector, int* index, Vector* outputVector) {
    Token* token = inputVector->contents[*index];

    if (token->type != TK_TILDE) {
        if (token->type == TK_IDENTIFIER && stringMapContains(defineMap, token->string)) {
            Token* value = getStringMap(defineMap, token->string);

            if (value != NULL) {
                pushVector(outputVector, value);
            }

            (*index)++;
            return ENABLED;
        }
        else {
            pushVector(outputVector, token);
            (*index)++;
            return ENABLED;
        }
    }
    else {
        if (strncmp("macro", token->string, 6) == 0) {
            const Token* name = inputVector->contents[++*index];
            (*index)++;

            if (token->hasValue) {
                Token* value = inputVector->contents[*index];
                appendStringMap(defineMap, name->string, value);
                (*index)++;

                return ENABLED;
            }
            else {
                appendStringMap(defineMap, name->string, NULL);
                return ENABLED;
            }
        }
        else if (strncmp("using", token->string, 6) == 0) { 
            token = inputVector->contents[++*index];

            if (token->type == TK_STRING) {
                char* file = readFile(token->string);

                if (file == NULL) {
                    printf("error: failed to open the file: \"%s\".\n", token->string); 
                    return INVALID;
                }

                const Vector* includeVector = lex(file);

                if (includeVector == NULL) {
                    printf("error: could not complete lexical analysis.\n");
                    return INVALID;
                }

                const Vector* includeProcessedVector = preprocess(includeVector);

                if (includeProcessedVector == NULL) {
                    printf("error: preprocess pass failed.\n");
                    return INVALID;
                }

                for (int i = 0; i < includeProcessedVector->size; i++) {
                    pushVector(outputVector, includeProcessedVector->contents[i]);
                }

                (*index)++;
            }
            else {
                printf("Token #%ls is invalid.\n", index);
                return INVALID;
            }

            return ENABLED;
        }
        else {
            return INVALID;
        }
    }
}

int disabledRead(const Vector* inputVector, int* index, Vector* outputVector) {
    const Token* token = inputVector->contents[*index];

    (*index)++;

    if (token->type != TK_TILDE) {
        return DISABLED;
    }
    else {
        return DISABLED;
    }
}

Vector* preprocess(const Vector* inputVector) {
    Vector* outputVector = buildVectorList();

    if (defineMap == NULL) {
        defineMap = buildStringMap(32);
    }

    int state = ENABLED;
    int index = 0;

    while (index < inputVector->size) {
        switch (state) {
            case ENABLED: {
                state = enabledRead(inputVector, &index, outputVector);

                if (state == INVALID) {
                    printf("Token #%d is invalid.\n", index);
                    return NULL;
                }

                break;
            }
            case DISABLED: {
                state = disabledRead(inputVector, &index, outputVector);

                if (state == INVALID) {
                    printf("Token #%d is invalid.\n", index);
                    return NULL;
                }

                break;
            }
            default: {
                printf("Invalid state=%d\n", state);
                return NULL;
            }
        }
    }

    return outputVector;
}
