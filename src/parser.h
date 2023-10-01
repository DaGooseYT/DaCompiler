#pragma once

#include "lexer.h"

typedef struct AsmStatementNode AsmStatementNode;
typedef struct UnaryExpressionNode UnaryExpressionNode;
typedef struct CastExpressionNode CastExpressionNode;
typedef struct ConditionalExpressionNode ConditionalExpressionNode;
typedef struct TransUnitNode TransUnitNode;
typedef struct AssignmentExpressionNode AssignmentExpressionNode;
typedef struct ExpressionNode ExpressionNode;
typedef struct PrimaryExpressionNode PrimaryExpressionNode;
typedef struct MultiplicationExpressionNode MultiplicationExpressionNode;
typedef struct AdditionExpressionNode AdditionExpressionNode;
typedef struct PostfixExpressionNode PostfixExpressionNode;
typedef struct ShiftExpressionNode ShiftExpressionNode;
typedef struct RelationalExpressionNode RelationalExpressionNode;
typedef struct EqualExpressionNode EqualExpressionNode;
typedef struct AndExpressionNode AndExpressionNode;
typedef struct ExclusiveOrExpressionNode ExclusiveOrExpressionNode;
typedef struct InclusiveOrExpressionNode InclusiveOrExpressionNode;
typedef struct LogicalAndExpressionNode LogicalAndExpressionNode;
typedef struct ExternalStatementNode ExternalStatementNode;
typedef struct OrExpressionNode OrExpressionNode;
typedef struct DirectDeclaratorNode DirectDeclaratorNode;
typedef struct DeclaratorNode DeclaratorNode;
typedef struct FunctionDefinitionNode FunctionDefinitionNode;
typedef struct ExternalDeclarationNode ExternalDeclarationNode;
typedef struct FlockSpecifierNode FlockSpecifierNode;
typedef struct TypeSpecifierNode TypeSpecifierNode;
typedef struct DeclarationSpecifierNode DeclarationSpecifierNode;
typedef struct SpecifierQualifierNode SpecifierQualifierNode;
typedef struct FlockDeclarationNode FlockDeclarationNode;
typedef struct TypeNameNode TypeNameNode;
typedef struct ParameterTypeListNode ParameterTypeListNode;
typedef struct ParameterListNode ParameterListNode;
typedef struct ParameterDeclarationNode ParameterDeclarationNode;
typedef struct GaggleSpecifierNode GaggleSpecifierNode;
typedef struct GaggleListNode GaggleListNode;
typedef struct DeclarationNode DeclarationNode;
typedef struct InitializeDeclaratorNode InitializeDeclaratorNode;
typedef struct InitializerNode InitializerNode;
typedef struct InitializerListNode InitializerListNode;
typedef struct CompoundStatementNode CompoundStatementNode;
typedef struct BlockItemNode BlockItemNode;
typedef struct StatementNode StatementNode;
typedef struct LabelStatementNode LabelStatementNode;
typedef struct ExpressionStatementNode ExpressionStatementNode;
typedef struct JumpStatementNode JumpStatementNode;
typedef struct SelectionStatementNode SelectionStatementNode;
typedef struct IterationStatementNode IterationStatementNode;

typedef struct PointerNode {
    int count;
} PointerNode;

typedef struct ConstantNode {
    int constantType;
    int integerConstant;
    int gaggleConstant;
    char* characterConstant;  
} ConstantNode; 

struct TransUnitNode {
    Vector* externalDeclarationNodes;
};

struct AssignmentExpressionNode {
    int assignOperator;
    ConditionalExpressionNode* conditionalExpressionNode;
    UnaryExpressionNode* unaryExpressionNode;
    AssignmentExpressionNode* assignExpressionNode;
};

struct ExpressionNode {
    AssignmentExpressionNode* assignExpressionNode;
    ExpressionNode* expressionNode;
};

struct PrimaryExpressionNode {
    char* str;
    char* identifier;
    bool isExternalFunction;
    int valueType;
    ConstantNode* constantNode;
    ExpressionNode* expressionNode;
};

struct PostfixExpressionNode {
    char* identifier;
    int postfixExpressionType;
    Vector* assignExpressionNodes;
    PrimaryExpressionNode* primaryExpressionNode;
    PostfixExpressionNode* postfixExpressionNode;
    ExpressionNode* expressionNode;
};

