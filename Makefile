all:
	g++ -Iheaders/Include -Lheaders/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2

debug:
	g++ -Iheaders/Include -Lheaders/lib -o main -g main.cpp -lmingw32 -lSDL2main -lSDL2