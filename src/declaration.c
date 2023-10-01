#include "parser.h"

bool isDeclarator(const Vector* vector, int index) {
    const Token* token = vector->contents[index];
    return(token->type == TK_IDENTIFIER || token->type == TK_ASTERISK);
}

ParameterDeclarationNode* makeParameterDeclarationNode(const Vector* vectorList, int* index) {
    ParameterDeclarationNode* parameterDeclarationNode = calloc(1, sizeof(ParameterDeclarationNode));
    parameterDeclarationNode->declaratorSpecifierNodes = buildVectorList();

    while (isDeclarationSpecifier(vectorList, *index)) {
        DeclarationSpecifierNode* declarationSpecifierNode = makeDeclarationSpecifierNode(vectorList, index);

        if (declarationSpecifierNode == NULL) {
            printf("error: could not create declaration-specifier-type node.\n");
            return NULL;
        }

        pushVector(parameterDeclarationNode->declaratorSpecifierNodes, declarationSpecifierNode);
    }

    if (isDeclarator(vectorList, *index)) {
        parameterDeclarationNode->declaratorNode = makeDeclaratorNode(vectorList, index); 
    } 

    return parameterDeclarationNode;
}

DirectDeclaratorNode* makeDirectDeclaratorNode(const Vector* vectorList, int* index) {
    DirectDeclaratorNode* directDeclaratorNode1 = calloc(1, sizeof(DirectDeclaratorNode));
    directDeclaratorNode1->identifierList = buildVectorList();

    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_IDENTIFIER: {
            directDeclaratorNode1->identifier = strdup(token->string);
            (*index)++;
            break;
        }
        case TK_LEFT_SQUARE: {
            (*index)++;
            // @todo
            break;
        }
        default: {
            break;
        }
    }

    DirectDeclaratorNode* current = directDeclaratorNode1;
    token = vectorList->contents[*index];

    while (token->type == TK_LEFT_SQUARE || token->type == TK_LEFT_PARENTHESIS) {
        (*index)++;
   
        DirectDeclaratorNode* directDeclaratorNode2 = calloc(1, sizeof(DirectDeclaratorNode));
        directDeclaratorNode2->directDeclaratorNode = current;
        directDeclaratorNode2->identifierList = buildVectorList();

        switch (token->type) {
            case TK_LEFT_PARENTHESIS: {
                token = vectorList->contents[*index];

                if (token->type == TK_RIGHT_PARENTHESIS) {
                    (*index)++;
                    break;
                }

                directDeclaratorNode2->conditionalExpressionNode = makeConditionalExpressionNode(vectorList, index);

                if (directDeclaratorNode2->conditionalExpressionNode == NULL) {
                    printf("error: could not create constant-expression-type node.\n");
                    return NULL;
                }

                token = vectorList->contents[*index];

                if (token->type != TK_RIGHT_PARENTHESIS) {
                    printf("error: invalid symbol, expected a ']'.\n");
                    return NULL;
                }

                (*index)++;
                break;
            }
            case TK_LEFT_SQUARE: {
                if (isDeclarationSpecifier(vectorList, *index)) {
                    directDeclaratorNode2->parameterTypeListNode = makeParameterTypeListNode(vectorList, index);

                    if (directDeclaratorNode2->parameterTypeListNode == NULL) {
                        printf("Failed to create parameter-type-list node.\n");
                        return NULL;
                    }

                    token = vectorList->contents[*index];

                    if (token->type != TK_RIGHT_SQUARE) {
                        printf("error: invalid symbol, expected a ']'.\n");
                        return NULL;
                    }

                    (*index)++;
                } 
                else {
                    token = vectorList->contents[*index];

                    while (token->type == TK_IDENTIFIER) {
                        char* identifier = strdup(token->string);
                        pushVector(directDeclaratorNode2->identifierList, identifier);

                        token = vectorList->contents[++*index];
                    }

                    if (token->type != TK_RIGHT_SQUARE) {
                        printf("error: invalid symbol, expected a ']'.\n");
                        return NULL;
                    }

                    (*index)++;
                }

                break;
            }
            default: {
                break;
            }
        }

        token = vectorList->contents[*index];
        current = directDeclaratorNode2;
    }

    return current;
}

