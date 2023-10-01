#include "asmgen.h"

char* getLabel() {
    char buf[8];
    snprintf(buf, 8, "LABEL%d", labelIndex++);
    return strdup(buf);
}

char* getStringLabel() {
    char buf[8];
    snprintf(buf, 8, "LABELC%d", stringIndex++);
    return strdup(buf);
}

int alignOffset(int offset, int size) {
    if (size == 1) {
        return offset;
    }

    if (offset % 8 != 0) {
        offset += (8 - offset % 8);
    }

    return offset;
}

const char* getRegister(const char* base, int size) {
    switch (size) {
        case 1: {
            if (strcmp("ax", base) == 0) { 
                return "al";
            }
            else if (strcmp("di", base) == 0) { 
                return "dil";
            }
            else if (strcmp("si", base) == 0) { 
                return "sil";
            }
            else if (strcmp("dx", base) == 0) { 
                return "dl";
            }
            else if (strcmp("cx", base) == 0) { 
                return "cl";
            }
            else if (strcmp("r8", base) == 0) { 
                return "r8b";
            }
            else if (strcmp("r9", base) == 0) { 
                return "r9b"; 
            }
            else { 
                return NULL;  
            }
        }
        case 8: {
            if (strcmp("ax", base) == 0) { 
                return "rax";
            }
            else if (strcmp("di", base) == 0) { 
                return "rdi";
            }
            else if (strcmp("si", base) == 0) { 
                return "rsi";
            }
            else if (strcmp("dx", base) == 0) { 
                return "rdx";
            }
            else if (strcmp("cx", base) == 0) { 
                return "rcx";
            }
            else if (strcmp("r8", base) == 0) { 
                return "r8";
            }
            else if (strcmp("r9", base) == 0) { 
                return "r9";
            }
            else { 
                return NULL;
            }
        }
        default: {
            printf("Invalid size=%d\n", size);
            return NULL;
        }    
    }
}

LocalVariable* getLocalVariable(const char* str) {
    for (int i = 0; i < localVariableList->size; i++) {
        LocalVariable* localvar = localVariableList->contents[i];

        if (strcmp(localvar->name, str) == 0) {
            return localvar;
        }
    }

    return NULL;
}

GlobalVariable* getGlobalVariable(const char* str) {
    for (int i = 0; i < globalVariableList->size; i++) {
        GlobalVariable* globalVariable = globalVariableList->contents[i];

        if (strcmp(globalVariable->name, str) == 0) {
            return globalVariable;
        }
    }

    return NULL;
}

void generateIdentifierLeft(const char* identifier) {
    const LocalVariable* localVariable = getLocalVariable(identifier);

    if (localVariable != NULL) {
        fprintf(assembly, "    lea      rax, [rbp-%d]\n", localVariable->offset);
        fprintf(assembly, "    push     rax\n");

        pushStack(typeStack, localVariable->type);
    }
    else {
        const GlobalVariable* globalVariable = getGlobalVariable(identifier);

        fprintf(assembly, "    lea      rax, [rel %s]\n", globalVariable->name);
        fprintf(assembly, "    push     rax\n");

        pushStack(typeStack, globalVariable->type);
    }

    pushIntegerStack(sizeStack, 8);
}

void generateIdentifierRight(const char* identifier) {
    if (stringIntegerMapContains(gaggleMap, identifier)) {
        fprintf(assembly, "    push     %d\n", getIntegerMap(gaggleMap, identifier));
        return;
    }

    const LocalVariable* localVariable = getLocalVariable(identifier);
    if (localVariable != NULL) {
        fprintf(assembly, "    lea      rax, [rbp-%d]\n", localVariable->offset);

        if (localVariable->type->arraySize == 0) { 
            if (localVariable->type->typeSize == 1 && localVariable->type->pointerCount == 0) {
                fprintf(assembly, "    movzx    eax, BYTE [rax]\n");
            } 
            else { 
                fprintf(assembly, "    mov      rax, [rax]\n");
            }
        }

        fprintf(assembly, "    push     rax\n");
        pushIntegerStack(sizeStack, localVariable->type->size);
        pushStack(typeStack, localVariable->type);

        return;
    } 

    const GlobalVariable* globalVariable = getGlobalVariable(identifier);
    fprintf(assembly, "    lea      rax, [rel %s]\n", globalVariable->name);

    if (globalVariable->type->arraySize == 0) {
        if (globalVariable->type->typeSize == 1 && globalVariable->type->pointerCount == 0) {
            fprintf(assembly, "    movzx    eax, BYTE [rax]\n");
        } 
        else { 
            fprintf(assembly, "    mov      rax, [rax]\n");
        }
    }

    fprintf(assembly, "    push     rax\n");

    pushIntegerStack(sizeStack, globalVariable->type->size);
    pushStack(typeStack, globalVariable->type);
}

