#include "asmgen.h"

void generatePrimaryExpressionLeft(const PrimaryExpressionNode* node) {
    if (node->constantNode != NULL) {
        generateConstantNode(node->constantNode);
    }
    else if (node->expressionNode != NULL) {
        generateExpressionLeft(node->expressionNode);
    }
    else if (node->identifier != NULL) {
        generateIdentifierLeft(node->identifier);
    }
    else {
        // @todo
    }
}

void generatePrimaryExpressionRight(const PrimaryExpressionNode* node) {
    if (node->constantNode != NULL) {
        generateConstantNode(node->constantNode);
    }
    else if (node->expressionNode != NULL) {
        generateExpression(node->expressionNode);
    }
    else if (node->identifier != NULL) {
        generateIdentifierRight(node->identifier);
    }
    else {
        // @todo
    }
}

void generatePostfixExpressionLeft(const PostfixExpressionNode* node) {
    switch (node->postfixExpressionType) {
        case POSTFIX_PRIMARY: {
            generatePrimaryExpressionLeft(node->primaryExpressionNode);
            break;
        }
        case POSTFIX_LEFT_SQUARE: {
            fprintf(assembly, "    ; START POSTFIX_LEFT_SQUARE\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            Type* type = topOfStack(typeStack);
            popStack(typeStack);

            generateExpression(node->expressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");

            if (type->arraySize) {
                if (type->pointerCount > 0) {
                    fprintf(assembly, "    imul     rdi, 8\n");
                    fprintf(assembly, "    mov      rax, [rax]\n");
                } 
                else {
                    fprintf(assembly, "    imul     rdi, %d\n", type->typeSize);
                }
            } 
            else {
                if (type->pointerCount > 1) {
                    fprintf(assembly, "    imul     rdi, 8\n");
                    fprintf(assembly, "    mov      rax, [rax]\n");
                } 
                else {
                    fprintf(assembly, "    imul     rdi, %d\n", type->typeSize);
                    fprintf(assembly, "    mov      rax, [rax]\n");
                }
            }

            fprintf(assembly, "    add      rax, rdi\n");
            fprintf(assembly, "    push     rax\n");

            pushStack(typeStack, type);
            fprintf(assembly, "    ; END POSTFIX_LEFT_SQUARE\n");
            break;
        }
        case POSTFIX_DOT: {
            fprintf(assembly, "    ; START POSTFIX_DOT\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            Type* type = topOfStack(typeStack);
            popStack(typeStack);

            const FieldInfo* fieldInfo = getStringMap(type->flockInfo->fieldInfoMap, node->identifier);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    add      rax, %d\n", fieldInfo->offset);
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END POSTFIX_DOT\n");
            break;        
        }
        case POSTFIX_POINTER: {
            fprintf(assembly, "    ; START POSTFIX_POINTER\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            Type* type = topOfStack(typeStack);
            popStack(typeStack);

            const FieldInfo* fieldInfo = getStringMap(type->flockInfo->fieldInfoMap, node->identifier);
            pushStack(typeStack, fieldInfo->type);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rax, [rax]\n");
            fprintf(assembly, "    add      rax, %d\n", fieldInfo->offset);
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END POSTFIX_POINTER\n");
            break;        
        }
        case POSTFIX_INCREMENT: {
            fprintf(assembly, "    ; START POSTFIX_INCREMENT\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rdi, [rax]\n");
            fprintf(assembly, "    push     rdi\n");
            fprintf(assembly, "    push     1\n");
            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    add      [rax], rdi\n");
            fprintf(assembly, "    ; END POSTFIX_INCREMENT\n");
            break;                       
        }
        case POSTFIX_DECREMENT: {
            fprintf(assembly, "    ; START POSTFIX_DECREMENT\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rdi, [rax]\n");
            fprintf(assembly, "    push     rdi\n");
            fprintf(assembly, "    push     1\n");
            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    sub      [rax], rdi\n");
            fprintf(assembly, "    ; END POSTFIX_DECREMENT\n");
            break;
        }
        default: {
            // @todo
            break;
        }
    }
}

void generatePostfixExpressionRight(const PostfixExpressionNode* node) {
    switch (node->postfixExpressionType) {
        case POSTFIX_PRIMARY: {
            generatePrimaryExpressionRight(node->primaryExpressionNode);
            break;
        }
        case POSTFIX_LEFT_SQUARE: {
            fprintf(assembly, "    ; START POSTFIX_LEFT_SQUARE\n");
            for (int i = 0; i < node->assignExpressionNodes->size; i++) {
                const AssignmentExpressionNode* assignmentExpressionNode = node->assignExpressionNodes->contents[i];
                generateConditionalExpression(assignmentExpressionNode->conditionalExpressionNode);
            }

            for (int j = node->assignExpressionNodes->size - 1; j >= 0; j--) {
                fprintf(assembly, "    pop      rax\n");
                fprintf(assembly, "    mov      %s, rax\n", argRegisters[j]);
            }

            const char* identifier = node->postfixExpressionNode->primaryExpressionNode->identifier;
            bool externalFunction = node->postfixExpressionNode->primaryExpressionNode->isExternalFunction;

            fprintf(assembly, "    mov      rax, 0\n");

            if (externalFunction) {
                fprintf(assembly, "    extern   %s\n", identifier);
                fprintf(assembly, "    call     %s wrt ..plt\n", identifier);
            }
            else {
                fprintf(assembly, "    call     %s\n", identifier);
            }

            fprintf(assembly, "    push     rax\n");
            pushIntegerStack(sizeStack, 8);
            fprintf(assembly, "    ; END POSTFIX_LEFT_SQUARE\n");
            break;
        }
        case POSTFIX_LEFT_PARENTHESIS: {
            fprintf(assembly, "    ; START POSTFIX_LEFT_PARENTHESIS\n");
            generatePostfixExpressionRight(node->postfixExpressionNode);

            Type* type = topOfStack(typeStack);
            popStack(typeStack);

            generateExpression(node->expressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");

            if (type->arraySize > 0) {
                if (type->pointerCount > 0) {
                    fprintf(assembly, "    imul     rdi, 8\n");
                } 
                else {
                    fprintf(assembly, "    imul     rdi, %d\n", type->typeSize);
                }

                fprintf(assembly, "    add      rax, rdi\n");
                fprintf(assembly, "    mov      rax, [rax]\n");
            }
            else {
                if (type->pointerCount > 1) {
                    fprintf(assembly, "    imul     rdi, 8\n");
                } 
                else {
                    fprintf(assembly, "    imul     rdi, %d\n", type->typeSize);
                }

                fprintf(assembly, "    add      rax, rdi\n");

                if (type->typeSize == 1 && type->pointerCount < 2) {
                    fprintf(assembly, "    movzx    eax, BYTE [rax]\n");
                } 
                else {
                    fprintf(assembly, "    mov      rax, [rax]\n");
                } 
            }

            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END POSTFIX_LEFT_PARENTHESIS\n");
            break;
        }
        case POSTFIX_DOT: {
            fprintf(assembly, "    ; START POSTFIX_DOT\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            Type* type = topOfStack(typeStack);
            popStack(typeStack);

            const FieldInfo* fieldInfo = getStringMap(type->flockInfo->fieldInfoMap, node->identifier);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    add      rax, %d\n", fieldInfo->offset);
            fprintf(assembly, "    push     [rax]\n");
            fprintf(assembly, "    ; END POSTFIX_DOT");
            break;        
        }
        case POSTFIX_POINTER: {
            fprintf(assembly, "    ; START POSTFIX_POINTER\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            Type* type = topOfStack(typeStack);
            popStack(typeStack);

            const FieldInfo* fieldInfo = getStringMap(type->flockInfo->fieldInfoMap, node->identifier);
            pushStack(typeStack, fieldInfo->type);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rax, [rax]\n");
            fprintf(assembly, "    add      rax, %d\n", fieldInfo->offset);
            fprintf(assembly, "    push     [rax]\n");
            fprintf(assembly, "    ; END POSTFIX_POINTER\n");
            break;
        }
        case POSTFIX_INCREMENT: {
            fprintf(assembly, "    ; START POSTFIX_INCREMENT\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rdi, [rax]\n");
            fprintf(assembly, "    push     rdi\n");
            fprintf(assembly, "    push     1\n");
            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    add      [rax], rdi\n");
            fprintf(assembly, "    ; END POSTFIX_INCREMENT\n");
            break;
        }
        case POSTFIX_DECREMENT: {
            fprintf(assembly, "    ; START POSTFIX_DECREMENT\n");
            generatePostfixExpressionLeft(node->postfixExpressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rdi, [rax]\n");
            fprintf(assembly, "    push     rdi\n");
            fprintf(assembly, "    push     1\n");
            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    sub      [rax], rdi\n");
            fprintf(assembly, "    ; END POSTFIX_DECREMENT\n");
            break;                       
        }
        default: {
            // @todo
            break;
        }
    }
}

void generateUnaryExpressionLeft(const UnaryExpressionNode* node) {
    switch (node->type) {
        case UNARY_NONE: {
            generatePostfixExpressionLeft(node->postfixExpressionNode);
            break;
        }
        case UNARY_INCREMENT: {
            // @todo
            break;
        }
        case UNARY_DECREMENT: {
            break;
        }
        case UNARY_OPERATOR: {
            switch (node->operatorType) {
                case OPERATOR_MULTIPLY: {
                    generateCastExpression(node->castExpressionNode);
                    break;
                }
                default: {
                    break;
                }
            }

            break;
        }
        case UNARY_SIZE_IDENTIFIER: 
        case UNARY_SIZE_TYPE: {
            break;
        }
        default: {
            break;
        }
    }
}

void generateCastExpressionLeft(const CastExpressionNode* node) {
    if (node->unaryExpressionNode != NULL) {
        generateUnaryExpressionLeft(node->unaryExpressionNode);
    }
    else {
        // @todo
    }
}

void generateUnaryExpressionRight(const UnaryExpressionNode* node) {
    switch (node->type) {
        case UNARY_NONE: {
            generatePostfixExpressionRight(node->postfixExpressionNode);
            break;
        }
        case UNARY_INCREMENT: {
            fprintf(assembly, "    ; START UNARY_INCREMENT\n");
            generateUnaryExpressionLeft(node->unaryExpressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rdi, [rax]\n");
            fprintf(assembly, "    add      rdi, 1\n");
            fprintf(assembly, "    mov      [rax], rdi\n");
            fprintf(assembly, "    ; END UNARY_INCREMENT\n");
            break;
        }
        case UNARY_DECREMENT: {
            fprintf(assembly, "    ; START UNARY_DECREMENT\n");
            generateUnaryExpressionLeft(node->unaryExpressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    mov      rdi, [rax]\n");
            fprintf(assembly, "    sub      rdi, 1\n");
            fprintf(assembly, "    mov      [rax], rdi\n");
            fprintf(assembly, "    ; END UNARY_DECREMENT\n");
            break;
        }
        case UNARY_OPERATOR: {
            switch (node->operatorType) {
                case OPERATOR_AND: {
                    fprintf(assembly, "    ; START OPERATOR_AND\n");
                    generateCastExpressionLeft(node->castExpressionNode);
                    fprintf(assembly, "    ; END OPERATOR_AND\n");
                    break;
                }
                case OPERATOR_MULTIPLY: {
                    fprintf(assembly, "    ; START OPERATOR_MULTIPLY\n");
                    generateCastExpression(node->castExpressionNode);
                    fprintf(assembly, "    pop      rax\n");
                    fprintf(assembly, "    mov      rax, [rax]\n");
                    fprintf(assembly, "    push     rax\n");
                    fprintf(assembly, "    ; END OPERATOR_MULTIPLY\n");
                    break;
                }
                case OPERATOR_ADD: {
                    break;
                }
                case OPERATOR_SUBTRACT: {
                    fprintf(assembly, "    ; START OPERATOR_SUBTRACT\n");
                    generateCastExpression(node->castExpressionNode);

                    fprintf(assembly, "    pop      rdi\n");
                    fprintf(assembly, "    mov      rax, 0\n");
                    fprintf(assembly, "    sub      rax, rdi\n");
                    fprintf(assembly, "    push     rax\n");
                    fprintf(assembly, "    ; END OPERATOR_SUBTRACT\n");
                    break;
                }
                case OPERATOR_EXCLAMATION: {
                    fprintf(assembly, "    ; START OPERATOR_EXCLAMATION\n");
                    generateCastExpression(node->castExpressionNode);

                    fprintf(assembly, "    pop      rax\n");
                    fprintf(assembly, "    cmp      rax, 0\n");
                    fprintf(assembly, "    sete     al\n");
                    fprintf(assembly, "    movzx    rax, al\n");
                    fprintf(assembly, "    push     rax\n");
                    fprintf(assembly, "    ; END OPERATOR_EXCLAMATION\n");
                    break;
                }
                default: {
                    break;
                }
            }

            break;
        }
        case UNARY_SIZE_IDENTIFIER: {
            fprintf(assembly, "    ; START UNARY_SIZE_IDENTIFIER\n");
            const LocalVariable* localVariable = getLocalVariable(node->sizeName);

            if (localVariable != NULL) {
                fprintf(assembly, "    push     %d\n", localVariable->type->typeSize);
            } 
            else {
                const GlobalVariable* gloabalVariable = getGlobalVariable(node->sizeName);
                fprintf(assembly, "    push     %d\n", gloabalVariable->type->typeSize);
            }

            fprintf(assembly, "    ; END UNARY_SIZE_IDENTIFIER\n");
            break;
        }
        case UNARY_SIZE_TYPE: {
            fprintf(assembly, "    ; START UNARY_SIZE_TYPE\n");
            const TypeNameNode* typeNameCode = node->typeNameNode;
            int size = 0;

            if (typeNameCode->isPointer) {
                size = 8;
            }
            else {
                const TypeSpecifierNode* typeSpecifierNode = typeNameCode->specifierQualifierNode->typeSpecifierNode;

                switch (typeSpecifierNode->typeSpecifier) {
                    case SIZE_SC8: {
                        size = 1;
                        break;
                    }
                    case SIZE_SI32: {
                        size = 8;
                        break;
                    }
                    case SIZE_FP64: {
                        size = 8;
                        break;
                    } 
                    case TYPE_FLOCK: {
                        const FlockSpecifierNode* flockSpecifierNode = typeSpecifierNode->flockSpecifierNode;
                        const char* ident = flockSpecifierNode->identifier;
                        const FlockInfo* flockInfo = getStringMap(flockMap, ident);
                        size = flockInfo->size;
                        break;
                    }
                    case TYPENAME: {
                        const FlockInfo* flockInfo = getStringMap(flockMap, typeSpecifierNode->flockName);
                        size = flockInfo->size;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }

            fprintf(assembly, "    push     %d\n", size);
            fprintf(assembly, "    ; END UNARY_SIZE_TYPE\n");
            break;
        }
        default: {
            break;
        }
    }
}

void generateCastExpression(const CastExpressionNode* node) {
    if (node->unaryExpressionNode != NULL) {
        generateUnaryExpressionRight(node->unaryExpressionNode);
    }
    else {
        // @todo
    }
}

void generateMultiplicationExpression(const MultiplicationExpressionNode* node) {
    if (node->multiplicationExpressionNode == NULL) {
        generateCastExpression(node->castExpressionNode);
    }
    else if (node->operatorType == OPERATOR_MULTIPLY) {
        fprintf(assembly, "    ; START OPERATOR_MULTIPLY\n");
        generateMultiplicationExpression(node->multiplicationExpressionNode);
        generateCastExpression(node->castExpressionNode);

        fprintf(assembly, "    pop      rdi\n");
        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    imul     rax, rdi\n");
        fprintf(assembly, "    push     rax\n");
        fprintf(assembly, "    ; END OPERATOR_MULTIPLY\n");
    }
    else if (node->operatorType == OPERATOR_DIVIDE) {
        fprintf(assembly, "    ; START OPERATOR_DIVIDE\n");
        generateMultiplicationExpression(node->multiplicationExpressionNode);
        generateCastExpression(node->castExpressionNode);

        fprintf(assembly, "    pop      rdi\n");
        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    cqo\n");
        fprintf(assembly, "    idiv     rdi\n");
        fprintf(assembly, "    push     rax\n");
        fprintf(assembly, "    ; END OPERATOR_DIVIDE\n");
    }
    else if (node->operatorType == OPERATOR_MODULO) {
        fprintf(assembly, "    ; START OPERATOR_MODULO\n");
        generateMultiplicationExpression(node->multiplicationExpressionNode);
        generateCastExpression(node->castExpressionNode);

        fprintf(assembly, "    pop      rdi\n");
        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    cqo\n");
        fprintf(assembly, "    idiv     rdi\n");
        fprintf(assembly, "    push     rdx\n");
        fprintf(assembly, "    ; END OPERATOR_MODULO\n");
    }
}

void generateAdditionExpression(const AdditionExpressionNode* node) {
    if (node->additionExpressionNode == NULL) {
        generateMultiplicationExpression(node->multiplicationExpressionNode);
    }
    else if (node->operatorType == OPERATOR_ADD) {
        fprintf(assembly, "    ; START OPERATOR_ADD\n");
        generateAdditionExpression(node->additionExpressionNode);
        generateMultiplicationExpression(node->multiplicationExpressionNode);

        fprintf(assembly, "    pop      rdi\n");
        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    add      rax, rdi\n");
        fprintf(assembly, "    push     rax\n");
        fprintf(assembly, "    ; END OPERATOR_ADD\n");
    }
    else if (node->operatorType == OPERATOR_SUBTRACT) {
        fprintf(assembly, "    ; START OPERATOR_SUBTRACT\n");
        generateAdditionExpression(node->additionExpressionNode);
        generateMultiplicationExpression(node->multiplicationExpressionNode);

        fprintf(assembly, "    pop      rdi\n");
        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    sub      rax, rdi\n");
        fprintf(assembly, "    push     rax\n");
        fprintf(assembly, "    ; END OPERATOR_SUBTRACT\n");
    }
}

void generateShiftExpression(const ShiftExpressionNode* node) {
    if (node->shiftExpressionNode == NULL) {
        generateAdditionExpression(node->additionExpressionNode);
    }
    else {
        // @todo
    }
}

void generateRelationalExpression(const RelationalExpressionNode* node) {
    switch (node->compareType) {
        case COMPARE_NONE: {
            generateShiftExpression(node->shiftExpressionNode);
            break;
        }
        case COMPARE_LESS_THAN: {
            fprintf(assembly, "    ; START COMPARE_LESS_THAN\n");
            generateRelationalExpression(node->relationalExpressionNode);
            generateShiftExpression(node->shiftExpressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, rdi\n");
            fprintf(assembly, "    setl     al\n");
            fprintf(assembly, "    movzx    rax, al\n");
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END COMPARE_LESS_THAN\n");
            break;
        }
        case COMPARE_GREATER_THAN: {
            fprintf(assembly, "    ; START COMPARE_GREATER_THAN\n");
            generateRelationalExpression(node->relationalExpressionNode);
            generateShiftExpression(node->shiftExpressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, rdi\n");
            fprintf(assembly, "    setg     al\n");
            fprintf(assembly, "    movzx    rax, al\n");
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END COMPARE_GREATER THAN\n");
            break;
        }
        case COMPARE_LESS_OR_EQUAL: {
            fprintf(assembly, "    ; START COMPARE_LESS_OR_EQUAL\n");
            generateRelationalExpression(node->relationalExpressionNode);
            generateShiftExpression(node->shiftExpressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, rdi\n");
            fprintf(assembly, "    setle    al\n");
            fprintf(assembly, "    movzx    rax, al\n");
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END COMPARE_LESS_OR_EQUAL\n");
            break;
        }
        case COMPARE_GREATER_OR_EQUAL: {
            fprintf(assembly, "    ; START COMPARE_GREATER_OR_EQUAL\n");
            generateRelationalExpression(node->relationalExpressionNode);
            generateShiftExpression(node->shiftExpressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, rdi\n");
            fprintf(assembly, "    setge    al\n");
            fprintf(assembly, "    movzx    rax, al\n");
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END COMPARE_GREATER_OR_EQUAL\n");
            break;
        }
        default: {
            break;
        }
    }
}

void generateEqualExpression(const EqualExpressionNode* node) {
    switch (node->compareType) {
        case COMPARE_NONE: {
            generateRelationalExpression(node->relationalExpressionNode);
            break;
        }
        case COMPARE_EQUAL: {
            fprintf(assembly, "    ; START COMPARE_EQUAL\n");
            generateEqualExpression(node->equalExpressionNode);
            generateRelationalExpression(node->relationalExpressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, rdi\n");
            fprintf(assembly, "    sete     al\n");
            fprintf(assembly, "    movzx    rax, al\n");
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END COMPARE_EQUAL\n");
            break;
        }
        case COMPARE_NOT_EQUAL: {
            fprintf(assembly, "    ; START COMPARE_NOT_EQUAL\n");
            generateEqualExpression(node->equalExpressionNode);
            generateRelationalExpression(node->relationalExpressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, rdi\n");
            fprintf(assembly, "    setne    al\n");
            fprintf(assembly, "    movzx    rax, al\n");
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END COMPARE_NOT_EQUAL\n");
            break;
        }
        default:{
            break;
        }
    }
}

void generateAndExpression(const AndExpressionNode* node) {
    if (node->andExpressionNode == NULL) {
        generateEqualExpression(node->equalExpressionNode);
    }
    else {
        // @todo
    }
}

void generateExclusiveOrExpression(const ExclusiveOrExpressionNode* node) {
    if (node->exclusiveOrExpressionNode == NULL) {
        generateAndExpression(node->andExpressionNode);
    }
    else {
        // @todo
    }
}

void generateInclusiveOrExpression(const InclusiveOrExpressionNode* node) {
    if (node->inclusiveOrExpressionNode == NULL) {
        generateExclusiveOrExpression(node->exclusiveOrExpressionNode);
    }
    else {
        // @todo
    }
}

void generateLogicalAndExpression(const LogicalAndExpressionNode* node) {
    if (node->logicalAndExpressionNode == NULL) {
        generateInclusiveOrExpression(node->inclusiveOrExpressionNode);
    }
    else {
        char* label1 = getLabel();
        char* label2 = getLabel();
        
        fprintf(assembly, "    ; START LOGICAL-AND\n");
        generateLogicalAndExpression(node->logicalAndExpressionNode);

        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    cmp      rax, 0\n");
        fprintf(assembly, "    je       %s\n", label1);

        generateInclusiveOrExpression(node->inclusiveOrExpressionNode);

        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    cmp      rax, 0\n");
        fprintf(assembly, "    je       %s\n", label1);
        fprintf(assembly, "    push     1\n");
        fprintf(assembly, "    jmp      %s\n", label2);
        fprintf(assembly, "%s:\n", label1);
        fprintf(assembly, "    push     0\n");
        fprintf(assembly, "%s:\n", label2);
        fprintf(assembly, "    ; END LOGICAL-AND\n");
    }
}

void generateLogicalOrExpression(const OrExpressionNode* node) {
    if (node->orExpressionNode == NULL) {
        generateLogicalAndExpression(node->logicalAndExpressionNode);
    }
    else {
        fprintf(assembly, "    ; START LOGICAL-OR\n");
        generateLogicalOrExpression(node->orExpressionNode);
        generateLogicalAndExpression(node->logicalAndExpressionNode);

        fprintf(assembly, "    pop      rdi\n");
        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    or       rax, rdi\n");
        fprintf(assembly, "    push     rax\n");
        fprintf(assembly, "    ; END LOGICAL-OR\n");
    }
}

void generateConditionalExpression(const ConditionalExpressionNode* node) {
    if (node->conditionalExpressionNode == NULL) {
        generateLogicalOrExpression(node->orExpressionNode);
    }
    else {
        const char* label1 = getLabel();
        const char* label2 = getLabel();

        fprintf(assembly, "    ; START CONDITIONAL-EXPRESSION\n");
        generateLogicalOrExpression(node->orExpressionNode);

        fprintf(assembly, "    pop      rax\n");
        fprintf(assembly, "    cmp      rax, 0\n");
        fprintf(assembly, "    je       %s\n", label1);

        generateExpression(node->expressionNode);

        fprintf(assembly, "    jmp      %s\n", label2);
        fprintf(assembly, "%s:\n", label1);

        generateConditionalExpression(node->conditionalExpressionNode);

        fprintf(assembly, "%s:\n", label2);
        fprintf(assembly, "    ; END CONDITIONAL-EXPRESSION\n");
    }
}

void generateAssignmentExpression(const AssignmentExpressionNode* node) {
    if (node->conditionalExpressionNode != NULL) {
        generateConditionalExpression(node->conditionalExpressionNode);
    }
    else {
        generateUnaryExpressionLeft(node->unaryExpressionNode);
        generateAssignmentExpression(node->assignExpressionNode);

        switch (node->assignOperator) {
            case OPERATOR_ASSIGN: {
                fprintf(assembly, "    ; START OPERATOR_ASSIGN\n");
                fprintf(assembly, "    pop      rdi\n");
                fprintf(assembly, "    pop      rax\n");

                const int size = topOfIntegerStack(sizeStack); 
                popIntegerStack(sizeStack);

                fprintf(assembly, "    mov      [rax], %s\n", getRegister("di", size));
                fprintf(assembly, "    ; END OPERATOR_ASSIGN\n");
                break;
            }
            case OPERATOR_MULTIPLY_EQUAL: {
                fprintf(assembly, "    ; START OPERATOR_MULTIPLY_EQUAL\n");
                fprintf(assembly, "    pop      rdi\n");
                fprintf(assembly, "    pop      rax\n");
                fprintf(assembly, "    mov      rsi, [rax]\n");
                fprintf(assembly, "    imul     rdi, rsi\n");
                fprintf(assembly, "    mov      [rax], rdi\n");
                fprintf(assembly, "    ; END OPERATOR_MULTIPLY_EQUAL\n");
                break;
            }
            case OPERATOR_DIVIDE_EQUAL: {
                fprintf(assembly, "    ; START OPERATOR_DIVIDE_EQUAL\n");
                fprintf(assembly, "    pop      rdi\n");
                fprintf(assembly, "    pop      rsi\n");
                fprintf(assembly, "    mov      rax, [rsi]\n");
                fprintf(assembly, "    cqo\n");
                fprintf(assembly, "    idiv     rdi\n");
                fprintf(assembly, "    mov      [rsi], rax\n");
                fprintf(assembly, "    ; END OPERATOR_DIVIDE_EQUAL\n");
                break;
            }
            case OPERATOR_MODULO_EQUAL: {
                fprintf(assembly, "    ; START OPERATOR_MODULO_EQUAL\n");
                fprintf(assembly, "    pop      rdi\n");
                fprintf(assembly, "    pop      rsi\n");
                fprintf(assembly, "    mov      rax, [rsi]\n");
                fprintf(assembly, "    cqo\n");
                fprintf(assembly, "    idiv     rdi\n");
                fprintf(assembly, "    mov      [rsi], rdx\n");
                fprintf(assembly, "    ; END OPERATOR_MODULO_EQUAL\n");
                break;
            }
            case OPERATOR_ADD_EQUAL: {
                fprintf(assembly, "    ; START OPERATOR_ADD_EQUAL\n");
                fprintf(assembly, "    pop      rdi\n");
                fprintf(assembly, "    pop      rax\n");
                fprintf(assembly, "    add      [rax], rdi\n");
                fprintf(assembly, "    ; END OPERATOR_ADD_EQUAL\n");
                break;
            }
            case OPERATOR_SUBTRACT_EQUAL: {
                fprintf(assembly, "    ; START OPERATOR_SUBTRACT_EQUAL\n");
                fprintf(assembly, "    pop      rdi\n");
                fprintf(assembly, "    pop      rax\n");
                fprintf(assembly, "    sub      [rax], rdi\n");
                fprintf(assembly, "    ; END OPERATOR_SUBTRACT_EQUAL\n");
                break;
            }
            default: {
                break;
            }
        }
    }
}

void generateExpressionLeft(const ExpressionNode* node) {
    generateUnaryExpressionLeft(node->assignExpressionNode->conditionalExpressionNode->orExpressionNode->logicalAndExpressionNode->inclusiveOrExpressionNode->exclusiveOrExpressionNode->andExpressionNode->equalExpressionNode->relationalExpressionNode->shiftExpressionNode->additionExpressionNode->multiplicationExpressionNode->castExpressionNode->unaryExpressionNode);
}

void generateExpression(const ExpressionNode* node) {
    if (node->expressionNode == NULL) {
        generateAssignmentExpression(node->assignExpressionNode);
    }
    else {
        const ExpressionNode* current = node;

        while (current->expressionNode != NULL) {
            generateAssignmentExpression(current->assignExpressionNode);
            current = current->expressionNode;
        }
    }
}

void generateExpressionStatement(const ExpressionStatementNode* node) {
    if (node->expressionNode != NULL) {
        generateExpression(node->expressionNode);
    }
}

int getArraySizeFromConstantExpression(const ConditionalExpressionNode* node) {
    return(node->orExpressionNode->logicalAndExpressionNode->inclusiveOrExpressionNode->exclusiveOrExpressionNode->andExpressionNode->equalExpressionNode->relationalExpressionNode->shiftExpressionNode->additionExpressionNode->multiplicationExpressionNode->castExpressionNode->unaryExpressionNode->postfixExpressionNode->primaryExpressionNode->constantNode->integerConstant);
}