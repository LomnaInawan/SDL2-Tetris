#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "core.h"

int fps = 120;
int windowWidth = 1280;
int windowHeight = 720;
char *_windowTitle = "Title";

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_Color _background = {0, 0, 0, 255};

Uint64 requiredDelta;
SDL_Event event;
int running = 1;

void SetBackgroundColor(SDL_Color color){
  _background = color;
}

void SetWindow(int width, int height, int game_fps, char *title){
  windowHeight = height;
  windowWidth = width;
  fps = game_fps;
  _windowTitle = title;
}

void mainloop(){
  printf("log 1");
  Uint64 startLoop = SDL_GetTicks();
  SDL_PollEvent(&event);
  if(event.type == SDL_QUIT){
    running = 0;
  }
  SDL_SetRenderDrawColor(renderer, _background.r, _background.g, _background.b, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  CheckForPressedKeys(); //Check for input
  PerFrameUpdate(); //The Update Function
  Render(); //Render the frame
  SDL_RenderPresent(renderer);

  Uint64 delta = SDL_GetTicks() - startLoop;
  if (delta < requiredDelta)
  {
    SDL_Delay(requiredDelta - delta);
  }
}

int main(int argc, char *argv[]){
  printf("Log 4");
  SDL_Init(SDL_INIT_VIDEO);

  SDL_CreateWindowAndRenderer(windowWidth,windowHeight, 0, &window, &renderer);
  StartDrawFunctions(renderer);
  //Need to work on adding this to the user functions
  //SDL_SetWindowGrab(window,SDL_TRUE);
  OnStart();
  SDL_SetWindowTitle(window, _windowTitle);

  SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
  StartInputFunctions(&event);
  requiredDelta = 1000/fps;

  printf("Log 2");
  #ifdef __EMSCRIPTEN__
    printf("Log 3");
    emscripten_set_main_loop(mainloop, 0, 1);
  #else
  while (running)
  {
    mainloop();
  }
  #endif

  SDL_Quit();
  return EXIT_SUCCESS;
}
