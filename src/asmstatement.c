#include "asmgen.h"

int calculateLocalVariableSizeInLabeledStatement(const LabelStatementNode* node) {
    return calculateLocalVariableSizeInStatement(node->statementNode);
}

int calculateLocalVariableSizeInSelectionStatement(const SelectionStatementNode* node) {
    int size = 0;
    size += calculateLocalVariableSizeInStatement(node->statementNode1);

    if (node->statementNode2 != NULL) {
        size += calculateLocalVariableSizeInStatement(node->statementNode2);
    }

    return size;
}

int calculateLocalVariableSizeInIterationStatement(const IterationStatementNode* node) {
    int size = 0;

    for (int i = 0; i < node->declarationNodes->size; i++) {
        const DeclarationNode* declarationNode = node->declarationNodes->contents[i];
        const Vector* initializeDeclaratorNodes = declarationNode->initializeDeclaratorNodes;

        for (int j = 0; j < initializeDeclaratorNodes->size; j++) {
            const InitializeDeclaratorNode* initializeDeclaratorNode = initializeDeclaratorNodes->contents[j];
            const DeclaratorNode* declaratorNode = initializeDeclaratorNode->declaratorNode;
            const DirectDeclaratorNode* directDeclaratorNode = declaratorNode->directDeclaratorNode;

            if (directDeclaratorNode->conditionalExpressionNode == NULL) {
                size += 8;
            }
            else {
                const int arraySize = getArraySizeFromConstantExpression(directDeclaratorNode->conditionalExpressionNode);
                size += (arraySize * 8);
            }
        }
    }

    size += calculateLocalVariableSizeInStatement(node->statementNode);
    return size;
}

void generateJumpStatement(const JumpStatementNode* node) {
    switch (node->type) {
        case JUMP_CONTINUE: {
            const char* label = topOfStack(continueLabelStack);

            fprintf(assembly, "    jmp      %s\n", label);
            break;
        }
        case JUMP_STOP: {
            const char* label = topOfStack(stopLabelStack);

            fprintf(assembly, "    jmp      %s\n", label);
            break;
        }
        case JUMP_RETURN: {
            if (node->expressionNode != NULL) {
                generateExpression(node->expressionNode);
            }

            fprintf(assembly, "    pop      rax\n");

            if (returnLabel == NULL) {
                returnLabel = getLabel();
            }

            fprintf(assembly, "    jmp      %s\n", returnLabel);
            break;
        }
        default: {
            break;
        }
    }
}