void generateConstantNode(const ConstantNode* node) {
    switch (node->constantType) {
        case CONSTANT_BYTE: {
            fprintf(assembly, "    ; START CONSTANT_BYTE\n");
            fprintf(assembly, "    push     %d\n", node->integerConstant);

            pushIntegerStack(sizeStack, 1);
            fprintf(assembly, "    ; END CONSTANT_BYTE\n");
            break;
        }
        case CONSTANT_SI32: {
            fprintf(assembly, "    ; START CONSTANT_SI32\n");
            fprintf(assembly, "    push     %d\n", node->integerConstant);

            pushIntegerStack(sizeStack, 8);
            fprintf(assembly, "    ; END CONSTANT_SI32\n");
            break;
        }
        case CONSTANT_STRING: {
            const char* label = getStringLabel();

            fprintf(assembly, "    ; START CONSTANT_STRING\n");
            fprintf(assembly, "section .data\n");
            fprintf(assembly, "%s:\n", label);
            fprintf(assembly, "    .string  db `%s`\n", node->characterConstant);
            fprintf(assembly, "section .text\n");
            fprintf(assembly, "    lea      rax, [rel %s]\n", label);
            fprintf(assembly, "    push     rax\n");
            fprintf(assembly, "    ; END CONSTANT_STRING\n");
            break;
        }
        case CONSTANT_FP64: {
            break;
        }
        default: {
            printf("error: invalid CONSTANT type.\n");
            break;
        }
    }
}

const DirectDeclaratorNode* getIdentifierDirectDeclarator(const DirectDeclaratorNode* node) {
    const DirectDeclaratorNode* current = node;

    while (current->directDeclaratorNode != NULL) {
        current = current->directDeclaratorNode;
    }

    return current;
}

void generateBlockItem(const BlockItemNode* node) {
    if (node->declarationNode != NULL) {
        generateDeclaration(node->declarationNode);
    } 
    else {
        generateStatement(node->statementNode);
    }
}

int calculateArgumentSize(const FunctionDefinitionNode* node) {
    const DeclaratorNode* declaratorNode = node->declaratorNode;

    if (declaratorNode == NULL) {
        return 0;
    }

    const DirectDeclaratorNode* directDeclaratorNode = declaratorNode->directDeclaratorNode;
    const ParameterTypeListNode* parameterTypeListNode = directDeclaratorNode->parameterTypeListNode;

    if (parameterTypeListNode == NULL) {
        return 0;
    }

    int cnt = 0;
    const ParameterListNode* current = parameterTypeListNode->parameterListNode;

    while (current != NULL) {
        cnt++;
        current = current->parameterListNode;
    }

    return cnt * 8;
}

