LLVM = D:\Program Files\LLVM-MinGW\bin\clang.exe
OUT_FILE = out/app.exe
FLAGS = -o ${OUT_FILE} $(wildcard src/*.c) $(wildcard test/*.c) $(wildcard lib/**/*.c) -lm -I. -std=c11 -Wall

release:
	${LLVM} ${FLAGS}

debug:
	${LLVM} -g ${FLAGS}

macro:
	${LLVM} src/main.c -E
