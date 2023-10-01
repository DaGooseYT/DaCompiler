#include "parser.h"

ConstantNode* makeConstantNode(const Vector* vectorList, int* index) {
    ConstantNode* constantNode = calloc(1, sizeof(ConstantNode));
    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_NUMBER: {
            constantNode->constantType = CONSTANT_SI32;
            constantNode->integerConstant = token->num;
            break;
        }
        case TK_TRUE: {
            constantNode->constantType = CONSTANT_SI32;
            constantNode->integerConstant = 1;
            break;
        }
        case TK_FALSE: {
            constantNode->constantType = CONSTANT_SI32;
            constantNode->integerConstant = 0;
            break;
        }
        case TK_BYTE: {
            constantNode->constantType = CONSTANT_BYTE;
            constantNode->integerConstant = token->num;
            break;
        }
        case TK_STRING: {
            constantNode->constantType = CONSTANT_STRING;
            constantNode->characterConstant = strdup(token->string);
            break;
        }
        default: {
            printf("error: invalid symbol, expected a CONSTANT.\n");
            return NULL;
        }
    }

    (*index)++;
    return constantNode;
}

TypeNameNode* makeTypeNameNode(const Vector* vectorList, int* index) {
    TypeNameNode* typeNameNode = calloc(1, sizeof(TypeNameNode));
    typeNameNode->specifierQualifierNode = makeSpecifierQualifierNode(vectorList, index);

    if (typeNameNode->specifierQualifierNode == NULL) {
        printf("error: could not create specifier-qualifier-type node.\n");
        return NULL;
    }

    const Token* token = vectorList->contents[*index];

    if (token->type == TK_ASTERISK) {
        typeNameNode->isPointer = true;
        (*index)++;
    }

    return typeNameNode;
}

InitializerListNode* makeInitializerListNode(const Vector* vectorList, int* index) {
    InitializerListNode* initializerListNode = malloc(sizeof(InitializerListNode));
    initializerListNode->initializerNodes = buildVectorList();

    InitializerNode* initializerNode = makeInitializerNode(vectorList, index);

    if (initializerNode == NULL) {
       printf("error: could not create initializer-type node.\n");
       return NULL;
    }

    pushVector(initializerListNode->initializerNodes, initializerNode);

    const Token* token = vectorList->contents[*index];

    while (token->type == TK_COMMA) {
        (*index)++;

        InitializerNode* node = makeInitializerNode(vectorList, index);

        if (initializerNode == NULL) {
            printf("error: could not create initializer-type node.\n");
            return NULL;
        }

        pushVector(initializerListNode->initializerNodes, node);
        token = vectorList->contents[*index];
    }

    return initializerListNode;
}