Type* generateTypeSpecifierInLocal(const TypeSpecifierNode* node) {
    Type* type = calloc(1, sizeof(Type));

    switch (node->typeSpecifier) {
        case TYPE_UI0: { 
            type->baseType = VAR_UI0;
            type->typeSize = 8;
            return type;
        }
        case TYPE_SC8: { 
            type->baseType = VAR_SC8;
            type->typeSize = 1;
            return type;
        }
        case TYPE_SI32: {
            type->baseType = VAR_SI32;
            type->typeSize = 8;
            return type;
        }
        case TYPE_FP64: { 
            type->baseType = VAR_F32;
            type->typeSize = 8;
            return type;
        }
        case TYPE_FLOCK: { 
            type->baseType = VAR_FLOCK;
            type->flockInfo = getStringMap(flockMap, node->flockSpecifierNode->identifier);

            if (type->flockInfo == NULL) {
                printf("Invalid flock identifier: \"%s\"\n", node->flockSpecifierNode->identifier);
                return NULL;
            }

            type->typeSize = type->flockInfo->size;
            return type;
        } 
        case TYPENAME: { 
            type->baseType = VAR_FLOCK;   

            type->flockInfo = getStringMap(flockMap, node->flockName);

            if (type->flockInfo == NULL) {
                printf("Invalid flock identifier: \"%s\"\n", node->flockName);
                return NULL;
            }

            type->typeSize = type->flockInfo->size;
            return type;
        } 
        default: { 
            return NULL;
        }
    }
}

int countParameterListNode(const ParameterListNode* node) {
    int cnt = 0;
    const ParameterListNode* current = node;

    while (current != NULL) {
        current = current->parameterListNode;
        cnt++;
    }

    return cnt;
}

void generateParameterListNode(const ParameterListNode* node, int argumentIndex) {
    const ParameterDeclarationNode* parameterDeclarationNode = node->parameterDeclarationNode;
    const DeclaratorNode* declaratorNode = parameterDeclarationNode->declaratorNode;

    if (declaratorNode == NULL) {
        return;
    }

    Type* type = NULL;

    for (int i = 0; i < parameterDeclarationNode->declaratorSpecifierNodes->size; i++) {
        const DeclarationSpecifierNode* declarationSpecifierNode = parameterDeclarationNode->declaratorSpecifierNodes->contents[i];
        const TypeSpecifierNode* typeSpecifierNode = declarationSpecifierNode->typeSpecifierNode;

        if (typeSpecifierNode == NULL) {
            continue;
        }

        type = generateTypeSpecifierInLocal(typeSpecifierNode);
        break;
    }

    currentOffset += 8;

    const DirectDeclaratorNode* directDeclaratorNode = declaratorNode->directDeclaratorNode;

    LocalVariable* localVariable = malloc(sizeof(LocalVariable));
    localVariable->type = type;
    localVariable->offset = currentOffset;
    localVariable->name = strdup(directDeclaratorNode->identifier);

    pushVector(localVariableList, localVariable);

    const PointerNode* pointerNode = declaratorNode->pointerNode;

    if (pointerNode != NULL) {
        localVariable->type->pointerCount = pointerNode->count;
        localVariable->type->size = 8;
    } 
    else {
        localVariable->type->size = localVariable->type->typeSize;
    }

    fprintf(assembly, "    mov      [rbp-%d], %s\n", localVariable->offset, argRegisters[argumentIndex]);
}

void generateArguments(const ParameterListNode* node) {
    const int parameterCount = countParameterListNode(node);
    const ParameterListNode* current = node;
    int index = 1;

    while (current != NULL) {
        generateParameterListNode(current, parameterCount - index);
        current = current->parameterListNode;
        index++;
    }
}

void generateFunctionDefinition(const FunctionDefinitionNode* node) {
    localVariableList = buildVectorList();
    currentOffset = 0;

    const DeclaratorNode* declaratorNode = node->declaratorNode;
    const DirectDeclaratorNode* directDeclaratorNode = declaratorNode->directDeclaratorNode;
    const char* strng = getIdentifierFromDirectDeclarator(directDeclaratorNode);

    fprintf(assembly, "    ; START FUNCTION-DEFINITION (%s)\n", strng);
    fprintf(assembly, "global %s\n", strng);
    fprintf(assembly, "%s:\n", strng);

    const int localVariableSize = calculateLocalVariableSizeInCompoundStatement(node->compoundStatementNode);
    const int argumentSize = calculateArgumentSize(node);

    fprintf(assembly, "    push     rbp\n");
    fprintf(assembly, "    mov      rbp, rsp\n");
    fprintf(assembly, "    sub      rsp, %d\n", localVariableSize + argumentSize);

    generateFunctionDeclarator(declaratorNode);
    generateCompoundStatement(node->compoundStatementNode);

    if (returnLabel != NULL) {
        fprintf(assembly, "%s:\n", returnLabel);
    }

    fprintf(assembly, "    mov      rsp, rbp\n");
    fprintf(assembly, "    pop      rbp\n");
    fprintf(assembly, "    ret\n");
    fprintf(assembly, "    ; END FUNCTION-DEFINITION (%s)\n", strng);

    free(localVariableList);
    free(returnLabel);
    returnLabel = NULL;
    currentOffset = 0;
}

