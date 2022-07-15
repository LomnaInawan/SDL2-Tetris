# SDL2-Tetris
A tetris clone in C using SDL2.
![](https://img.itch.zone/aW1hZ2UvMTM0OTQxOC85NDU3MDg0LmdpZg==/250x600/dTktv9.gif)

## Dependency
[SDL2](https://www.libsdl.org)

## Compiling
### Linux
After installing SDL, make a folder named build and use the command

    gcc ./src/core.c ./src/draw.c ./src/input.c ./src/basic-math.c ./src/game.c -I ./include -o ./build/test -lSDL2main -lSDL2

The compiled binary is in the build folder.
Before running the program copy assets folder into the same directory as the compiled binary.

### Web
The game has been compiled for web using [emscripten.](https://emscripten.org/) The version used is 3.0.0. It is playable on [itch.](https://lomna.itch.io/tetris-sdl) To compile the for web, make a new folder build-web and use the command.

    emccc -o ./build-web/index.html ./src/core.c ./src/draw.c ./src/input.c ./src/basic-math.c ./src/game.c -I ./include -s USE_SDL=1 --preload-file assets