struct UnaryExpressionNode {
    int type;
    int operatorType;
    char* sizeName;
    PostfixExpressionNode* postfixExpressionNode;
    UnaryExpressionNode* unaryExpressionNode;
    CastExpressionNode* castExpressionNode;
    TypeNameNode* typeNameNode;
};

struct CastExpressionNode {
    UnaryExpressionNode* unaryExpressionNode;
    CastExpressionNode* castExpressionNode;
    TypeNameNode* typeNameNode;
}; 

struct MultiplicationExpressionNode {
    int operatorType;
    CastExpressionNode* castExpressionNode;
    MultiplicationExpressionNode* multiplicationExpressionNode;
};

struct AdditionExpressionNode {
    int operatorType;
    MultiplicationExpressionNode* multiplicationExpressionNode;
    AdditionExpressionNode* additionExpressionNode;
};

struct ShiftExpressionNode {
    AdditionExpressionNode* additionExpressionNode;
    ShiftExpressionNode* shiftExpressionNode;
};

struct RelationalExpressionNode {
    int compareType;
    ShiftExpressionNode* shiftExpressionNode;
    RelationalExpressionNode* relationalExpressionNode;
};

struct EqualExpressionNode {
    int compareType;
    RelationalExpressionNode* relationalExpressionNode;
    EqualExpressionNode* equalExpressionNode;
};

struct AndExpressionNode {
    EqualExpressionNode* equalExpressionNode;
    AndExpressionNode* andExpressionNode;
};

struct ExclusiveOrExpressionNode {
    AndExpressionNode* andExpressionNode;
    ExclusiveOrExpressionNode* exclusiveOrExpressionNode;
};

struct InclusiveOrExpressionNode {
    ExclusiveOrExpressionNode* exclusiveOrExpressionNode;
    InclusiveOrExpressionNode* inclusiveOrExpressionNode;
};

struct LogicalAndExpressionNode {
    InclusiveOrExpressionNode* inclusiveOrExpressionNode;
    LogicalAndExpressionNode* logicalAndExpressionNode;
};

struct OrExpressionNode {
    LogicalAndExpressionNode* logicalAndExpressionNode;
    OrExpressionNode* orExpressionNode;
};

struct ConditionalExpressionNode {
    OrExpressionNode* orExpressionNode;
    ExpressionNode* expressionNode;
    ConditionalExpressionNode* conditionalExpressionNode;
};

struct DirectDeclaratorNode {
    char* identifier;
    Vector* identifierList;
    DeclaratorNode* declaratorNode;
    DirectDeclaratorNode* directDeclaratorNode;
    ConditionalExpressionNode* conditionalExpressionNode;
    ParameterTypeListNode* parameterTypeListNode;
};

struct DeclaratorNode {
    PointerNode* pointerNode;
    DirectDeclaratorNode* directDeclaratorNode;     
};

struct FunctionDefinitionNode {
    Vector* declarationSpecifierNodes;
    DeclaratorNode* declaratorNode;
    CompoundStatementNode* compoundStatementNode;
};

struct ExternalDeclarationNode {
    FunctionDefinitionNode* functionDefinitionNode;
    DeclarationNode* declarationNode;
    GaggleSpecifierNode* gaggleSpecifierNode;
};

struct FlockSpecifierNode {
    char* identifier;
    Vector* flockDeclarationNodes; 
};

struct TypeSpecifierNode {
    char* flockName;
    int typeSpecifier;
    FlockSpecifierNode* flockSpecifierNode;
};

struct DeclarationSpecifierNode {
    bool isConstant;
    TypeSpecifierNode* typeSpecifierNode;
};

struct FlockDeclarationNode {
    char* identifier;
    Vector* specifierQualifierNodes;
    PointerNode* pointerNode;
};

struct SpecifierQualifierNode {
    bool isConstant;
    TypeSpecifierNode* typeSpecifierNode;
};

struct TypeNameNode {
    bool isPointer;
    SpecifierQualifierNode* specifierQualifierNode; 
};

struct ParameterTypeListNode {
    ParameterListNode* parameterListNode; 
};

struct ParameterListNode {
    ParameterDeclarationNode* parameterDeclarationNode;
    ParameterListNode* parameterListNode;  
};

struct ParameterDeclarationNode {
    Vector* declaratorSpecifierNodes;
    DeclaratorNode* declaratorNode;
};

struct GaggleSpecifierNode {
    char* identifier;
    GaggleListNode* gaggleListNode;
};

struct GaggleListNode {
    Vector* identifiers;
};

struct DeclarationNode {
    Vector* declarationSpecifierNodes;
    Vector* initializeDeclaratorNodes;
};