Type* generateTypeSpecifierInGlobal(const TypeSpecifierNode* node) {
    Type* type = calloc(1, sizeof(Type));

    switch (node->typeSpecifier) {
        case TYPE_UI0: { 
            type->baseType = VAR_UI0;
            type->typeSize = 8;
            return type;
        }
        case TYPE_SC8: { 
            type->baseType = VAR_SC8;
            type->typeSize = 1;
            return type;
        }
        case TYPE_SI32: {
            type->baseType = VAR_SI32;
            type->typeSize = 8;
            return type;
        }
        case TYPE_FP64: { 
            type->baseType = VAR_F32;
            type->typeSize = 8;
            return type;
        }
        case TYPE_FLOCK: { 
            type->baseType = VAR_FLOCK;   
            type->typeSize = 0;

            const FlockSpecifierNode* flockSpecifierNode = node->flockSpecifierNode;
            const char* flockName = flockSpecifierNode->identifier;
            const Vector* flockDeclarationNodes = flockSpecifierNode->flockDeclarationNodes;

            if (flockDeclarationNodes->size != 0) {
                type->flockInfo = getStringMap(flockMap, flockName);

                if (type->flockInfo == NULL) { 
                    type->flockInfo = malloc(sizeof(FlockInfo));
                    type->flockInfo->fieldInfoMap = buildStringMap(1024);
                    type->flockInfo->size = flockDeclarationNodes->size * 8;
                    appendStringMap(flockMap, flockName, type->flockInfo);
                }
                else {
                    type->flockInfo->fieldInfoMap = buildStringMap(1024);
                    type->flockInfo->size = flockDeclarationNodes->size * 8;
                }

                int offset = 0;

                for (int i = 0; i < flockDeclarationNodes->size; i++) {
                    const FlockDeclarationNode* flockDeclarationNode = flockDeclarationNodes->contents[i];
                    const Vector* specifierQualifierNodes = flockDeclarationNode->specifierQualifierNodes;

                    Type* fieldType = NULL;

                    for (int j = 0; j < specifierQualifierNodes->size; j++) {
                        const SpecifierQualifierNode* specifierQualifierNode = specifierQualifierNodes->contents[j];
                        const TypeSpecifierNode* typeSpecifierNode = specifierQualifierNode->typeSpecifierNode;

                        if (typeSpecifierNode == NULL) {
                            continue;
                        }

                        fieldType = generateTypeSpecifierInGlobal(typeSpecifierNode);
                        break;
                    }

                    const PointerNode* pointerNode = flockDeclarationNode->pointerNode;

                    if (pointerNode != NULL) {
                        fieldType->pointerCount = pointerNode->count;
                        fieldType->size = 8; 
                    } 
                    else {
                        fieldType->size = fieldType->typeSize;
                    }

                    FieldInfo* fieldInfo = malloc(sizeof(FieldInfo));
                    fieldInfo->type = fieldType;

                    fieldInfo->offset = offset;
                    offset += fieldInfo->type->size;

                    appendStringMap(type->flockInfo->fieldInfoMap, flockDeclarationNode->identifier, fieldInfo);
                }

                type->flockInfo->size = type->flockInfo->fieldInfoMap->size * 8;
            }

            return type;
        }
        case TYPENAME: {
            type->baseType = VAR_FLOCK;   
            FlockInfo* flockInfo = getStringMap(flockMap, node->flockName);

            if (flockInfo == NULL) {
                flockInfo = calloc(1, sizeof(FlockInfo));
                appendStringMap(flockMap, node->flockName, flockInfo);
            }

            type->flockInfo = flockInfo;
            type->typeSize = flockInfo->size;
            return type;
        }
        default: {
            return NULL;
        }
    }
}

