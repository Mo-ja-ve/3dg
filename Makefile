all:
	g++ -Iheaders/Include -Lheaders/lib -o main -O0 main.cpp -lmingw32 -lSDL2main -lSDL2

static:
	g++ -c headers/include/SDL2/SDL.h -o lib_SDL.o
	g++ -c main.cpp -o main.o
	ar rcs headers/lib/libSDL2.a headers/lib/libSDL2main.a lib_SDL.o 
	g++ -o main main.o -L. headers/lib/libSDL2.a headers/lib/libSDL2main.a -w -Wl,-subsystem,windows -lmingw32

debug:
	g++ -Iheaders/Include -Lheaders/lib -o main -g -O0 main.cpp -lmingw32 -lSDL2main -lSDL2 