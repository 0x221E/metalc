## Description
------------------
This compiler is meant to be a full os dev-
elopment toolchain. Replaces the function-
ality of GNU/make, GNU/bash, GNU/gcc, GNU/ld
and combines its functionality for an easier
OS-development experience.

"What you see is what you get"

This compiler doubles as AOT shell on my own
operating system.

If the C language and this compiler has the
keyword, the functionality of that keyword
is self-explanatory. If not, or it differs,
a description is provided below.

There is no floating point support.

Syntax of this language is deterministic,
yet similar to C, removing function call
and parantheses ambiguity through the
"fn" keyword.

For example;

fn g64 test() {
    <contents>
    return;
}

This way, function pointer declarations also 
become deterministic.

E.g.,

fn g64 *ptr_test = &test;

This compiler does not accept ambigous
parsing.

## Error Levels
------------------
- Warning 
Just fix the warnings, they are specific 
for your purposes. 

- Error 
Code does not produce functional outcome.

- No Error & No Warning
Code produces functional and architecturally
correct outcome, however, said function may 
not be the intended one.

This compiler does not guarantee "correct
code" in the sense that it will 100% result
in intended functionality.

## Error Types
------------------
- Syntax Error
- Token Error

## Keywords
------------------
- if
- else
- elif
- while
- for
- continue
- break
- asm

## Data Types
------------------
- g<8/16/32/64>
- u<8/16/32/64>
- s<8/16/32/64>

## Operators
------------------
- Arithmetic; + - * / %
- Comparison; == != > < => <=
- Bit; & | ^ << >> ~

## Link-time processing units
------------------
Starts with "$" and instructs the layout of
the produced binary.

- $region "attr" ""extended attr""

## Directives
------------------
Starts with "#" and either manipulates the 
outer environment that the compiler lives in, 
or manipulates code depending on the environment.

Functionality depends on platform.

All Directives:

- #run "<command>"
Runs the following command in the environment
when compiling.

- #inject FILE/BYTE "<file_path/bytes>" "<addr/symbol>"
Injects the specified file into the compiled
binary at the specified location. 

## g<8/16/32/64>
------------------
This keyword, when used, does not force any
bit composition to the value like signed or
unsigned or zeroed.

## u<8/16/32/64>
------------------
This keyword is similar to g, however, it
forces unsigned composition to the loaded
value.

## s<8/16/32/64>
------------------
Forces signed composition.

## region "name" "attr" ""extended attr""
------------------
Puts the following code, unless otherwise
specified, to the specified data region.

This keyword has different functionality
depending on the used platform.

## asm
------------------
This keyword declares an assembly block