int getIntegerConstant(const AssignmentExpressionNode* node) {
    return(node->conditionalExpressionNode->orExpressionNode->logicalAndExpressionNode->inclusiveOrExpressionNode->exclusiveOrExpressionNode->andExpressionNode->equalExpressionNode->relationalExpressionNode->shiftExpressionNode->additionExpressionNode->multiplicationExpressionNode->castExpressionNode->unaryExpressionNode->postfixExpressionNode->primaryExpressionNode->constantNode->integerConstant);
}

const char* getCharacterConstant(const AssignmentExpressionNode* node) {
    return(node->conditionalExpressionNode->orExpressionNode->logicalAndExpressionNode->inclusiveOrExpressionNode->exclusiveOrExpressionNode->andExpressionNode->equalExpressionNode->relationalExpressionNode->shiftExpressionNode->additionExpressionNode->multiplicationExpressionNode->castExpressionNode->unaryExpressionNode->postfixExpressionNode->primaryExpressionNode->constantNode->characterConstant);
}

bool isIntegerConstant(const AssignmentExpressionNode* node) {
    return(SIZE_SI32 == node->conditionalExpressionNode->orExpressionNode->logicalAndExpressionNode->inclusiveOrExpressionNode->exclusiveOrExpressionNode->andExpressionNode->equalExpressionNode->relationalExpressionNode->shiftExpressionNode->additionExpressionNode->multiplicationExpressionNode->castExpressionNode->unaryExpressionNode->postfixExpressionNode->primaryExpressionNode->constantNode->constantType);
}

