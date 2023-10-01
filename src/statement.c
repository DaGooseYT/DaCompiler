#include "parser.h"

AsmStatementNode* makeAsmStatementNode(const Vector* vectorList, int* index) {
    AsmStatementNode* asmStatementNode = calloc(1, sizeof(AsmStatementNode));
    const Token* token = vectorList->contents[*index];

    token = vectorList->contents[++*index];

    if (token->type != TK_LEFT_SQUARE) {
        printf("error: invalid symbol, expected '['.\n");
        return NULL;
    }
    
    token = vectorList->contents[++*index];

    if (token->type == TK_STRING) {
        asmStatementNode->constantNode = makeConstantNode(vectorList, index);

        if (asmStatementNode->constantNode == NULL) {
            printf("error: could not create constant node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];
    }
    else {
        printf("error: invalid symbol, expected STRING.\n");
        return NULL;
    }

    if (token->type != TK_RIGHT_SQUARE) {
        printf("error: invalid symbol, expected ']'.\n");
        return NULL;
    }

    token = vectorList->contents[++*index];

    if (token->type != TK_SEMICOLON) {
        printf("error: invalid symbol, expected ';'.\n");
        return NULL;
    }

    (*index)++;
    return asmStatementNode;
}

JumpStatementNode* makeJumpStatementNode(const Vector* vectorList, int* index) {
    JumpStatementNode* jumpStatementNode = calloc(1, sizeof(JumpStatementNode));
    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_CONTINUE: {
            jumpStatementNode->type = JUMP_CONTINUE;
            token = vectorList->contents[++*index];

            if (token->type != TK_SEMICOLON) {
                printf("error: invalid symbol, expected ';'.\n");
                return NULL;
            }

            (*index)++;
            break;
        }
        case TK_STOP: {
            jumpStatementNode->type = JUMP_STOP;
            token = vectorList->contents[++*index];

            if (token->type != TK_SEMICOLON) {
                printf("error: invalid symbol, expected ';'.\n");
                return NULL;
            }

            (*index)++;
            break;
        }
        case TK_RETURN: {
            jumpStatementNode->type = JUMP_RETURN;
            token = vectorList->contents[++*index];

            if (token->type != TK_SEMICOLON) {
                jumpStatementNode->expressionNode = makeExpressionNode(vectorList, index);

                if (jumpStatementNode->expressionNode == NULL) {
                    printf("error: could not create expression-type node.\n");
                    return NULL;
                }

                token = vectorList->contents[*index];

                if (token->type != TK_SEMICOLON) {
                    printf("error: invalid symbol, expected ';'.\n");
                    return NULL;
                }
            }

            (*index)++;
            break;
        }
        default: {
            printf("error: invalid symbol, expected JUMP statement keyword.\n");
            return NULL;
        }
    }

    return jumpStatementNode;
}

ExpressionStatementNode* makeExpressionStatementNode(const Vector* vectorList, int* index) {
    ExpressionStatementNode* expressionStatementNode = calloc(1, sizeof(ExpressionStatementNode));
    const Token* token = vectorList->contents[*index];

    if (token->type == TK_SEMICOLON) {
        expressionStatementNode->expressionNode = NULL;
        (*index)++;
    }
    else {
        expressionStatementNode->expressionNode = makeExpressionNode(vectorList, index);

        if (expressionStatementNode->expressionNode == NULL) {
            printf("error: could not create expression-type node.\n");
            return NULL;
        }

        token = vectorList->contents[*index];

        if (token->type != TK_SEMICOLON) {
            printf("error: invalid symbol, expected ';'.\n");
            return NULL;
        }

        (*index)++;
    }

    return expressionStatementNode;
}

SelectionStatementNode* makeSelectionStatementNode(const Vector* vectorList, int* index) {
    SelectionStatementNode* selectionStatementNode = calloc(1, sizeof(SelectionStatementNode));
    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_IF: {
            token = vectorList->contents[++*index];

            if (token->type != TK_LEFT_SQUARE) {
                printf("error: invalid symbol, expected '['.\n");
                return NULL;
            }

            (*index)++;
            selectionStatementNode->expressionNode = makeExpressionNode(vectorList, index);

            if (selectionStatementNode->expressionNode == NULL) {
                printf("error: could not create expression-statement-type node.\n");
                return NULL;
            }

            token = vectorList->contents[*index];

            if (token->type != TK_RIGHT_SQUARE) {
                printf("error: invalid symbol, expected ']'.\n");
                return NULL;
            }

            (*index)++;
            selectionStatementNode->statementNode1 = makeStatementNode(vectorList, index);

            if (selectionStatementNode->statementNode1 == NULL) {
                printf("error: could not create statement-type node.\n");
                return NULL;
            }

            token = vectorList->contents[*index];

            if (token->type == TK_ELSE) {
                (*index)++;
                selectionStatementNode->selectionType = SELECTION_IF_ELSE;
                selectionStatementNode->statementNode2 = makeStatementNode(vectorList, index);

                if (selectionStatementNode->statementNode2 == NULL) {
                    printf("error: could not create statement node.\n");
                    return NULL;
                }
            } 
            else {
                selectionStatementNode->selectionType = SELECTION_IF;
            }

            break;
        }
        case TK_MATCH: {
            selectionStatementNode->selectionType = SELECTION_MATCH;
            token = vectorList->contents[++*index];

            if (token->type != TK_LEFT_SQUARE) {
                printf("error: invalid symbol, expected '['.\n");
                return NULL;
            }

            (*index)++;
            selectionStatementNode->expressionNode = makeExpressionNode(vectorList, index);

            if (selectionStatementNode->expressionNode == NULL) {
                printf("error: could not create expression node.\n");
                return NULL;
            }

            token = vectorList->contents[*index];

            if (token->type != TK_RIGHT_SQUARE) {
                printf("error: invalid symbol, expected ']'.\n");
                return NULL;
            }

            (*index)++;
            selectionStatementNode->statementNode1 = makeStatementNode(vectorList, index);

            if (selectionStatementNode->statementNode1 == NULL) {
                printf("error: could not create statement node.\n");
                return NULL;
            }

            break;
        }
        default: {
            printf("error: invalid symbol, expected IF/MATCH keywords.\n");
            return NULL;
        }
    }
 
    return selectionStatementNode;
}