void generateSelectionStatement(const SelectionStatementNode* node) {
    switch (node->selectionType) {
        case SELECTION_IF: {
            const char* label = getLabel();

            fprintf(assembly, "    ; START SELECTION_IF\n");
            generateExpression(node->expressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, 0\n");
            fprintf(assembly, "    je       %s\n", label);

            generateStatement(node->statementNode1);

            fprintf(assembly, "%s:\n", label);
            fprintf(assembly, "    ; END SELECTION_IF\n");
            break;
        }
        case SELECTION_IF_ELSE: {
            const char* label1 = getLabel();
            const char* label2 = getLabel();

            fprintf(assembly, "    ; START SELECTION_IF_ELSE\n");
            generateExpression(node->expressionNode);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, 0\n");
            fprintf(assembly, "    je       %s\n", label1);

            generateStatement(node->statementNode1);

            fprintf(assembly, "    jmp      %s\n", label2);
            fprintf(assembly, "%s:\n", label1);

            generateStatement(node->statementNode2);

            fprintf(assembly, "%s:\n", label2);
            fprintf(assembly, "    ; END SELECTION_IF_ELSE\n");
            break;
        }
        case SELECTION_MATCH: {
            char* label = getLabel();

            fprintf(assembly, "    ; START SELECTION_MATCH\n");
            pushStack(stopLabelStack, label);
            pushStack(currentStatementLabelStack, getLabel()); 

            generateExpression(node->expressionNode);
            generateStatement(node->statementNode1);

            fprintf(assembly, "%s:\n", label);

            popStack(stopLabelStack);
            popStack(currentStatementLabelStack);
            fprintf(assembly, "    ; END SELECTION_MATCH\n");
            break;
        }
        default: {
            break;
        }
    }
}

void generateIterationStatement(const IterationStatementNode* node) {
    switch (node->type) {
        case ITERATION_WHILE: {
            char* label1 = getLabel();
            char* label2 = getLabel();

            fprintf(assembly, "    ; START ITERATION_WHILE\n");
            pushStack(continueLabelStack, label1);
            pushStack(stopLabelStack, label2);

            fprintf(assembly, "%s:\n", label1);

            generateExpression(node->expressionNode1);

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, 0\n");
            fprintf(assembly, "    je       %s\n", label2);

            generateStatement(node->statementNode);

            fprintf(assembly, "    jmp      %s\n", label1);
            fprintf(assembly, "%s:\n", label2);

            popStack(continueLabelStack);
            popStack(stopLabelStack);
            fprintf(assembly, "    ; END ITERATION_WHILE\n");
            break;
        }
        case ITERATION_FOR: {
            char* label1 = getLabel();
            char* label2 = getLabel();
            char* label3 = getLabel();

            fprintf(assembly, "    ; START ITERATION_FOR\n");
            pushStack(continueLabelStack, label2);
            pushStack(stopLabelStack, label3);

            if (node->declarationNodes->size != 0) {
                for (int i = 0; i < node->declarationNodes->size; i++) {
                    generateDeclaration(node->declarationNodes->contents[i]);
                }
            }
            else if (node->expressionNode1 != NULL) {
                generateExpression(node->expressionNode1);
            }

            fprintf(assembly, "%s:\n", label1);

            if (node->expressionNode2 != NULL) {
                generateExpression(node->expressionNode2);
            }

            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    cmp      rax, 0\n");
            fprintf(assembly, "    je       %s\n", label3);

            generateStatement(node->statementNode);

            fprintf(assembly, "%s:\n", label2);

            if (node->expressionNode3 != NULL) {
                generateExpression(node->expressionNode3);
            }

            fprintf(assembly, "    jmp      %s\n", label1);
            fprintf(assembly, "%s:\n", label3);

            popStack(continueLabelStack);
            popStack(stopLabelStack);
            fprintf(assembly, "    ; END ITERATION_FOR\n");
            break;
        }
        default: {
            break;
        }
    }
}

void generateCompoundStatement(const CompoundStatementNode* node) {
    for (int i = 0; i < node->blockItemNodes->size; i++) {
        generateBlockItem(node->blockItemNodes->contents[i]);
    }
}

void generateLabeledStatement(const LabelStatementNode* node) {
    switch (node->labeledStatementType) {
        case LABEL_CASE: {
            fprintf(assembly, "    ; START LABEL_CASE\n");
            generateConditionalExpression(node->conditionalExpressionNode);

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    cmp      rax, rdi\n");

            char* currentStatementLabel = topOfStack(currentStatementLabelStack);

            if (node->statementNode->labeledStatementNode == NULL) {
                const char* label = getLabel();
                fprintf(assembly, "    jne      %s\n", label);
                fprintf(assembly, "%s:\n", currentStatementLabel);
                generateStatement(node->statementNode);
                fprintf(assembly, "%s:\n", label); 
            
                popStack(currentStatementLabelStack);
                pushStack(currentStatementLabelStack, getLabel()); 
            } 
            else {
                fprintf(assembly, "    je       %s\n", currentStatementLabel);
                generateLabeledStatement(node->statementNode->labeledStatementNode);
            }
            
            fprintf(assembly, "    ; END LABEL_CASE\n");
            break;
        }
        case LABEL_DEFAULT: {
            generateStatement(node->statementNode);
            break;
        }
        default: {
            break;
        }
    }
}

void generateAsmStatement(const AsmStatementNode* node) {
    if (node->constantNode->constantType == CONSTANT_STRING) {
        fprintf(assembly, "    %s\n", node->constantNode->characterConstant);
    }
}

void generateStatement(const StatementNode* node) {
    if (node->labeledStatementNode != NULL) {
        generateLabeledStatement(node->labeledStatementNode);
    }
    else if (node->expressionStatementNode != NULL) {
        generateExpressionStatement(node->expressionStatementNode);
    }
    else if (node->iterationStatementNode != NULL) {
        generateIterationStatement(node->iterationStatementNode);
    }
    else if (node->asmStatementNode != NULL) {
        generateAsmStatement(node->asmStatementNode);
    }
    else if (node->compoundStatementNode != NULL) {
        generateCompoundStatement(node->compoundStatementNode);
    }
    else if (node->jumpStatementNode != NULL) {
        generateJumpStatement(node->jumpStatementNode);
    }
    else if (node->selectionStatementNode != NULL) {
        generateSelectionStatement(node->selectionStatementNode);
    }
}

int calculateLocalVariableSizeInStatement(const StatementNode* node) {
    if (node->labeledStatementNode != NULL) {
        return calculateLocalVariableSizeInLabeledStatement(node->labeledStatementNode);
    }
    else if (node->compoundStatementNode != NULL) {
        return calculateLocalVariableSizeInCompoundStatement(node->compoundStatementNode);
    }
    else if (node->selectionStatementNode != NULL) {
        return calculateLocalVariableSizeInSelectionStatement(node->selectionStatementNode);
    }
    else if (node->iterationStatementNode != NULL) {
        return calculateLocalVariableSizeInIterationStatement(node->iterationStatementNode);
    }

    return 0;
}

int calculateLocalVariableSizeInCompoundStatement(const CompoundStatementNode* node) {
    if (node == NULL) {
        return 0;
    }

    int size = 0;

    for (int i = 0; i < node->blockItemNodes->size; i++) {
        const BlockItemNode* blockItemNode = node->blockItemNodes->contents[i];

        if (blockItemNode->declarationNode != NULL) {
            size += calculateLocalVariableSizeInDeclaration(blockItemNode->declarationNode);
        } 
        else {
            size += calculateLocalVariableSizeInStatement(blockItemNode->statementNode);
        }
    }

    return size;
}