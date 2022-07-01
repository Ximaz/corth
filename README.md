# Corth
It's like [Porth](https://gitlab.com/tsoding/porth), which itself is like Forth, but in C. This project came in my brain when I discovered [Tsoding](https://www.youtube.com/c/TsodingDaily) and it's [Porth videos](https://www.youtube.com/watch?v=8QP2fDBIxjM&list=PLpM-Dvs8t0VbMZA7wW9aR3EtBqe2kinu4). I have to practice C for school purposes, so why not doing so while creating (rather copying) a programming language ? At least, I can work with files, error handling, debugging, assembly code, etc ... In addition, it will help me to understand how a programming language does work. I share this project to you the same way Tsoding did for me.

## Roadmap
Corth is planned to be :
- [X] Compiled
- [X] Native
- [X] Stack based (just like Porth and Forth)
- [ ] Turing-complete
- [ ] Statically typed
- [ ] Self-hosted

## TODO
- [X] Fix the lexer about handling `\r\n` (Win) **AND** `\n` (Unix)
- [ ] Fix the compiler about printing negative numbers. (Ex : `-10` -> `/0`)

## Quick Start

### Simulation
Simulation simply interprets the program.
```console
$ cat program.corth
34 35 + .
$ ./corth sim program.corth
69
```

### Compilation
Compilation generates assembly code which can be compiled using [NASM](https://www.nasm.us/) and linked with [ld](https://www.gnu.org/software/binutils/). So make sure you have them installed.
```console
$ cat program.corth
34 35 + .
$ ./corth com program.corth
$ nasm -felf64 output.asm -o output.o
$ ld output.o -o output
$ ./output
69
```

## Language Reference
This is what the language supports so far. **Since the language is a work in progress, some operations are subject to change.**

### Stack manipulation
- `<integer>` : pushes the integer onto the stack. Right now, it's the only kind of thing that can be parsed by the lexer as number.
```
push(stack, <integer>)
```
- `dup` : duplicates an element on the top of the stack.
```
a = pop(stack)
push(stack, a)
push(stack, a)
```
- `dump` : pops the element at the top of the stack to print it to the stdout.
```
a = pop(stack)
print(a)
```

### Arithmetics
- `+` : sums up two elements at the top of the stack.
```
a = pop(stack)
b = pop(stack)
push(stack, a + b)
```
- `-` : substracts two elements at the top of the stack.
```
a = pop(stack)
b = pop(stack)
push(stack, b - a)
```

### Comparisons
- `!=`, `=`, `<`, `<=`, `>=` and `>` : pops two elements from the top of the stack, compares them, and pushes either `0` if the condition is not fulfilled or `1` if it is.

### Control Flow
- `if <then-branch> else <else-branch> end` : pops the element at the top of the stack and check if it's value is not `0` to execute the `<then-branch>`, otherwise it runs the `<else-branch>`.
- `while <condition> do <body> end` : keeps executing both `<condition>` and `<body>` until `<condition>` produces `0` at the top of the stack. To check the result of the `<condition>`, the last element of the stack is poped.
- `halt` : pops the elements at the top of the stack as an [exit status](https://en.wikipedia.org/wiki/Exit_status), then quits the program.

### Memory
- `mem` : pushes the address of the beginning of the readable/writable memory onto the stack.
- `store` : pops two elements at the top of the stack which represent the byte to store (first pop) and then the `mem` address to which the byte will be stored (second pop).
- `load` : pops the element at the top of the stack that contains the `mem` address to which the byte should be extracted from, and pushes it onto the stack.