IterationStatementNode* makeIterationStatementNode(const Vector* vectorList, int* index) {
    IterationStatementNode* iterationStatementNode = calloc(1, sizeof(IterationStatementNode));
    iterationStatementNode->declarationNodes = buildVectorList();
    
    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_WHILE: {
            iterationStatementNode->type = ITERATION_WHILE;
            token = vectorList->contents[++*index];

            if (token->type != TK_LEFT_SQUARE) {
                printf("error: invalid symbol, expected '['.\n");
                return NULL;
            }

            (*index)++;
            iterationStatementNode->expressionNode1 = makeExpressionNode(vectorList, index);

            if (iterationStatementNode->expressionNode1 == NULL) {
                printf("error: could not create expression-type node.\n");
                return NULL;
            }

            token = vectorList->contents[*index];

            if (token->type != TK_RIGHT_SQUARE) {
                printf("error: invalid symbol, expected ']'.\n");
                return NULL;
            }

            token = vectorList->contents[++*index];

            if (token->type != TK_DO) {
                printf("error: invalid symbol, expected 'do'.\n");
                return NULL;
            }

            (*index)++;
            iterationStatementNode->statementNode = makeStatementNode(vectorList, index);

            if (iterationStatementNode->statementNode == NULL) {
                printf("error: could not create statement-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_FOR: {
            iterationStatementNode->type = ITERATION_FOR;
            token = vectorList->contents[++*index];

            if (token->type != TK_LEFT_SQUARE) {
                printf("error: invalid symbol, expected '['.\n");
                return NULL;
            }

            token = vectorList->contents[++*index];

            if (token->type == TK_SEMICOLON) {
                (*index)++;
            } 

            else if (isDeclarationSpecifier(vectorList, *index)) {
                while (isDeclarationSpecifier(vectorList, *index)) {
                    DeclarationNode* declarationNode = makeDeclarationNode(vectorList, index);

                    if (declarationNode == NULL) {
                        printf("error: could not create declaration-type node.\n");
                        return NULL;
                    }

                    pushVector(iterationStatementNode->declarationNodes, declarationNode);
                }
            } 
            else {
                iterationStatementNode->expressionNode1 = makeExpressionNode(vectorList, index);

                if (iterationStatementNode->expressionNode1 == NULL) {
                    printf("error: could not create expression-type node.\n");
                    return NULL;
                }

                token = vectorList->contents[*index];

                if (token->type != TK_SEMICOLON) {
                    printf("error: invalid symbol, expected ';'.\n");
                    return NULL;
                }

                (*index)++;
            }

            token = vectorList->contents[*index];

            if (token->type == TK_SEMICOLON) {
                (*index)++;
            } 
            else {
                iterationStatementNode->expressionNode2 = makeExpressionNode(vectorList, index);

                if (iterationStatementNode->expressionNode2 == NULL) {
                    printf("error: could not create expression-type node.\n");
                    return NULL;
                }

                token = vectorList->contents[*index];

                if (token->type != TK_SEMICOLON) {
                    printf("error: invalid symbol, expected ';'.\n");
                    return NULL;
                }

                (*index)++;
            }

            token = vectorList->contents[*index];

            if (token->type == TK_RIGHT_SQUARE) {
                (*index)++;
            } 
            else {
                iterationStatementNode->expressionNode3 = makeExpressionNode(vectorList, index);

                if (iterationStatementNode->expressionNode3 == NULL) {
                    printf("error: could not create expression-type node.\n");
                    return NULL;
                }

                token = vectorList->contents[*index];

                if (token->type != TK_RIGHT_SQUARE) {
                    printf("error: invalid symbol, expected ']'.\n");
                    return NULL;
                }

                (*index)++;
            }

            iterationStatementNode->statementNode = makeStatementNode(vectorList, index);

            if (iterationStatementNode->statementNode == NULL) {
                printf("error: could not create statement-type node.\n");
                return NULL;
            }

            break;
        }
        default: {
            break;
        }
    }

    return iterationStatementNode;
}

LabelStatementNode* makeLabelStatementNode(const Vector* vectorList, int* index) {
    LabelStatementNode* labeledStatementNode = calloc(1, sizeof(LabelStatementNode));
    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_CASE: {
            (*index)++;

            labeledStatementNode->labeledStatementType = LABEL_CASE;
            labeledStatementNode->conditionalExpressionNode = makeConditionalExpressionNode(vectorList, index);

            if (labeledStatementNode->conditionalExpressionNode == NULL) {
                printf("error: could not create conditional-expression-type node.\n");
                return NULL;
            }
        
            token = vectorList->contents[*index];

            if (token->type != TK_COLON) {
                printf("error: invalid symbol, expected ':'.\n");
                return NULL;
            }

            (*index)++;

            labeledStatementNode->statementNode = makeStatementNode(vectorList, index);

            if (labeledStatementNode->statementNode == NULL) {
                printf("error: could not create statement-expression-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_DEFAULT: {
            labeledStatementNode->labeledStatementType = LABEL_DEFAULT;
            token = vectorList->contents[++*index];

            if (token->type != TK_COLON) {
                printf("error: invalid symbol, expected ':'.\n");
                return NULL;
            }

            (*index)++;
            labeledStatementNode->statementNode = makeStatementNode(vectorList, index);

            if (labeledStatementNode->statementNode == NULL) {
                printf("error: failed to create statement-expression-type node.\n");
                return NULL;
            }

            break;
        }
        default: {
            printf("error: invalid symbol, expected CASE/DEFAULT.\n");
            return NULL;
        }
    }

    return labeledStatementNode;
}

StatementNode* makeStatementNode(const Vector* vectorList, int* index) {
    StatementNode* statementNode = calloc(1, sizeof(StatementNode));
    const Token* token = vectorList->contents[*index];

    switch (token->type) {
        case TK_CASE:
        case TK_DEFAULT: {
            statementNode->labeledStatementNode = makeLabelStatementNode(vectorList, index);

            if (statementNode->labeledStatementNode == NULL) {
                printf("error: could not create labeled-statement-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_WHILE:
        case TK_FOR: {
            statementNode->iterationStatementNode = makeIterationStatementNode(vectorList, index);

            if (statementNode->iterationStatementNode == NULL) {
                printf("error: could not create iteration-statement-type node.\n");
                return NULL;
            }

            break;
        } 
        case TK_RETURN: 
        case TK_STOP:
        case TK_CONTINUE: {
            statementNode->jumpStatementNode = makeJumpStatementNode(vectorList, index);

            if (statementNode->jumpStatementNode == NULL) {
                printf("error: could not create jump-statement-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_IF: 
        case TK_MATCH: {
            statementNode->selectionStatementNode = makeSelectionStatementNode(vectorList, index);

            if (statementNode->selectionStatementNode == NULL) {
                printf("error: could not create selection-statement node.\n");
                return NULL;
            }

            break;
        }
        case TK_ASM: {
            statementNode->asmStatementNode = makeAsmStatementNode(vectorList, index);

            if (statementNode->asmStatementNode == NULL) {
                printf("error: could not create asm-statement-type node.\n");
                return NULL;
            }

            break;
        }
        case TK_LEFT_BRACKET: {
            statementNode->compoundStatementNode = makeCompoundStatementNode(vectorList, index);

            if (statementNode->compoundStatementNode == NULL) {
                printf("error: could not create compound-statement-type node.\n");
                return NULL;
            }

            break;
        }
        default: {
            isExternFunction = false;
            statementNode->expressionStatementNode = makeExpressionStatementNode(vectorList, index);

            if (statementNode->expressionStatementNode == NULL) {
                printf("error: could not create expression-statement-type node.\n");
                return NULL;
            }

            break;
        }
    }

    return statementNode;
}

CompoundStatementNode* makeCompoundStatementNode(const Vector* vectorList, int* index) {
    CompoundStatementNode* compoundStatementNode = malloc(sizeof(CompoundStatementNode));
    compoundStatementNode->blockItemNodes = buildVectorList();

    const Token* token = vectorList->contents[*index];

    if (token->type != TK_LEFT_BRACKET) {
        printf("error: invalid symbol, expected '['.\n");
        return NULL;
    }

    (*index)++;

    while (true) {
        token = vectorList->contents[*index];

        if (token->type == TK_RIGHT_BRACKET) {
            (*index)++;
            break;
        }

        BlockItemNode* blockItemNode = makeBlockItemNode(vectorList, index);

        if (blockItemNode == NULL) {
            printf("error: could not create block-item-type node.\n");
            return NULL; 
        }

        pushVector(compoundStatementNode->blockItemNodes, blockItemNode);
    }

    return compoundStatementNode;
}