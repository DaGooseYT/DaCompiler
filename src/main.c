#include "lexer.h"
#include "parser.h"
#include "asmgen.h"
#include "preprocessor.h"
#include "util.h"

void syntax() {
    printf("Syntax: dacompiler [OPTION] <input>\nOptions:\n     --asm                output assembly file\n     --output <output>    output file name (without extension)\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        syntax();
        return -1;
    }

    bool asmGeneration = false;
    int index = 0;
    char* outputFile;

    if (strcmp("--asm", argv[1]) == 0) {
        asmGeneration = true;

        if (strcmp("--output", argv[2]) == 0) {
            outputFile = malloc(sizeof(char) + strlen(argv[3]));
            outputFile = argv[3];
            index = 4;

            if (argc < 5) {
                printf("error: insufficient arguments.\n");
                syntax();
                return -1;
            }
            else if (argc > 5) {
                printf("error: too many arguments specified.\n");
                syntax();
                return -1;
            }
        }
        else {
            printf("error: output file is not specified.\n");
            return -1;
        }
    }
    else if (strcmp("--output", argv[1]) == 0) {
        index = 3;
        outputFile = malloc(sizeof(char) + strlen(argv[2]));
        outputFile = argv[2];

        if (strcmp("--asm", argv[3]) == 0) {
            index = 4;
            asmGeneration = true;

            if (argc < 5) {
                printf("error: insufficient arguments.\n");
                syntax();
                return -1;
            }
            else if (argc > 5) {
                printf("error: too many arguments specified.\n");
                syntax();
                return -1;
            }
        }
        else if (argc < 4) {
            printf("error: insufficient arguments.\n");
            syntax();
            return -1;
        }
        else if (argc > 4) {
            printf("error: too many arguments specified.\n");
            syntax();
            return -1;
        }
    }
    else {
        printf("error: invalid argument: %s\n", argv[1]);
        syntax();
        return -1;
    }

    char* addr = readFile(argv[index]);

    if (addr == NULL) {
        printf("error: could not load file.\n"); 
        return -1;
    }

    const Vector* vector = lex(addr);

    if (vector == NULL) {
        printf("error: could not tokenize file.\n");
        return -1;
    }

    const Vector* preprocessedVector = preprocess(vector);

    if (preprocessedVector == NULL) {
        printf("error: could not preprocess.\n");
        return -1;
    }

    const TransUnitNode* node = parse(preprocessedVector);

    if (node == NULL) {
        printf("error: could not parse.\n");
        return -1;
    }

    asmgen(node, outputFile, asmGeneration);

    if (asmGeneration) {
        printf("output assembly file \"%s.asm\" generated successfully.\n", outputFile);
    }
    else {
        // assemble object file.
        if (system("nasm -f elf64 temp.asm") != 0) {
            printf("error: compilation failed. unable to assemble source file. assembly code file \"temp.asm\" was auto generated for debugging.\n");
            return -1;
        }

        char* str = malloc(strlen(outputFile) + 27);
        sprintf(str, "gcc -o %s -static temp.o", outputFile); // gcc used to invoke linker ONLY. it is not the backend!
        
        if (system(str) == 0) {
            remove("temp.asm");
        }
        else {
            printf("error: compilation failed. unable to link object file. assembly code file \"temp.asm\" was auto generated for debugging.\n");
            return -1;
        }

        free(str);
    }

    return 0;
}
