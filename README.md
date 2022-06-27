# Corth
It's like [Porth](https://gitlab.com/tsoding/porth), which itself is like Forth, but in C. This project came in my brain when I discovered [Tsoding](https://www.youtube.com/c/TsodingDaily) and it's [Porth videos](https://www.youtube.com/watch?v=8QP2fDBIxjM&list=PLpM-Dvs8t0VbMZA7wW9aR3EtBqe2kinu4) . I have to practice C for school purposes, so why not doing so while creating (rather copying) a programming language ? At least, I can work with files, error handling, debugging, assembly code, etc ... In addition, it will help me to understand how a programming language does work. I share this project to you the same way Tsoding did for me.

## Quick Start

```console
$ make && ./corth sim # Simulate the program
$ make && ./corth com # Compile the program
$ ./output            # Run the program
```

Be aware the Makefile compiles to ELF64 using NASM and ld. If you have other installed, you may change the Makefile to apply them, or install NASM to compile with the default Makefile.