void generateGlobalDeclaration(const DeclarationNode* node) {
    Type* type = NULL;

    for (int i = 0; i < node->declarationSpecifierNodes->size; i++) {
        const DeclarationSpecifierNode* declarationSpecifierNode = node->declarationSpecifierNodes->contents[i];
        const TypeSpecifierNode* typeSpecifierNode = declarationSpecifierNode->typeSpecifierNode;

        if (typeSpecifierNode == NULL) {
            continue;
        }

        type = generateTypeSpecifierInGlobal(typeSpecifierNode);
        break;
    }

    for (int j = 0; j < node->initializeDeclaratorNodes->size; j++) {
        GlobalVariable* globalVeriable = malloc(sizeof(GlobalVariable));
        globalVeriable->type = type;
        globalVeriable->type->pointerCount = 0;

        const InitializeDeclaratorNode* initializeDeclaratorNode = node->initializeDeclaratorNodes->contents[j];
        const DeclaratorNode* declaratorNode = initializeDeclaratorNode->declaratorNode;
        const PointerNode* pointerNode = declaratorNode->pointerNode;

        if (pointerNode != NULL) {
            globalVeriable->type->pointerCount = pointerNode->count;
            globalVeriable->type->size = 8;
        } 
        else {
            globalVeriable->type->size = globalVeriable->type->typeSize;
        }

        const DirectDeclaratorNode* directDeclaratorNode = declaratorNode->directDeclaratorNode;
        const ConditionalExpressionNode* conditionalExpressionNode = directDeclaratorNode->conditionalExpressionNode;

        if (conditionalExpressionNode == NULL) {
            globalVeriable->type->arraySize = 0;
        }
        else {
            globalVeriable->type->arraySize = getArraySizeFromConstantExpression(conditionalExpressionNode);
        }

        const DirectDeclaratorNode* identifierNode = getIdentifierDirectDeclarator(directDeclaratorNode);

        if (initializeDeclaratorNode->initializerNode != NULL) {
            fprintf(assembly, "global %s\n", identifierNode->identifier);
        }
        else {
            fprintf(assembly, "common %s 8 8\n", identifierNode->identifier);
        }

        globalVeriable->name = strdup(identifierNode->identifier);
        pushVector(globalVariableList, globalVeriable);

        if (initializeDeclaratorNode->initializerNode != NULL) {
            const InitializerNode* initializerNode = initializeDeclaratorNode->initializerNode;

            if (initializerNode->assignmentExpressionNode != NULL) {
                if (isIntegerConstant(initializerNode->assignmentExpressionNode)) {
                    fprintf(assembly, "section .data\n");
                    fprintf(assembly, "%s:\n", globalVeriable->name);
                    fprintf(assembly, "section .quad  %d\n", getIntegerConstant(initializerNode->assignmentExpressionNode));
                    fprintf(assembly, "section .text\n");
                }
                else {
                    char* label1 = getStringLabel();

                    fprintf(assembly, "section .data\n");
                    fprintf(assembly, "%s:\n", label1);
                    fprintf(assembly, "    .string  db `%s`\n", getCharacterConstant(initializerNode->assignmentExpressionNode));
                    fprintf(assembly, "%s:\n", globalVeriable->name);
                    fprintf(assembly, "section .quad  %s\n", label1);
                    fprintf(assembly, "section .text\n");
                }
            } 
            else {
                InitializerListNode* initializerListNode = initializerNode->initializerListNode;
                const InitializerNode* first = initializerListNode->initializerNodes->contents[0];

                if (isIntegerConstant(first->assignmentExpressionNode)) {
                    fprintf(assembly, "section .data\n");
                    fprintf(assembly, "%s:\n", globalVeriable->name);

                    for (int k = 0; k < initializerListNode->initializerNodes->size; k++) {
                        const InitializerNode* init1 = initializerListNode->initializerNodes->contents[k];
                        fprintf(assembly, "section .quad  %d\n", getIntegerConstant(init1->assignmentExpressionNode));
                    }

                    fprintf(assembly, "section .text\n");
                }
                else {
                    Vector* labelList = buildVectorList();
                    fprintf(assembly, "section .data\n");

                    for (int l = 0; l < initializerListNode->initializerNodes->size; l++) {
                        const InitializerNode* initilizer2 = initializerListNode->initializerNodes->contents[l];
                        char* label2 = getStringLabel();

                        fprintf(assembly, "%s:\n", label2);
                        fprintf(assembly, "    .string  db `%s`\n", getCharacterConstant(initilizer2->assignmentExpressionNode));

                        pushVector(labelList, label2);
                    }

                    fprintf(assembly, "%s:\n", globalVeriable->name);

                    for (int m = 0; m < labelList->size; m++) {
                        char* label = labelList->contents[m];
                        fprintf(assembly, "section .quad  %s\n", label);
                    }

                    fprintf(assembly, "section .text\n");
                }
            }
        }
    }
}

void generateGaggleSpecifier(const GaggleSpecifierNode* node) {
    const GaggleListNode* gaggleListNode = node->gaggleListNode;
    const Vector* identifiers = gaggleListNode->identifiers;

    for (int i = 0; i < identifiers->size; i++) {
        appendStringIntegerMap(gaggleMap, identifiers->contents[i], i);
    }
}

void asmgen(const TransUnitNode* node, char* outputFile, bool asmGeneration) {
    argRegisters[0] = "rdi";
    argRegisters[1] = "rsi";
    argRegisters[2] = "rdx";
    argRegisters[3] = "rcx";
    argRegisters[4] = "r8";
    argRegisters[5] = "r9";

    if (asmGeneration) {
        char* output = malloc(strlen(outputFile) + 5);
        sprintf(output, "%s.asm", outputFile);
        assembly = fopen(output, "w");
    }
    else {
        assembly = fopen("temp.asm", "w");
    }
    
    fprintf(assembly, ";\n; this file was generated by DaCompiler from Goose source code.\n; use NASM to assemble.\n;\n\n");

    labelIndex = 2;
    stopLabelStack = buildStack();
    continueLabelStack = buildStack();
    typeStack = buildStack();
    currentStatementLabelStack = buildStack();
    sizeStack = initializeIntegerStack();
    globalVariableList = buildVectorList();
    flockMap = buildStringMap(1024);
    gaggleMap = buildStringIntegerMap(1024);

    for (int i = 0; i < node->externalDeclarationNodes->size; i++) {
        generateExternalDeclaration(node->externalDeclarationNodes->contents[i]);
    }

    fclose(assembly);
}