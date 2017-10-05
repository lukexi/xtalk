all: xtalk

xtalk: xtalk.c edid.c
	clang xtalk.c edid.c -o xtalk -lSDL2 -lGLEW -lGL -lm -lX11 -lXrandr
