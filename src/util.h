#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

typedef struct Vector {
    void** contents;
    int size;
    int capacity;
} Vector;

typedef struct Stack {
    void** elements;
    int top;
    int capacity;
} Stack;

typedef struct StringPointerMapEntry {
    char* key;
    void* value;
    struct StringPointerMapEntry* next;
} StringPointerMapEntry;

typedef struct StringMap {
    int size;
    int capacity;
    StringPointerMapEntry** entries;
} StringMap;

typedef struct IntegerStack {
    int* elements;
    int top;
    int capacity;
} IntegerStack;

typedef struct StringIntegerMapEntry {
    char* key;
    int value;
    struct StringIntegerMapEntry* next;
} StringIntegerMapEntry;

typedef struct StringIntegerMap {
    int size;
    int capacity;
    StringIntegerMapEntry** entries;
} StringIntegerMap;

Vector* buildVectorList();
Stack* buildStack();
IntegerStack* initializeIntegerStack();
StringMap* buildStringMap(int capacity);
StringIntegerMap* buildStringIntegerMap(int capacity);

char* readFile(const char* path);
void pushVector(Vector* vec, void* e);
void pushStack(Stack* stack, void* e);
void* topOfStack(Stack* stack);
void popStack(Stack* stack);
void pushIntegerStack(IntegerStack* stack, int e);
int topOfIntegerStack(IntegerStack* stack);
int calculateHash(const char* string);
void popIntegerStack(IntegerStack* stack);
void appendStringMap(StringMap* map, const char* key, void* value);
bool stringMapContains(StringMap* map, const char* key);
void* getStringMap(StringMap* map, const char* key);
void appendStringIntegerMap(StringIntegerMap* map, const char* key, int value);
bool stringIntegerMapContains(StringIntegerMap* map, const char* key);
int getIntegerMap(StringIntegerMap* map, const char* key);