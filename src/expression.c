#include "parser.h"

UnaryExpressionNode* makeUnaryExpressionNode(const Vector* vectorList, int* index) {
    UnaryExpressionNode* unaryExpressionNode = calloc(1, sizeof(UnaryExpressionNode));
    unaryExpressionNode->type = UNARY_NONE;

    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_INCREMENT: {
            unaryExpressionNode->type = UNARY_INCREMENT;
            (*index)++;
            unaryExpressionNode->unaryExpressionNode = makeUnaryExpressionNode(vectorList, index);

            if (unaryExpressionNode->unaryExpressionNode == NULL) {
                printf("error: could not create unary-expression-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_DECREMENT: {
            unaryExpressionNode->type = UNARY_DECREMENT;
            (*index)++;
            unaryExpressionNode->unaryExpressionNode = makeUnaryExpressionNode(vectorList, index);

            if (unaryExpressionNode->unaryExpressionNode == NULL) {
                printf("error: could not create unary-expression-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_SIZE: {
            token = vectorList->contents[++*index];

            if (token->type != TK_LEFT_SQUARE) {
                printf("error: invalid symbol, expected a '['.\n");
                return NULL;
            }

            token = vectorList->contents[++*index];

            if (token->type == TK_IDENTIFIER && !stringMapContains(classMap, token->string)) {
                unaryExpressionNode->type = UNARY_SIZE_IDENTIFIER;
                unaryExpressionNode->sizeName = strdup(token->string);
                (*index)++;
            }
            else {
                unaryExpressionNode->type = UNARY_SIZE_TYPE;
                unaryExpressionNode->typeNameNode = makeTypeNameNode(vectorList, index);

                if (unaryExpressionNode->typeNameNode == NULL) {
                    printf("error: could not create type-name-type node.\n");
                    return NULL;
                }
            }

            token = vectorList->contents[*index];

            if (token->type != TK_RIGHT_SQUARE) {
                printf("error: invalid symbol, expected a ']'.\n");
                return NULL;
            }

            (*index)++;
            break;
        }
        case TK_AMP:
        case TK_ASTERISK:
        case TK_PLUS:
        case TK_MINUS:
        case TK_EXCLAMATION: {
            unaryExpressionNode->type = UNARY_OPERATOR;

            if (token->type == TK_AMP) {
                unaryExpressionNode->operatorType = OPERATOR_AND;
            }
            else if (token->type == TK_ASTERISK) {
                unaryExpressionNode->operatorType = OPERATOR_MULTIPLY;
            }
            else if (token->type == TK_PLUS) {
                unaryExpressionNode->operatorType = OPERATOR_ADD;
            }
            else if (token->type == TK_MINUS) {
                unaryExpressionNode->operatorType = OPERATOR_SUBTRACT;
            }
            else if (token->type == TK_EXCLAMATION) {
                unaryExpressionNode->operatorType = OPERATOR_EXCLAMATION;
            }

            (*index)++;
            unaryExpressionNode->castExpressionNode = makeCastExpressionNode(vectorList, index);

            if (unaryExpressionNode->castExpressionNode == NULL) {
                printf("error: could not create cast-expression-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_EXTERNAL: {
            token = vectorList->contents[++*index];

            if (token->type != TK_SCOPE) {
                printf("error: invalid symbol, expected a '::'.\n");
                return NULL;
            }

            token = vectorList->contents[++*index];
            isExternFunction = true;
            
            unaryExpressionNode->type = UNARY_NONE;
            unaryExpressionNode->postfixExpressionNode = makePostfixExpressionNode(vectorList, index);

            if (unaryExpressionNode->postfixExpressionNode == NULL) {
                printf("error: could not create postfix-expression-type node.\n");
                return NULL;
            }

            break;
        }
        default: {
            unaryExpressionNode->type = UNARY_NONE;
            unaryExpressionNode->postfixExpressionNode = makePostfixExpressionNode(vectorList, index);

            if (unaryExpressionNode->postfixExpressionNode == NULL) {
                printf("error: could not create postfix-expression-type node.\n");
                return NULL;
            }

            break;
        }
    }

    return unaryExpressionNode;
}

CastExpressionNode* makeCastExpressionNode(const Vector* vectorList, int* index) {
    CastExpressionNode* castExpressionNode = calloc(1, sizeof(CastExpressionNode));
    castExpressionNode->unaryExpressionNode = makeUnaryExpressionNode(vectorList, index);

    if (castExpressionNode->unaryExpressionNode == NULL) {
        printf("error: could not create unary-expression-type node.\n");
        return NULL;
    }

    return castExpressionNode;
}

MultiplicationExpressionNode* makeMultiplicationExpressionNode(const Vector* vectorList, int* index) {
    MultiplicationExpressionNode* multiplicationExpressionNode1 = calloc(1, sizeof(MultiplicationExpressionNode));
    multiplicationExpressionNode1->operatorType = OPERATOR_NONE;
    multiplicationExpressionNode1->castExpressionNode = makeCastExpressionNode(vectorList, index);

    if (multiplicationExpressionNode1->castExpressionNode == NULL) {
        printf("error: could not create cast-expression-type node.\n");
        return NULL;
    }

    MultiplicationExpressionNode* current = multiplicationExpressionNode1;
    const Token* token = vectorList->contents[*index];

    while (token->type == TK_ASTERISK || token->type == TK_SLASH || token->type == TK_PERCENT) {
        (*index)++;

        MultiplicationExpressionNode* multiplicationExpressionNode2 = malloc(sizeof(MultiplicationExpressionNode));
        multiplicationExpressionNode2->multiplicationExpressionNode = current;
        multiplicationExpressionNode2->castExpressionNode = makeCastExpressionNode(vectorList, index);

        if (multiplicationExpressionNode2->castExpressionNode == NULL) {
            printf("error: could not create cast-expression-type node.\n");
            return NULL;
        }

        if (token->type == TK_ASTERISK) {
            multiplicationExpressionNode2->operatorType = OPERATOR_MULTIPLY;
        }
        else if (token->type == TK_SLASH) {
            multiplicationExpressionNode2->operatorType = OPERATOR_DIVIDE;
        }
        else if (token->type == TK_PERCENT) {
            multiplicationExpressionNode2->operatorType = OPERATOR_MODULO;
        }

        token = vectorList->contents[*index];
        current = multiplicationExpressionNode2;
    }

    return current;
}

AdditionExpressionNode* makeAdditiveExpressionNode(const Vector* vectorList, int* index) {
    AdditionExpressionNode* additionExpressionNode1 = calloc(1, sizeof(AdditionExpressionNode));

    additionExpressionNode1->operatorType = OPERATOR_NONE;
    additionExpressionNode1->multiplicationExpressionNode = makeMultiplicationExpressionNode(vectorList, index);

    if (additionExpressionNode1->multiplicationExpressionNode == NULL) {
        printf("error: could not create multiplication-expression-type node.\n");
        return NULL;
    }

    AdditionExpressionNode* current = additionExpressionNode1;
    const Token* token = vectorList->contents[*index];

    while (token->type == TK_PLUS || token->type == TK_MINUS) {
        (*index)++;

        AdditionExpressionNode* additionExpressionNode2 = malloc(sizeof(AdditionExpressionNode));
        additionExpressionNode2->operatorType = (token->type == TK_PLUS) ? OPERATOR_ADD: OPERATOR_SUBTRACT;
        additionExpressionNode2->additionExpressionNode = current;
        additionExpressionNode2->multiplicationExpressionNode = makeMultiplicationExpressionNode(vectorList, index);

        if (additionExpressionNode2->multiplicationExpressionNode == NULL) {
            printf("error: could not create multiplication-expression-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];
        current = additionExpressionNode2;
    }

    return current;
}

ShiftExpressionNode* makeShiftExpression(const Vector* vectorList, int* index) {
    ShiftExpressionNode* shiftExpressionNode = calloc(1, sizeof(ShiftExpressionNode));
    shiftExpressionNode->additionExpressionNode = makeAdditiveExpressionNode(vectorList, index);

    if (shiftExpressionNode->additionExpressionNode == NULL) {
        printf("error: could not additive-expression-type node.\n");
        return NULL;
    }

    return shiftExpressionNode;
}

RelationalExpressionNode* makeRelationalExpressionNode(const Vector* vectorList, int* index) {
    RelationalExpressionNode* relationalExpressionNode = calloc(1, sizeof(RelationalExpressionNode));
    relationalExpressionNode->compareType = COMPARE_NONE;
    relationalExpressionNode->shiftExpressionNode = makeShiftExpression(vectorList, index);

    if (relationalExpressionNode->shiftExpressionNode == NULL) {
        printf("error: could not create shift-expression-type node.\n");
        return NULL;
    }
    
    RelationalExpressionNode* current = relationalExpressionNode;
    const Token* token = vectorList->contents[*index];

    while (token->type == TK_LEFT_ANGLE || token->type == TK_RIGHT_ANGLE || token->type == TK_LESS_OR_EQUAL || token->type == TK_GREATER_OR_EQUAL) {
        (*index)++;
    
        RelationalExpressionNode* parent = calloc(1, sizeof(EqualExpressionNode));
        parent->relationalExpressionNode = current;
        parent->shiftExpressionNode = makeShiftExpression(vectorList, index);

        if (parent->shiftExpressionNode == NULL) {
            printf("error: could not create shif-texpression-type node.\n");
            return NULL;
        }

        if (token->type == TK_LEFT_ANGLE) {
            parent->compareType = COMPARE_LESS_THAN;
        }
        else if (token->type == TK_RIGHT_ANGLE) {
            parent->compareType = COMPARE_GREATER_THAN;
        }
        else if (token->type == TK_LESS_OR_EQUAL) {
            parent->compareType = COMPARE_LESS_OR_EQUAL;
        }
        else if (token->type == TK_GREATER_OR_EQUAL) {
            parent->compareType = COMPARE_GREATER_OR_EQUAL;
        }
     
        token = vectorList->contents[*index];
        current = parent;
    }

    return current;
}

EqualExpressionNode* makeEqualExpressionNode(const Vector* vectorList, int* index) {
    EqualExpressionNode* equalExpressionNode1 = calloc(1, sizeof(EqualExpressionNode));
    equalExpressionNode1->compareType = COMPARE_NONE;
    equalExpressionNode1->relationalExpressionNode = makeRelationalExpressionNode(vectorList, index);

    if (equalExpressionNode1->relationalExpressionNode == NULL) {
        printf("error: could not create relational-expression-type node.\n");
        return NULL;
    }

    EqualExpressionNode* current = equalExpressionNode1;
    const Token* token = vectorList->contents[*index];

    while (token->type == TK_EQUAL || token->type == TK_NOT_EQUAL) {
        (*index)++;
    
        EqualExpressionNode* equalExpressionNode2 = calloc(1, sizeof(EqualExpressionNode));
        equalExpressionNode2->equalExpressionNode = current;
        equalExpressionNode2->relationalExpressionNode = makeRelationalExpressionNode(vectorList, index);

        if (equalExpressionNode1->relationalExpressionNode == NULL) {
            printf("error: could not create relational-expression-type node.\n");
            return NULL;
        }

        if (token->type == TK_EQUAL) {
            equalExpressionNode2->compareType = COMPARE_EQUAL;
        }
        else if (token->type == TK_NOT_EQUAL) {
            equalExpressionNode2->compareType = COMPARE_NOT_EQUAL;
        }

        token = vectorList->contents[*index];
        current = equalExpressionNode2;
    }

    return current;
}

AndExpressionNode* makeAndExpressionNode(const Vector* vectorList, int* index) {
    AndExpressionNode* andExpressionNode = calloc(1, sizeof(AndExpressionNode));
    andExpressionNode->equalExpressionNode = makeEqualExpressionNode(vectorList, index);

    if (andExpressionNode->equalExpressionNode == NULL) {
        printf("error: could not create equality-expression-type node.\n");
        return NULL;
    }

    return andExpressionNode;
}

ExclusiveOrExpressionNode* makeExclusiveOrExpressionNode(const Vector* vectorList, int* index) {
    ExclusiveOrExpressionNode* exclusiveOrExpressionNode = calloc(1, sizeof(ExclusiveOrExpressionNode));
    exclusiveOrExpressionNode->andExpressionNode = makeAndExpressionNode(vectorList, index);

    if (exclusiveOrExpressionNode->andExpressionNode == NULL) {
        printf("error: could not create and-expression-type node.\n");
        return NULL;
    }

    return exclusiveOrExpressionNode;
}

InclusiveOrExpressionNode* makeInclusiveOrExpressionNode(const Vector* vectorList, int* index) {
    InclusiveOrExpressionNode* inclusiveOrExpressionNode = malloc(sizeof(InclusiveOrExpressionNode));
    inclusiveOrExpressionNode->inclusiveOrExpressionNode = NULL;
    inclusiveOrExpressionNode->exclusiveOrExpressionNode = makeExclusiveOrExpressionNode(vectorList, index);

    if (inclusiveOrExpressionNode->exclusiveOrExpressionNode == NULL) {
        printf("error: could not create exclusive-or-expression-type node.\n");
        return NULL;
    }

    return inclusiveOrExpressionNode;
}

LogicalAndExpressionNode* makeLogicalAndExpressionNode(const Vector* vectorList, int* index) {
    LogicalAndExpressionNode* andExpressionNode = calloc(1, sizeof(LogicalAndExpressionNode));
    andExpressionNode->inclusiveOrExpressionNode = makeInclusiveOrExpressionNode(vectorList, index);

    if (andExpressionNode->inclusiveOrExpressionNode == NULL) {
        printf("error: could not create inclusive-or-expression-type node.\n");
        return NULL;
    }

    const Token* token = vectorList->contents[*index];
    LogicalAndExpressionNode* current = andExpressionNode;

    while (token->type == TK_AND) {
        (*index)++;

        LogicalAndExpressionNode* logicalAndExpressionNode = calloc(1, sizeof(LogicalAndExpressionNode));
        logicalAndExpressionNode->logicalAndExpressionNode = current;
        logicalAndExpressionNode->inclusiveOrExpressionNode = makeInclusiveOrExpressionNode(vectorList, index);

        if (logicalAndExpressionNode->inclusiveOrExpressionNode == NULL) {
            printf("error: could not create inclusive-or-expression-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];
        current = logicalAndExpressionNode;
    }

    return current;
}

OrExpressionNode* makeLogicalOrExpressionNode(const Vector* vectorList, int* index) {
    OrExpressionNode* logicalOrExpressionNode1 = calloc(1, sizeof(OrExpressionNode));
    logicalOrExpressionNode1->logicalAndExpressionNode = makeLogicalAndExpressionNode(vectorList, index);

    if (logicalOrExpressionNode1->logicalAndExpressionNode == NULL) {
        printf("error: could not create logical-and-expression-type node.\n");
        return NULL;
    }

    const Token* token = vectorList->contents[*index];
    OrExpressionNode* current = logicalOrExpressionNode1;

    while (token->type == TK_OR) {
        (*index)++;

        OrExpressionNode* logicalOrExpressionNode2 = calloc(1, sizeof(OrExpressionNode));
        logicalOrExpressionNode2->orExpressionNode = current;
        logicalOrExpressionNode2->logicalAndExpressionNode = makeLogicalAndExpressionNode(vectorList, index);

        if (logicalOrExpressionNode2->logicalAndExpressionNode == NULL) {
            printf("error: could not create logical-and-expression-type node.\n");
            return NULL;
        }
        
        token = vectorList->contents[*index];
        current = logicalOrExpressionNode2;
    }

    return current;
}

ConditionalExpressionNode* makeConditionalExpressionNode(const Vector* vectorList, int* index) {
    ConditionalExpressionNode* conditionalExpressionNode = calloc(1, sizeof(ConditionalExpressionNode));
    conditionalExpressionNode->orExpressionNode = makeLogicalOrExpressionNode(vectorList, index);

    if (conditionalExpressionNode->orExpressionNode == NULL) {
        printf("error: could not create logical-or-expression-type node.\n");
        return NULL;
    }

    const Token* token = vectorList->contents[*index];
    if (token->type == TK_QUESTION) {
        (*index)++;
        conditionalExpressionNode->expressionNode = makeExpressionNode(vectorList, index);

        if (conditionalExpressionNode->expressionNode == NULL) {
            printf("error: could not create expression-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];

        if (token->type != TK_COLON) {
            printf("error: invalid symbol, expected a ';'.\n");
            return NULL;
        }

        (*index)++;
        conditionalExpressionNode->conditionalExpressionNode = makeConditionalExpressionNode(vectorList, index);

        if (conditionalExpressionNode->conditionalExpressionNode == NULL) {
            printf("error: could not create conditional-expression-type node.\n");
            return NULL;
        }
    }

    return conditionalExpressionNode;
}

AssignmentExpressionNode* makeAssignmentExpressionNode(const Vector* vectorList, int* index) {
    AssignmentExpressionNode* assignmentExpressionNode = calloc(1, sizeof(AssignmentExpressionNode));
    assignmentExpressionNode->assignOperator = OPERATOR_NONE;

    const int buf = *index;
    assignmentExpressionNode->unaryExpressionNode = makeUnaryExpressionNode(vectorList, index);

    if (assignmentExpressionNode->unaryExpressionNode == NULL) {
        printf("error: could not create unary-expression-type node.\n");
        return NULL;
    }

    const Token* token = vectorList->contents[*index];

    if (token->type == TK_ASSIGNMENT || token->type == TK_MULTIPLY_EQUAL || token->type == TK_DIVIDE_EQUAL || token->type == TK_MODULO_EQUAL || token->type == TK_ADD_EQUAL || token->type == TK_MINUS_EQUAL) {
        if (token->type == TK_ASSIGNMENT) {
            assignmentExpressionNode->assignOperator = OPERATOR_ASSIGN;
        }
        else if (token->type == TK_MODULO_EQUAL) {
            assignmentExpressionNode->assignOperator = OPERATOR_MODULO_EQUAL;
        }
        else if (token->type == TK_MULTIPLY_EQUAL) {
            assignmentExpressionNode->assignOperator = OPERATOR_MULTIPLY_EQUAL;
        }
        else if (token->type == TK_DIVIDE_EQUAL) {
            assignmentExpressionNode->assignOperator = OPERATOR_DIVIDE_EQUAL;
        }
        else if (token->type == TK_ADD_EQUAL) {
            assignmentExpressionNode->assignOperator = OPERATOR_ADD_EQUAL;
        }
        else if (token->type == TK_MINUS_EQUAL) {
            assignmentExpressionNode->assignOperator = OPERATOR_SUBTRACT_EQUAL;
        }

        (*index)++;
        assignmentExpressionNode->assignExpressionNode = makeAssignmentExpressionNode(vectorList, index);

        if (assignmentExpressionNode->assignExpressionNode == NULL) {
            printf("error: could not create assign-expression-type node.\n");
            return NULL;
        }
    } 
    else {
        assignmentExpressionNode->unaryExpressionNode = NULL;
        *index = buf;
        assignmentExpressionNode->conditionalExpressionNode = makeConditionalExpressionNode(vectorList, index);

        if (assignmentExpressionNode->conditionalExpressionNode == NULL) {
            printf("error: could not create conditional-expression-type node.\n");
            return NULL;
        }
    }

    return assignmentExpressionNode;
}

ExpressionNode* makeExpressionNode(const Vector* vectorList, int* index) {
    ExpressionNode* expressionNode1 = calloc(1, sizeof(ExpressionNode));
    expressionNode1->assignExpressionNode = makeAssignmentExpressionNode(vectorList, index);

    if (expressionNode1->assignExpressionNode == NULL) {
        printf("error: could not create assignment-expression-type node.\n");
        return NULL;
    }

    ExpressionNode* current = expressionNode1;
    const Token* token = vectorList->contents[*index];

    while (token->type == TK_COMMA) {
        (*index)++;

        ExpressionNode* expressionNode2 = calloc(1, sizeof(ExpressionNode));
        expressionNode2->expressionNode = current;
        expressionNode2->assignExpressionNode = makeAssignmentExpressionNode(vectorList, index);

        if (expressionNode2->assignExpressionNode == NULL) {
            printf("error: could not create assignment-expression-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];
        current = expressionNode2;
    }

    return current;
}

PostfixExpressionNode* makePostfixExpressionNode(const Vector* vectorList, int* index) {
    PostfixExpressionNode* postfixExpressionNode1 = calloc(1, sizeof(PostfixExpressionNode));
    postfixExpressionNode1->assignExpressionNodes = buildVectorList();
    postfixExpressionNode1->postfixExpressionType = POSTFIX_PRIMARY;
    postfixExpressionNode1->primaryExpressionNode = makePrimaryExpressionNode(vectorList, index);

    if (postfixExpressionNode1->primaryExpressionNode == NULL) {
        printf("error: could not create primary-expression-type node.\n");
        return NULL;
    }

    PostfixExpressionNode* current = postfixExpressionNode1;
    const Token* token = vectorList->contents[*index];

    while (token->type == TK_LEFT_SQUARE || token->type == TK_LEFT_PARENTHESIS || token->type == TK_DOT || token->type == TK_POINTER || token->type == TK_INCREMENT || token->type == TK_DECREMENT) {
        PostfixExpressionNode* postfixExpressionNode = NULL;

        switch (token->type) {
            case TK_LEFT_PARENTHESIS: {
                (*index)++;

                postfixExpressionNode = calloc(1, sizeof(PostfixExpressionNode));
                postfixExpressionNode->postfixExpressionNode = current;
                postfixExpressionNode->assignExpressionNodes = buildVectorList();
                postfixExpressionNode->postfixExpressionType = POSTFIX_LEFT_PARENTHESIS;
                postfixExpressionNode->expressionNode = makeExpressionNode(vectorList, index);

                if (postfixExpressionNode->expressionNode == NULL) {
                    printf("error: could not create assignment-expression-type node.\n");
                    return NULL;
                }

                token = vectorList->contents[*index];

                if (token->type != TK_RIGHT_PARENTHESIS) {
                    printf("error: invalid symbol, expected a ')'.\n");
                    return NULL;    
                }

                (*index)++;
                break;
            }
            case TK_LEFT_SQUARE: {
                (*index)++;

                postfixExpressionNode = calloc(1, sizeof(PostfixExpressionNode));
                postfixExpressionNode->postfixExpressionNode = current;
                postfixExpressionNode->assignExpressionNodes = buildVectorList();
                postfixExpressionNode->postfixExpressionType = POSTFIX_LEFT_SQUARE;

                token = vectorList->contents[*index];

                while (token->type != TK_RIGHT_SQUARE) {
                    AssignmentExpressionNode* assignmentExpressionNode = makeAssignmentExpressionNode(vectorList, index);

                    if (assignmentExpressionNode == NULL) {
                        printf("error: could not create assignment-expression-type node.\n");
                        return NULL;
                    }

                    pushVector(postfixExpressionNode->assignExpressionNodes, assignmentExpressionNode);
                    token = vectorList->contents[*index];

                    if (token->type == TK_COMMA) {
                        token = vectorList->contents[++*index];
                    }
                }

                (*index)++;
                break;
            }
            case TK_DOT: {
                (*index)++;

                postfixExpressionNode = calloc(1, sizeof(PostfixExpressionNode));
                postfixExpressionNode->postfixExpressionNode = current;
                postfixExpressionNode->assignExpressionNodes = buildVectorList();
                postfixExpressionNode->postfixExpressionType = POSTFIX_DOT;

                token = vectorList->contents[*index];

                if (token->type != TK_IDENTIFIER) {
                    printf("error: invalid symbol, expected an IDENTIFIER.\n");
                    return NULL;
                }

                postfixExpressionNode->identifier = strdup(token->string);

                (*index)++;
                break;
            }
            case TK_POINTER: {
                (*index)++;

                postfixExpressionNode = calloc(1, sizeof(PostfixExpressionNode));
                postfixExpressionNode->postfixExpressionNode = current;
                postfixExpressionNode->assignExpressionNodes = buildVectorList();
                postfixExpressionNode->postfixExpressionType = POSTFIX_POINTER;

                token = vectorList->contents[*index];

                if (token->type != TK_IDENTIFIER) {
                    printf("error: invalid symbol, expected an IDENTIFIER.\n");
                    return NULL;
                }

                postfixExpressionNode->identifier = strdup(token->string);

                (*index)++;
                break;
            }
            case TK_INCREMENT: {
                (*index)++;

                postfixExpressionNode = calloc(1, sizeof(PostfixExpressionNode));
                postfixExpressionNode->assignExpressionNodes = buildVectorList();
                postfixExpressionNode->postfixExpressionNode = current;
                postfixExpressionNode->postfixExpressionType = POSTFIX_INCREMENT;
                break;
            }
            case TK_DECREMENT: {
                (*index)++;

                postfixExpressionNode = calloc(1, sizeof(PostfixExpressionNode));
                postfixExpressionNode->assignExpressionNodes = buildVectorList();
                postfixExpressionNode->postfixExpressionNode = current;
                postfixExpressionNode->postfixExpressionType = POSTFIX_DECREMENT;
                break;
            }
            default: {
                printf("error: invalid symbol, expected POSTFIX expression.\n");
                return NULL;
            }
        }

        current = postfixExpressionNode;
        token = vectorList->contents[*index];
    }

    return current;
}

PrimaryExpressionNode* makePrimaryExpressionNode(const Vector* vectorList, int* index) {
    PrimaryExpressionNode* primaryExpressionNode = calloc(1, sizeof(PrimaryExpressionNode));
    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_IDENTIFIER: {
            primaryExpressionNode->identifier = strdup(token->string);
            primaryExpressionNode->isExternalFunction = isExternFunction;
            (*index)++;
            break;
        }
        case TK_NUMBER:
        case TK_TRUE:
        case TK_FALSE:
        case TK_BYTE:
        case TK_STRING: {
            primaryExpressionNode->constantNode = makeConstantNode(vectorList, index);

            if (primaryExpressionNode->constantNode == NULL) {
                printf("error: could not create constant-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_LEFT_SQUARE: {
            (*index)++;
            primaryExpressionNode->expressionNode = makeExpressionNode(vectorList, index);

            if (primaryExpressionNode->expressionNode == NULL) {
                printf("error: could not create expression-type node.\n");
                return NULL;
            }

            Token* rightParethesisToken = vectorList->contents[*index];

            if (rightParethesisToken->type != TK_RIGHT_SQUARE) {
                printf("error: invalid symbol, expected a ']'.\n");
                return NULL;
            }

            (*index)++;
            break;
        }
        default: {
            printf("error: invalid symbol, expected a PRIMARY expression.\n");
            return NULL;
        }
    }

    return primaryExpressionNode;
}
