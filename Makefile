start:
	gcc -g ./src/core.c ./src/draw.c ./src/input.c ./src/basic-math.c ./src/game.c -I ./include -o ./build/test -lSDL2main -lSDL2
