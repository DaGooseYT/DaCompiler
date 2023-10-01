#include "util.h"

char* readFile(const char* path) {
    FILE* fp = fopen(path, "r");

    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* addr = calloc(fsize, sizeof(char) + 1);
    fread(addr, sizeof(char), fsize, fp);
    fclose(fp);

    return addr;
}

Vector* buildVectorList() {
    Vector* vectorList = malloc(sizeof(Vector));
    vectorList->contents = calloc(16, sizeof(void*));
    vectorList->capacity = 16;
    vectorList->size = 0;

    return vectorList;
}

void pushVector(Vector* vectorList, void* e) {
    if (vectorList->size == vectorList->capacity) {
        vectorList->capacity *= 2;
        vectorList->contents = realloc(vectorList->contents, sizeof(void*) * vectorList->capacity);
    }

    vectorList->contents[vectorList->size] = e;
    vectorList->size++;
}

Stack* buildStack() {
    Stack* stack = malloc(sizeof(Stack));
    stack->elements = calloc(16, sizeof(void*));
    stack->capacity = 16;
    stack->top = -1;

    return stack;
}

void pushStack(Stack* stack, void* e) {
    stack->top++;

    if (stack->top == stack->capacity) {
        stack->capacity *= 2;
        stack->elements = realloc(stack->elements, sizeof(void*) * stack->capacity);
    }

    stack->elements[stack->top] = e;
}

void* topOfStack(Stack* stack) {
    if (stack->top < 0) {
        return NULL;
    }

    return stack->elements[stack->top];
}

void popStack(Stack* stack) {
    stack->top--;
}

IntegerStack* initializeIntegerStack() {
    IntegerStack* stack = malloc(sizeof(Stack));
    stack->elements = malloc(sizeof(int) * 16);
    stack->capacity = 16;
    stack->top = -1;

    return stack;
}

void pushIntegerStack(IntegerStack* stack, int e) {
    stack->top++;

    if (stack->top == stack->capacity) {
        stack->capacity *= 2;
        stack->elements = realloc(stack->elements, sizeof(int) * stack->capacity);
    }

    stack->elements[stack->top] = e;
}

int topOfIntegerStack(IntegerStack* stack) {
    if (stack->top < 0) {
        printf("stack is empty!\n");
        exit(-1);
    }

    return stack->elements[stack->top];
}

void popIntegerStack(IntegerStack* stack) {
    stack->top--;
}

StringMap* buildStringMap(int capacity) {
    StringMap* map = malloc(sizeof(StringMap));
    map->size = 0;
    map->capacity = capacity;
    map->entries = calloc(capacity, sizeof(StringPointerMapEntry*));

    return map;
}

int calculateHash(const char* string) {
    int h = 0, pos = 0;

    while (string[pos] != '\0') {
        h += string[pos];
        pos++;
    }

    return h;
}

void appendStringMap(StringMap* map, const char* key, void* value) {
    const int hash = calculateHash(key);
    const int index = hash % map->capacity;

    if (map->entries[index] == NULL) {
        map->entries[index] = calloc(1, sizeof(StringPointerMapEntry));
        map->entries[index]->key = strdup(key);
        map->entries[index]->value = value;
        map->entries[index]->next = NULL;
    }
    else {
        StringPointerMapEntry* current = map->entries[index];

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = calloc(1, sizeof(StringPointerMapEntry));
        current->next->key = strdup(key);
        current->next->value = value;
        current->next->next = NULL;
    }

    map->size++;
}

bool stringMapContains(StringMap* map, const char* key) {
    const int hash = calculateHash(key);
    const int index = hash % map->capacity;

    if (map->entries[index] == NULL) {
        return false;
    }
    else {
        StringPointerMapEntry* current = map->entries[index];

        while (current != NULL) {
            if (strcmp(current->key, key) == 0) {
                return true;
            }

            current = current->next;
        }

        return false;
    }
}

void* getStringMap(StringMap* map, const char* key) {
    const int hash = calculateHash(key);
    const int index = hash % map->capacity;

    if (map->entries[index] == NULL) {
        return NULL;
    }
    else {
        StringPointerMapEntry* current = map->entries[index];

        while (current != NULL) {
            if (strcmp(current->key, key) == 0) {
                return current->value;
            }

            current = current->next;
        }

        return NULL;
    }
}

StringIntegerMap* buildStringIntegerMap(int capacity) {
    StringIntegerMap* map = malloc(sizeof(StringIntegerMap));
    map->size = 0;
    map->capacity = capacity;
    map->entries = calloc(capacity, sizeof(StringIntegerMapEntry*));

    return map;
}

void appendStringIntegerMap(StringIntegerMap* map, const char* key, int value) {
    const int hash = calculateHash(key);
    const int index = hash % map->capacity;

    if (map->entries[index] == NULL) {
        map->entries[index] = calloc(1, sizeof(StringIntegerMapEntry));
        map->entries[index]->key = strdup(key);
        map->entries[index]->value = value;
        map->entries[index]->next = NULL;
    }
    else {
        StringIntegerMapEntry* current = map->entries[index];

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = calloc(1, sizeof(StringIntegerMapEntry));
        current->next->key = strdup(key);
        current->next->value = value;
        current->next->next = NULL;
    }

    map->size++;
}

bool stringIntegerMapContains(StringIntegerMap* map, const char* key) {
    const int hash  = calculateHash(key);
    const int index = hash % map->capacity;

    if (map->entries[index] == NULL) {
        return false;
    }
    else {
        StringIntegerMapEntry* current = map->entries[index];

        while (current != NULL) {
            if (strcmp(current->key, key) == 0) {
                return true;
            }

            current = current->next;
        }
        return false;
    }
}

int getIntegerMap(StringIntegerMap* map, const char* key) {
    const int hash  = calculateHash(key);
    const int index = hash % map->capacity;

    if (map->entries[index] == NULL) {
        printf("no key=\"%s\"\n", key);
        exit(-1);
    }
    else {
        StringIntegerMapEntry* current = map->entries[index];

        while (current != NULL) {
            if (strcmp(current->key, key) == 0) {
                return current->value;
            }

            current = current->next;
        }

        printf("no key=\"%s\"\n", key);
        exit(-1);
    }
}