InitializerNode* makeInitializerNode(const Vector* vectorList, int* index) {
    InitializerNode* initializerNode = calloc(1, sizeof(InitializerNode));
    const Token* token = vectorList->contents[*index];

    if (token->type == TK_LEFT_BRACKET) {
        (*index)++; 

        initializerNode->initializerListNode = makeInitializerListNode(vectorList, index);

        if (initializerNode->initializerListNode == NULL) {
            printf("error: could not create initializer-list-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];

        if (token->type != TK_RIGHT_BRACKET) {
            printf("error: Invalid symbol, expected a ']'.\n");
            return NULL;
        }

        (*index)++; 
    }
    else {
        initializerNode->assignmentExpressionNode = makeAssignmentExpressionNode(vectorList, index);

        if (initializerNode->assignmentExpressionNode == NULL) {
            printf("error: could not create assignment-expression-type node.\n");
            return NULL;
        }
    }

    return initializerNode;
}

ParameterListNode* makeParameterListNode(const Vector* vectorList, int* index) {
    ParameterListNode* paramListNode1 = calloc(1, sizeof(ParameterListNode));
    
    paramListNode1->parameterDeclarationNode = makeParameterDeclarationNode(vectorList, index);

    if (paramListNode1->parameterDeclarationNode == NULL) {
        printf("error: could not create parameter-declaration-type node.\n");
        return NULL;
    }
    
    ParameterListNode* current = paramListNode1;
    const Token* token = vectorList->contents[*index];

    while (token->type == TK_COMMA) {
        (*index)++;

        ParameterListNode* paramListNode2 = calloc(1, sizeof(ParameterListNode));
        paramListNode2->parameterListNode = current;
        paramListNode2->parameterDeclarationNode = makeParameterDeclarationNode(vectorList, index);

        if (paramListNode2->parameterDeclarationNode == NULL) {
            printf("error: could not create parameter-declaration-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];
        current = paramListNode2;
    }

    return current;
}

ParameterTypeListNode* makeParameterTypeListNode(const Vector* vectorList, int* index) {
    ParameterTypeListNode* parameterTypeListNode = calloc(1, sizeof(ParameterTypeListNode));
    parameterTypeListNode->parameterListNode = makeParameterListNode(vectorList, index);

    if (parameterTypeListNode->parameterListNode == NULL) {
        printf("error: could not create parameter-type-list-type node.\n");
        return NULL;
    }

    const Token* token = vectorList->contents[*index];

    if (token->type == TK_COMMA) {
        token = vectorList->contents[++*index];
        (*index)++;
    }

    return parameterTypeListNode;
}

PointerNode* makePointerNode(const Vector* vectorList, int* index) {
    PointerNode* pointerNode = malloc(sizeof(PointerNode));
    pointerNode->count = 1;

    const Token* token = vectorList->contents[*index];

    if (token->type != TK_ASTERISK) {
        printf("error: Invalid symbol, expected a '*'.\n");
        return NULL;
    }

    token = vectorList->contents[++*index];

    while (token->type == TK_ASTERISK) {
        pointerNode->count++;
        token = vectorList->contents[++*index];
    } 
    
    return pointerNode;  
}

SpecifierQualifierNode* makeSpecifierQualifierNode(const Vector* vectorList, int* index) {
    SpecifierQualifierNode* specifierQualifierNode = calloc(1, sizeof(SpecifierQualifierNode));
    const Token* token = vectorList->contents[*index];

    if (isTypeSpecifier(vectorList, *index)) {
        specifierQualifierNode->typeSpecifierNode = makeTypeSpecifierNode(vectorList, index);

        if (specifierQualifierNode->typeSpecifierNode == NULL) {
            printf("error: could not create type-specifier-type node.\n");
            return NULL;
        }
    } 
    else if (token->type == TK_CONSTANT) {
        (*index)++;
        specifierQualifierNode->isConstant = true;
    }
    else {
        printf("error: invalid symbol, expected a SPECIFIER type.\n");
        return NULL;
    }

    return specifierQualifierNode;
}

FlockSpecifierNode* makeFlockSpecifierNode(const Vector* vectorList, int* index) {
    FlockSpecifierNode* flockSpecifierNode = malloc(sizeof(FlockSpecifierNode));
    flockSpecifierNode->flockDeclarationNodes = buildVectorList();

    const Token* token = vectorList->contents[*index];

    if (token->type != TK_FLOCK) {
        printf("error: invalid symbol, expected a FLOCK.\n");
        return NULL;
    }

    token = vectorList->contents[++*index];

    if (token->type != TK_IDENTIFIER) {
        printf("error: invalid symbol, expected an IDENTIFIER.\n");
        return NULL;
    }

    (*index)++;
    flockSpecifierNode->identifier = strdup(token->string);
    token = vectorList->contents[*index];

    if (token->type == TK_LEFT_BRACKET) {
        token = vectorList->contents[++*index];

        while (token->type != TK_RIGHT_BRACKET) {
            FlockDeclarationNode* flockDeclarationNode = makeFlockDeclarationNode(vectorList, index);

            if (flockDeclarationNode == NULL) {
                printf("error: could not create flock-declaration-type node.\n");
                return NULL;
            }

            pushVector(flockSpecifierNode->flockDeclarationNodes, flockDeclarationNode);
            token = vectorList->contents[*index];
        }

        (*index)++; 
    }

    return flockSpecifierNode;
}

GaggleListNode* makeGaggleListNode(const Vector* vectorList, int* index) {
    GaggleListNode* gaggleListNode = malloc(sizeof(GaggleListNode));
    gaggleListNode->identifiers = buildVectorList();

    const Token* token = vectorList->contents[*index];

    while (true) {
        if (token->type != TK_IDENTIFIER) {
            printf("error: invalid symbol, expected an IDENTIFIER.\n");
            return NULL;
        }

        char* identifier = strdup(token->string);
        pushVector(gaggleListNode->identifiers, identifier);

        token = vectorList->contents[++*index];

        if (token->type == TK_COMMA) {
           token = vectorList->contents[++*index];
        }

        if (token->type == TK_RIGHT_BRACKET) {
            break;
        }
    }

    return gaggleListNode;
}

GaggleSpecifierNode* makeGaggleSpecifierNode(const Vector* vectorList, int* index) {
    GaggleSpecifierNode* gaggleSpecifierNode = calloc(1, sizeof(GaggleSpecifierNode));
    const Token* token = vectorList->contents[*index];

    if (token->type != TK_GAGGLE) {
        printf("error: invalid symbol, expected a GAGGLE.\n");
        return NULL;
    }

    token = vectorList->contents[++*index];

    if (token->type == TK_IDENTIFIER) {
        gaggleSpecifierNode->identifier = strdup(token->string);
        (*index)++;
    }

    token = vectorList->contents[*index];

    if (token->type != TK_LEFT_BRACKET) {
        printf("error: invalid symbol, expected a '['.\n");
        return NULL;
    }

    (*index)++;
    gaggleSpecifierNode->gaggleListNode = makeGaggleListNode(vectorList, index);

    if (gaggleSpecifierNode->gaggleListNode == NULL) {
        printf("error: could not create enumerator-list-type node.\n");
        return NULL;
    }

    token = vectorList->contents[*index];

    if (token->type != TK_RIGHT_BRACKET) {
        printf("error: invalid symbol, expected a ']'.\n");
        return NULL;
    }

    (*index)++;
    return gaggleSpecifierNode;
}

TypeSpecifierNode* makeTypeSpecifierNode(const Vector* vectorList, int* index) {
    TypeSpecifierNode* typeSpecifierNode = calloc(1, sizeof(TypeSpecifierNode));
    typeSpecifierNode->typeSpecifier = TYPE_NONE;

    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_UI0: {
            typeSpecifierNode->typeSpecifier = TYPE_UI0;
            (*index)++;
            break;
        }
        case TK_SC8: {
            typeSpecifierNode->typeSpecifier = TYPE_SC8;
            (*index)++;
            break;
        }
        case TK_SI32: {
            typeSpecifierNode->typeSpecifier = TYPE_SI32;
            (*index)++;
            break;
        }
        case TK_FP64: {
            typeSpecifierNode->typeSpecifier = TYPE_FP64;
            (*index)++;
            break;
        }
        case TK_B8: {
            typeSpecifierNode->typeSpecifier = TYPE_SI32;
            (*index)++;
            break;
        }
        case TK_FLOCK: {
            typeSpecifierNode->typeSpecifier = TYPE_FLOCK;
            typeSpecifierNode->flockSpecifierNode = makeFlockSpecifierNode(vectorList, index);

            if (typeSpecifierNode->flockSpecifierNode == NULL) {
                printf("error: could not create flock-specifier-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_IDENTIFIER: { 
            typeSpecifierNode->typeSpecifier = TYPENAME;
            typeSpecifierNode->flockName = getStringMap(classMap, token->string);

            if (typeSpecifierNode->flockName == NULL) {
                printf("error: invalid symbol, expected an IDENTIFIER.\n");
                return NULL;
            }

            (*index)++;
            break;
        }
        default: {
            printf("error: invalid symbol, expected a TYPE specifier.\n");
            return NULL;
        }
    }

    return typeSpecifierNode;
}

bool isTypeSpecifier(const Vector* vectorList, int index) {
    const Token* token = vectorList->contents[index];
    const int type = token->type;

    return(type == TK_UI0 || type == TK_SC8 || type == TK_SI32 || type == TK_FP64 || type == TK_B8 || type == TK_FLOCK || (token->string != NULL && stringMapContains(classMap, token->string)));
}

BlockItemNode* makeBlockItemNode(const Vector* vectorList, int* index) {
    BlockItemNode* blockItemNode = calloc(1, sizeof(BlockItemNode));

    if (isDeclarationSpecifier(vectorList, *index)) {
        blockItemNode->declarationNode = makeDeclarationNode(vectorList, index);

        if (blockItemNode->declarationNode == NULL) {
            printf("error: could not create decalratiaon-type node.\n");
            return NULL;
        }
    }
    else {
        blockItemNode->statementNode = makeStatementNode(vectorList, index);

        if (blockItemNode->statementNode == NULL) {
            printf("error: could not create statement-type node.\n");
            return NULL;
        }
    }

    return blockItemNode;
}

FunctionDefinitionNode* makeFunctionDefinitionNode(const Vector* vectorList, int* index) {
    FunctionDefinitionNode* functionDefinitionNode = calloc(1, sizeof(FunctionDefinitionNode));
    functionDefinitionNode->declarationSpecifierNodes = buildVectorList();

    while (isDeclarationSpecifier(vectorList, *index)) {
        DeclarationSpecifierNode* declarationSpecifierNode = makeDeclarationSpecifierNode(vectorList, index);

        if (declarationSpecifierNode == NULL) {
            printf("error: could not create declaration-specifier-type node.\n");
            return NULL;
        }
        
        pushVector(functionDefinitionNode->declarationSpecifierNodes, declarationSpecifierNode);
    }

    functionDefinitionNode->declaratorNode = makeDeclaratorNode(vectorList, index);

    if (functionDefinitionNode->declaratorNode == NULL) {
        printf("error: could not create declarator-type node.\n");
        return NULL;
    }

    functionDefinitionNode->compoundStatementNode = makeCompoundStatementNode(vectorList, index);

    if (functionDefinitionNode->compoundStatementNode == NULL) {
        printf("error: could not create compound-statement-type node\n");
        return NULL;
    }

    return functionDefinitionNode;
}

bool isFunctionDefinition(const Vector* vectorList, int index) {
    const Token* token = vectorList->contents[index];

    while (!(token->type == TK_IDENTIFIER && !stringMapContains(classMap, token->string))) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];

    if (token->type != TK_LEFT_SQUARE) {
        return false;
    }

    while (token->type != TK_RIGHT_SQUARE) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];
    return (token->type == TK_LEFT_BRACKET);
}

bool isFunctionDeclaration(const Vector* vectorList, int index) {
    const Token* token = vectorList->contents[index];

    while (!(token->type == TK_IDENTIFIER && !stringMapContains(classMap, token->string))) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];

    if (token->type != TK_LEFT_SQUARE) {
        return false;
    }

    while (token->type != TK_RIGHT_SQUARE) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];
    return (token->type == TK_SEMICOLON);
}

TransUnitNode* makeTransUnitNode() {
    TransUnitNode* transUnitNode = malloc(sizeof(TransUnitNode));
    transUnitNode->externalDeclarationNodes = buildVectorList();
    return transUnitNode;
}

TransUnitNode* parse(const Vector* vectorList) {
    classMap = buildStringMap(1024);
    int index = 0;

    TransUnitNode* transUnitNode = makeTransUnitNode();

    while (index < vectorList->size) {
        ExternalDeclarationNode* externalDeclarationNode = makeExternalDeclarationNode(vectorList, &index);

        if (externalDeclarationNode == NULL) {
            printf("error: could not create external-declaration-type node.\n");
            return NULL;
        }

        pushVector(transUnitNode->externalDeclarationNodes, externalDeclarationNode);
    }

    return transUnitNode;
}