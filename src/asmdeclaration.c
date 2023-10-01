#include "asmgen.h"

int getVariableSizeFromDeclarationSpecifierNodes(const Vector* nodes) {
    for (int i = 0; i < nodes->size; i++) {
        const DeclarationSpecifierNode* declarationSpecifierNode = nodes->contents[i];

        if (declarationSpecifierNode->typeSpecifierNode == NULL) {
            continue;
        }

        const TypeSpecifierNode* typeSpecifierNode = declarationSpecifierNode->typeSpecifierNode;

        if (typeSpecifierNode->typeSpecifier == TYPE_FLOCK) {
            const FlockSpecifierNode* flockSpecifierNode = typeSpecifierNode->flockSpecifierNode;
            const char* identifier = flockSpecifierNode->identifier;
            const FlockInfo* flockInfo = getStringMap(flockMap, identifier);
            return (flockInfo->fieldInfoMap->size * 8);
        }
        else if (typeSpecifierNode->typeSpecifier == TYPENAME) {
            const FlockInfo* flockInfo = getStringMap(flockMap, typeSpecifierNode->flockName);
            return (flockInfo->fieldInfoMap->size * 8);
        }
        else {
            return 8;
        }
    }

    return 0;
}

const char* getIdentifierFromDirectDeclarator(const DirectDeclaratorNode* node) {
    const DirectDeclaratorNode* current = node;
    
    while (current->directDeclaratorNode != NULL) {
        current = current->directDeclaratorNode;
    }

    return current->identifier;
}

void generateDeclaration(const DeclarationNode* node) {
    Type* type = NULL;

    for (int i = 0; i < node->declarationSpecifierNodes->size; i++) {
        const DeclarationSpecifierNode* declarationSpecifierNode = node->declarationSpecifierNodes->contents[i];
        const TypeSpecifierNode* typeSpecifierNode = declarationSpecifierNode->typeSpecifierNode;

        if (typeSpecifierNode == NULL) {
            continue;
        }

        type = generateTypeSpecifierInLocal(typeSpecifierNode);
        break;
    }

    for (int j = 0; j < node->initializeDeclaratorNodes->size; j++) {
        LocalVariable* localVariable = malloc(sizeof(LocalVariable));
        localVariable->type = type;

        const InitializeDeclaratorNode* initializeDeclaratorNode = node->initializeDeclaratorNodes->contents[j];
        const DeclaratorNode* declaratorNode = initializeDeclaratorNode->declaratorNode;
        const PointerNode* pointerNode = declaratorNode->pointerNode;

        if (pointerNode != NULL) {
            localVariable->type->pointerCount = pointerNode->count;
            localVariable->type->size = 8;
        } 
        else {
            localVariable->type->size = localVariable->type->typeSize;
        }

        const DirectDeclaratorNode* directDeclaratorNode = declaratorNode->directDeclaratorNode;
        const ConditionalExpressionNode* conditionalExpressionNode = directDeclaratorNode->conditionalExpressionNode;

        if (conditionalExpressionNode == NULL) {
            localVariable->type->arraySize = 0;
            currentOffset += localVariable->type->size;
        } 
        else {
            localVariable->type->arraySize = getArraySizeFromConstantExpression(conditionalExpressionNode);
            currentOffset += (localVariable->type->arraySize * localVariable->type->size);

        }

        localVariable->offset = alignOffset(currentOffset, localVariable->type->size);

        const DirectDeclaratorNode* identifierNode = getIdentifierDirectDeclarator(directDeclaratorNode);
        localVariable->name = strdup(identifierNode->identifier);
        pushVector(localVariableList, localVariable);

        if (initializeDeclaratorNode->initializerNode != NULL) {
            const InitializerNode* initializerNode = initializeDeclaratorNode->initializerNode;
            fprintf(assembly, "    lea      rax, [rbp-%d]\n", localVariable->offset);
            fprintf(assembly, "    push     rax\n");

            if (initializerNode->assignmentExpressionNode != NULL) {
                generateAssignmentExpression(initializerNode->assignmentExpressionNode);
            }

            fprintf(assembly, "    pop      rdi\n");
            fprintf(assembly, "    pop      rax\n");

            if (localVariable->type->size == 1) {
                fprintf(assembly, "    mov      [rax], dil\n");
            } 
            else {
                fprintf(assembly, "    mov      [rax], rdi\n");
            }
        }
    }
}

void generateExternalDeclaration(const ExternalDeclarationNode* node) {
    if (node->gaggleSpecifierNode != NULL) {
        generateGaggleSpecifier(node->gaggleSpecifierNode);
    }

    if (node->declarationNode != NULL) {
        generateGlobalDeclaration(node->declarationNode);
    }

    if (node->functionDefinitionNode != NULL) {
        generateFunctionDefinition(node->functionDefinitionNode);
    }
}

void generateFunctionDeclarator(const DeclaratorNode* node) {
    const DirectDeclaratorNode* directDeclaratorNode = node->directDeclaratorNode;
    const ParameterTypeListNode* parameterTypeListNode = directDeclaratorNode->parameterTypeListNode;

    if (parameterTypeListNode == NULL) {
        return;
    }

    const ParameterListNode* parameterListNode = parameterTypeListNode->parameterListNode;
    generateArguments(parameterListNode);
}

int calculateLocalVariableSizeInDeclaration(const DeclarationNode* node) {
    const int variableSize = getVariableSizeFromDeclarationSpecifierNodes(node->declarationSpecifierNodes);

    int size = 0;
    const Vector* initializeDeclaratorNodes = node->initializeDeclaratorNodes;

    for (int i = 0; i < initializeDeclaratorNodes->size; i++) {
        const InitializeDeclaratorNode* initializeDeclaratorNode = initializeDeclaratorNodes->contents[i];
        const DeclaratorNode* declarator_node = initializeDeclaratorNode->declaratorNode;
        const DirectDeclaratorNode* directDeclaratorNode = declarator_node->directDeclaratorNode;

        if (declarator_node->pointerNode != NULL) {
            size += 8;
        }
        else if (directDeclaratorNode->conditionalExpressionNode == NULL) {
            size += variableSize;
        } 
        else {
            const int arraySize = getArraySizeFromConstantExpression(directDeclaratorNode->conditionalExpressionNode);
            size += (arraySize * variableSize);
        }
    }

    return size;
}