struct InitializeDeclaratorNode {
    DeclaratorNode* declaratorNode;
    InitializerNode* initializerNode;
};

struct InitializerNode {
    AssignmentExpressionNode* assignmentExpressionNode;
    InitializerListNode* initializerListNode;
};

struct InitializerListNode {
    Vector* initializerNodes;        
};

struct CompoundStatementNode {
    Vector* blockItemNodes;
};

struct BlockItemNode {
    DeclarationNode* declarationNode;
    StatementNode* statementNode;
};

struct ExternalStatementNode {
    ExternalStatementNode* externalStatementNode;
};

struct StatementNode {
    AsmStatementNode* asmStatementNode;
    LabelStatementNode* labeledStatementNode;
    ExpressionStatementNode* expressionStatementNode; 
    CompoundStatementNode* compoundStatementNode;
    SelectionStatementNode* selectionStatementNode;
    IterationStatementNode* iterationStatementNode;
    JumpStatementNode* jumpStatementNode;
};

struct LabelStatementNode {
    int labeledStatementType;
    ConditionalExpressionNode* conditionalExpressionNode;
    StatementNode* statementNode;
};

struct ExpressionStatementNode {
    ExpressionNode* expressionNode;  
};

struct SelectionStatementNode {
    int selectionType;
    ExpressionNode* expressionNode;
    StatementNode* statementNode1;
    StatementNode* statementNode2;
};

struct IterationStatementNode {
    int type;
    Vector* declarationNodes;
    StatementNode* statementNode;
    ExpressionNode* expressionNode1;
    ExpressionNode* expressionNode2;
    ExpressionNode* expressionNode3;
};

struct JumpStatementNode {
    int type;
    char* identifier;
    ExpressionNode* expressionNode;
};

struct AsmStatementNode {
    ConstantNode* constantNode;
    ExpressionNode* expressionNode;
};

enum TypeSpecifier {
    TYPE_NONE,
    TYPE_UI0,
    TYPE_SC8,
    TYPE_SI32,
    TYPE_FP64,
    TYPE_B8,
    TYPE_FLOCK,
    TYPE_GAGGLE,
    TYPENAME
};

enum OperatorType {
    OPERATOR_NONE,
    OPERATOR_ASSIGN,
    OPERATOR_MULTIPLY_EQUAL,
    OPERATOR_DIVIDE_EQUAL,
    OPERATOR_MODULO_EQUAL,
    OPERATOR_ADD_EQUAL,
    OPERATOR_SUBTRACT_EQUAL,
    OPERATOR_AND,
    OPERATOR_ADD,
    OPERATOR_SUBTRACT,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_MODULO,
    OPERATOR_EXCLAMATION
};

enum ComparisonOperatorType {
    COMPARE_NONE,
    COMPARE_LESS_THAN,
    COMPARE_GREATER_THAN,
    COMPARE_EQUAL,
    COMPARE_NOT_EQUAL,
    COMPARE_LESS_OR_EQUAL,
    COMPARE_GREATER_OR_EQUAL
};

enum ConstantType {
    CONSTANT_SI32,
    CONSTANT_BYTE,
    CONSTANT_STRING,
    CONSTANT_FP64
};

enum JumpType {
    JUMP_CONTINUE,
    JUMP_STOP,
    JUMP_RETURN
};

enum UnaryType {
    UNARY_NONE,
    UNARY_INCREMENT,
    UNARY_DECREMENT,
    UNARY_OPERATOR,
    UNARY_SIZE_IDENTIFIER,
    UNARY_SIZE_TYPE
};

enum SizeType {
    SIZE_SC8,
    SIZE_SI32,
    SIZE_FP64,
    SIZE_IDENTIFIER
};

enum PostfixType {
    POSTFIX_PRIMARY,
    POSTFIX_LEFT_SQUARE,
    POSTFIX_LEFT_PARENTHESIS,
    POSTFIX_DOT,
    POSTFIX_POINTER,
    POSTFIX_INCREMENT,
    POSTFIX_DECREMENT
};

enum SelectionStatementType {
    SELECTION_IF,
    SELECTION_IF_ELSE,
    SELECTION_MATCH
};

enum IterationType {
    ITERATION_WHILE,
    ITERATION_FOR
};

enum LabeledStatementType {
    LABEL_CASE,
    LABEL_DEFAULT
};

StringMap* classMap;
bool isExternFunction;

TransUnitNode* parse(const Vector* vectorList);

