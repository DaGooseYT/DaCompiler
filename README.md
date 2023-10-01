# DaCompiler
A front-end Unix x86-64 compiler for the Goose programming language.

## Features
- Fast compilation
- Compiles to intel-syntax x86 assembly and uses NASM to assemble
- Goose code has a C-like syntax
- Can implement C standard library functions in Goose code
- Can output assembly files for advanced debugging

## Goose code syntax
Basic hello world program (only 3 lines!):<br />
```
UI0 main[] {
    extern::printf["Hello world!\n"];
}
```
<br />
In Goose code, function arguments and parameters are encapsulated with the square brackets. <br />
Arrays uses parenthesis. <br />
Stucts are called "flock". <br />
Enums are called "gaggle". <br />
Int is "SI32". <br />
Double is "FP64". <br />
Boolean is "B8". <br />
Char is "SC8". <br />
Void is "UI0". <br />
Switch statement is called "match". <br />
The pre-processor directive symbol is "~". "~using FILE.goose" includes external .goose code file. "~macro NAME VALUE" is a pre-processor macro constant.

## Usage
DaCompiler was made with simple compilation arguments in mind:<br />
```
Syntax: dacompiler [OPTION] <input>
Options:
     --asm                output assembly file
     --output <output>    output file name (without extension)
```

## TODO
- Change LR parsing to operator precedence based parsing
- Use stack arguments over register arguments (this is possible)
- Make a code optimizer
- Make a custom standard library
- Make a back-end assembler