DeclaratorNode* makeDeclaratorNode(const Vector* vectorList, int* index) {
    DeclaratorNode* declaratorNode = calloc(1, sizeof(DeclaratorNode));
    const Token* token = vectorList->contents[*index];

    if (token->type == TK_ASTERISK) {
        declaratorNode->pointerNode = makePointerNode(vectorList, index);
    }

    declaratorNode->directDeclaratorNode = makeDirectDeclaratorNode(vectorList, index);

    if (declaratorNode->directDeclaratorNode == NULL) {
        printf("error: could not create direct-declarator-type node.\n");
        return NULL;
    }

    return declaratorNode;
}

FlockDeclarationNode* makeFlockDeclarationNode(const Vector* vectorList, int* index) {
    FlockDeclarationNode* flockDeclarationNode = calloc(1, sizeof(FlockDeclarationNode));  
    flockDeclarationNode->specifierQualifierNodes = buildVectorList();
    
    const Token* token = vectorList->contents[*index];

    if (isTypeSpecifier(vectorList, *index) || token->type == TK_CONSTANT) {
        while (isTypeSpecifier(vectorList, *index) || token->type == TK_CONSTANT) {
            SpecifierQualifierNode* specifierQualifierNode = makeSpecifierQualifierNode(vectorList, index);

            if (specifierQualifierNode == NULL) {
                printf("error: could not create specifier-qualifier-type node.\n");
                return NULL;
            }
        
            pushVector(flockDeclarationNode->specifierQualifierNodes, specifierQualifierNode);
            token = vectorList->contents[*index];
        }
    } 

    token = vectorList->contents[*index];

    if (token->type == TK_ASTERISK) {
        flockDeclarationNode->pointerNode = makePointerNode(vectorList, index);

        if (flockDeclarationNode->pointerNode == NULL) {
            printf("error: could not create pointer-type node.\n");
            return NULL;
        }
    } 

    token = vectorList->contents[*index];

    if (token->type != TK_IDENTIFIER) {
        printf("error: invalid symbol, expected an IDENTIFIER.\n");
        return NULL;
    }

    flockDeclarationNode->identifier = strdup(token->string);
    token = vectorList->contents[++*index];

    if (token->type != TK_SEMICOLON) {
        printf("error: invalid symbol, expected a ';'.\n");
        return NULL;
    }

    (*index)++;
    return flockDeclarationNode;
}

DeclarationSpecifierNode* makeDeclarationSpecifierNode(const Vector* vectorList, int* index) {
    DeclarationSpecifierNode* declarationSpecifierNode = calloc(1, sizeof(DeclarationSpecifierNode));
    const Token* token = vectorList->contents[*index];

    if (token->type == TK_CONSTANT) {
        declarationSpecifierNode->isConstant = true;
        (*index)++;
    }
    else if (isTypeSpecifier(vectorList, *index)) {
        declarationSpecifierNode->typeSpecifierNode = makeTypeSpecifierNode(vectorList, index);

        if (declarationSpecifierNode->typeSpecifierNode == NULL) {
            printf("error: could not create type-specifier-type node.\n");
            return NULL;
        }
    }

    return declarationSpecifierNode;
}

DeclarationNode* makeDeclarationNode(const Vector* vectorList, int* index) {
    DeclarationNode* declarationNode = malloc(sizeof(DeclarationNode));
    declarationNode->declarationSpecifierNodes = buildVectorList();
    declarationNode->initializeDeclaratorNodes = buildVectorList();

    const Token* token = vectorList->contents[*index];

    while (isDeclarationSpecifier(vectorList, *index) && token->type != TK_SEMICOLON) {
        DeclarationSpecifierNode* declarationSpecifierNode = makeDeclarationSpecifierNode(vectorList, index);

        if (declarationSpecifierNode == NULL) {
            printf("error: could not create declaration-specifier-type node.\n");
            return NULL;
        }

        pushVector(declarationNode->declarationSpecifierNodes, declarationSpecifierNode);
    }

    token = vectorList->contents[*index];

    while (token->type != TK_SEMICOLON) {
        InitializeDeclaratorNode* initializeDeclaratorNode = makeInitializingDeclaratorNode(vectorList, index);

        if (initializeDeclaratorNode == NULL) {
            printf("error: could not create initialize-declarator-type node.\n");
            return NULL;
        }

        pushVector(declarationNode->initializeDeclaratorNodes, initializeDeclaratorNode);

        token = vectorList->contents[*index];

        if (token->type == TK_COMMA) {
            token = vectorList->contents[++*index];
        } 
    }

    (*index)++;
    return declarationNode;
}

