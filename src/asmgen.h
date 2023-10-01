#pragma once

#include "parser.h"

typedef struct FlockInfo FlockInfo;
struct FlockInfo {
    int size;
    StringMap* fieldInfoMap;
};

typedef struct Type Type;
struct Type {
    int baseType;
    int typeSize;
    int size;
    int arraySize;
    int pointerCount;
    FlockInfo* flockInfo;
};

typedef struct FieldInfo FieldInfo;
struct FieldInfo {
    int offset;
    Type* type;
};

typedef struct LocalVariable LocalVariable; 
struct LocalVariable {
    char* name;
    int nameLength;
    int offset;
    Type* type; 
};

typedef struct GlobalVariable GlobalVariable;
struct GlobalVariable {
    char* name;
    int nameLength;
    Type* type;
};

enum VarType {
    VAR_UI0,
    VAR_SC8,
    VAR_SI32,
    VAR_F32,
    VAR_PTR,
    VAR_FLOCK,
};

int labelIndex;
int stringIndex;
int currentOffset;
char* returnLabel;
char* argRegisters[6];
Vector* localVariableList;
Vector* globalVariableList;
StringMap* flockMap;
StringIntegerMap* gaggleMap;
Stack* stopLabelStack;
Stack* continueLabelStack;
Stack* currentStatementLabelStack;
Stack* typeStack;
IntegerStack* sizeStack;
FILE* assembly;

const char* getIdentifierFromDirectDeclarator(const DirectDeclaratorNode* node);
const DirectDeclaratorNode* getIdentifierDirectDeclarator(const DirectDeclaratorNode* node);
int countParameterListNode(const ParameterListNode* node);
int calculateLocalVariableSizeInDeclaration(const DeclarationNode* node);
int calculateLocalVariableSizeInLabeledStatement(const LabelStatementNode* node);
int calculateLocalVariableSizeInSelectionStatement(const SelectionStatementNode* node);
int calculateLocalVariableSizeInIterationStatement(const IterationStatementNode* node);
int getVariableSizeFromDeclarationSpecifierNodes(const Vector* nodes);
int getIntegerConstant(const AssignmentExpressionNode* node);
char* getLabel();
const char* getRegister(const char* base, int size);
const char* getCharacterConstant(const AssignmentExpressionNode* node);
int calculateLocalVariableSizeInCompoundStatement(const CompoundStatementNode* node);
int calculateArgumentSize(const FunctionDefinitionNode* node);
int calculateLocalVariableSizeInStatement(const StatementNode* node);
LocalVariable* getLocalVariable(const char* str);
GlobalVariable* getGlobalVariable(const char* str);
void generateExpression(const ExpressionNode* node);
void generateExpressionLeft(const ExpressionNode* node);
void generateIdentifierRight(const char* identifier);
void generateIdentifierLeft(const char* identifier);
void generateExpressionStatement(const ExpressionStatementNode* node);
void generateStatement(const StatementNode* node);
void generateBlockItem(const BlockItemNode* node);
void generateFunctionDefinition(const FunctionDefinitionNode* node);
void generateIterationStatement(const IterationStatementNode* node);
void generatePrimaryExpressionLeft(const PrimaryExpressionNode* node);
void generatePrimaryExpressionRight(const PrimaryExpressionNode* node);
void generatePostfixExpressionLeft(const PostfixExpressionNode* node);
void generatePostfixExpressionRight(const PostfixExpressionNode* node);
void generateUnaryExpressionLeft(const UnaryExpressionNode* node);
void generateUnaryExpressionRight(const UnaryExpressionNode* node);
void generateMultiplicationExpression(const MultiplicationExpressionNode* node);
void generateCastExpressionLeft(const CastExpressionNode* node);
void generateCompoundStatement(const CompoundStatementNode* node);
void generateConditionalExpression(const ConditionalExpressionNode* node);
void generateLabeledStatement(const LabelStatementNode* node);
void generateAsmStatement(const AsmStatementNode* node);
void generateAdditionExpression(const AdditionExpressionNode* node);
void generateShiftExpression(const ShiftExpressionNode* node);
void generateRelationalExpression(const RelationalExpressionNode* node);
void generateEqualExpression(const EqualExpressionNode* node);
void generateAndExpression(const AndExpressionNode* node);
void generateExclusiveOrExpression(const ExclusiveOrExpressionNode* node);
void generateInclusiveOrExpression(const InclusiveOrExpressionNode* node);
void generateLogicalAndExpression(const LogicalAndExpressionNode* node);
void generateLogicalOrExpression(const OrExpressionNode* node);
void generateAssignmentExpression(const AssignmentExpressionNode* node);
void generateFunctionDeclarator(const DeclaratorNode* node);
void generateGaggleSpecifier(const GaggleSpecifierNode* node);
void generateConstantNode(const ConstantNode* node);
void generateCastExpression(const CastExpressionNode* node);
void generateJumpStatement(const JumpStatementNode* node);
void generateSelectionStatement(const SelectionStatementNode* node);
void generateExternalDeclaration(const ExternalDeclarationNode* node);
void generateDeclaration(const DeclarationNode* node);
void generateArguments(const ParameterListNode* node);
void generateParameterListNode(const ParameterListNode* node, int argumentIndex);
Type* generateTypeSpecifierInLocal(const TypeSpecifierNode* node);
Type* generateTypeSpecifierInGlobal(const TypeSpecifierNode* node);
int getArraySizeFromConstantExpression(const ConditionalExpressionNode* node);
bool isIntegerConstant(const AssignmentExpressionNode* node);
int alignOffset(int offset, int size);
void generateGlobalDeclaration(const DeclarationNode* node);

void asmgen(const TransUnitNode* node, char* outputFile, bool asmGeneration);