ExpressionNode* makeExpressionNode(const Vector* vectorList, int* index);
AssignmentExpressionNode* makeAssignmentExpressionNode(const Vector* vectorList, int* index);
DeclarationSpecifierNode* makeDeclarationSpecifierNode(const Vector* vectorList, int* index);
DeclaratorNode* makeDeclaratorNode(const Vector* vectorList, int* index);
DeclarationNode* makeDeclarationNode(const Vector* vectorList, int* index);
StatementNode* makeStatementNode(const Vector* vectorList, int* index);
ConstantNode* makeConstantNode(const Vector* vectorList, int* index);
CompoundStatementNode* makeCompoundStatementNode(const Vector* vectorList, int* index);
SelectionStatementNode* makeSelectionStatementNode(const Vector* vectorList, int* index);
ExpressionStatementNode* makeExpressionStatementNode(const Vector* vectorList, int* index);
LabelStatementNode* makeLabelStatementNode(const Vector* vectorList, int* index);
CastExpressionNode* makeCastExpressionNode(const Vector* vectorList, int* index);
TypeNameNode* makeTypeNameNode(const Vector* vectorList, int* index);
JumpStatementNode* makeJumpStatementNode(const Vector* vectorList, int* index);
IterationStatementNode* makeIterationStatementNode(const Vector* vectorList, int* index);
AsmStatementNode* makeAsmStatementNode(const Vector* vectorList, int* index);
TypeSpecifierNode* makeTypeSpecifierNode(const Vector* vectorList, int* index);
InitializerNode* makeInitializerNode(const Vector* vectorList, int* index);
SpecifierQualifierNode* makeSpecifierQualifierNode(const Vector* vectorList, int* index);
FlockDeclarationNode* makeFlockDeclarationNode(const Vector* vectorList, int* index);
ExternalDeclarationNode* makeExternalDeclarationNode(const Vector* vectorList, int* index);
DirectDeclaratorNode* makeDirectDeclaratorNode(const Vector* vectorList, int* index);
ParameterTypeListNode* makeParameterTypeListNode(const Vector* vectorList, int* index);
InitializeDeclaratorNode* makeInitializingDeclaratorNode(const Vector* vectorList, int* index);
UnaryExpressionNode* makeUnaryExpressionNode(const Vector* vectorList, int* index);
MultiplicationExpressionNode* makeMultiplicationExpressionNode(const Vector* vectorList, int* index);
FunctionDefinitionNode* makeFunctionDefinitionNode(const Vector* vectorList, int* index);
ShiftExpressionNode* makeShiftExpression(const Vector* vectorList, int* index);
RelationalExpressionNode* makeRelationalExpressionNode(const Vector* vectorList, int* index);
EqualExpressionNode* makeEqualExpressionNode(const Vector* vectorList, int* index);
AndExpressionNode* makeAndExpressionNode(const Vector* vectorList, int* index);
BlockItemNode* makeBlockItemNode(const Vector* vectorList, int* index);
InclusiveOrExpressionNode* makeInclusiveOrExpressionNode(const Vector* vectorList, int* index);
LogicalAndExpressionNode* makeLogicalAndExpressionNode(const Vector* vectorList, int* index);
TypeSpecifierNode* makeTypeSpecifierNode(const Vector* vectorList, int* index);
OrExpressionNode* makeLogicalOrExpressionNode(const Vector* vectorList, int* index);
ConditionalExpressionNode* makeConditionalExpressionNode(const Vector* vectorList, int* index);
GaggleSpecifierNode* makeGaggleSpecifierNode(const Vector* vectorList, int* index);
PostfixExpressionNode* makePostfixExpressionNode(const Vector* vectorList, int* index);
PointerNode* makePointerNode(const Vector* vectorList, int* index);
PrimaryExpressionNode* makePrimaryExpressionNode(const Vector* vectorList, int* index);
ExclusiveOrExpressionNode* makeExclusiveOrExpressionNode(const Vector* vectorList, int* index);
AdditionExpressionNode* makeAdditiveExpressionNode(const Vector* vectorList, int* index);
ParameterDeclarationNode* makeParameterDeclarationNode(const Vector* vectorList, int* index);

bool isFunctionDefinition(const Vector* vectorList, int index);
bool isDeclarator(const Vector* vectorList, int index);
bool isFunctionDeclaration(const Vector* vectorList, int index);
bool isDeclarationSpecifier(const Vector* vectorList, int index);
bool isTypeSpecifier(const Vector* vectorList, int index);

//
// debug
//