bool isDeclarationSpecifier(const Vector* vectorList, int index) {
    const Token* token = vectorList->contents[index];
    const int type = token->type;

    return(isTypeSpecifier(vectorList, index) || type == TK_CONSTANT);
}

ExternalDeclarationNode* makeExternalDeclarationNode(const Vector* vectorList, int* index) {
    ExternalDeclarationNode* externalDeclarationNode = calloc(1, sizeof(ExternalDeclarationNode));
    const Token* token = vectorList->contents[*index];

    if (token->type == TK_CLASS) {
        token = vectorList->contents[++*index];

        if (token->type != TK_FLOCK) {
            printf("error: invalid symbol, expected a FLOCK.\n");
            return NULL;
        }

        token = vectorList->contents[++*index];

        if (token->type != TK_IDENTIFIER) {
            printf("error: invalid symbol, expected an IDENTIFIER.\n");
            return NULL;
        }

        char* flockName = strdup(token->string);

        token = vectorList->contents[++*index];

        if (token->type != TK_IDENTIFIER) {
            printf("error: invalid symbol, expected an IDENTIFIER.\n");
            return NULL;
        }

        char* className = strdup(token->string);

        (*index)++;
        appendStringMap(classMap, className, flockName); 
    }
    else if (token->type == TK_GAGGLE) {
        externalDeclarationNode->gaggleSpecifierNode = makeGaggleSpecifierNode(vectorList, index); 

        if (externalDeclarationNode->gaggleSpecifierNode == NULL) {
            printf("error: could not create enum-specifier-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];

        if (token->type != TK_SEMICOLON) {
            printf("error: invalid symbol, expected a ';'.\n");
            return NULL;
        }

        (*index)++;
    }
    else if (isFunctionDefinition(vectorList, *index)) {
        externalDeclarationNode->functionDefinitionNode = makeFunctionDefinitionNode(vectorList, index);

        if (externalDeclarationNode->functionDefinitionNode == NULL) {
            printf("error: could not create function-definition-type node.\n");
            return NULL;
        }
    }
    else if (isFunctionDeclaration(vectorList, *index)) {
        token = vectorList->contents[*index];
        printf("%s\n", token->string);

        while (token->type != TK_SEMICOLON) {
            token = vectorList->contents[++*index];
        }

        (*index)++;
    }
    else {
        externalDeclarationNode->declarationNode = makeDeclarationNode(vectorList, index);

        if (externalDeclarationNode->declarationNode == NULL) {
            printf("error: could not create declaration-definition-type node.\n");
            return NULL;
        }
    } 

    return externalDeclarationNode;
}

InitializeDeclaratorNode* makeInitializingDeclaratorNode(const Vector* vectorList, int* index) {
    InitializeDeclaratorNode* initializeDeclaratorNode = calloc(1, sizeof(InitializeDeclaratorNode));
    initializeDeclaratorNode->declaratorNode = makeDeclaratorNode(vectorList, index);

    if (initializeDeclaratorNode->declaratorNode == NULL) {
        printf("error: could not create declarator-type node.\n");
        return NULL;
    }

    const Token* token = vectorList->contents[*index];

    if (token->type == TK_ASSIGNMENT) {
        (*index)++;

        initializeDeclaratorNode->initializerNode = makeInitializerNode(vectorList, index);

        if (initializeDeclaratorNode->initializerNode == NULL) {
            printf("error: could not create initializer-type node.\n");
            return NULL;
        }
    }

    return initializeDeclaratorNode;
}