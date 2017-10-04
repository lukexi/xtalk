all: xtalk

xtalk: xtalk.c
	clang xtalk.c -o xtalk -lSDL2 -lGLEW -lGL -lm
