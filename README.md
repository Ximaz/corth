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
```console
$ make && ./corth sim <filename> # Simulate the program
$ make && ./corth com <filename> # Compile the program
$ ./output                       # Run the program
```
Be aware the Makefile compiles to [ELF64](https://fr.wikipedia.org/wiki/Executable_and_Linkable_Format) using [NASM](https://www.nasm.us/) and [ld](https://www.gnu.org/software/binutils/).

If you have other installed, you may change the Makefile to apply them, or install NASM to compile with the default Makefile.

## Documentation

### Arithmetics
- Any number : pushes the number onto the stack.
```
a = 69
push(stack, a)
```
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
- `!=`, `=`, `<`, `<=`, `>=` and `>` : pops two elements from the top of the stack, compare them, and pushes either `0` if the condition is not fulfilled or `1` if it is.

### Control Flow
- `if <then-branch> else <else-branch> end` : pops the element at the top of the stack and check if it's value is not `0` to execute the `<then-branch>`, otherwise it runs the `<else-branch>`.
- `while <condition> do <body> end` : keeps executing both `<condition>` and `<body>` until `<condition>` produces `0` at the top of the stack. To check the result of the `<condition>`, the last element of the stack is poped.
