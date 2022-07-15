#include "core.h"
#include <stdio.h>
#include <SDL2/SDL.h>

typedef struct{
    SDL_KeyCode kcode;
    bool pressed;
} inputStructure;

SDL_Event *ev;
inputStructure inputArray[50];
int inputArrayLen;

void StartInputFunctions(SDL_Event *event){
    ev = event;
    inputArray[0].kcode = SDLK_SPACE;
    inputArray[0].pressed = false;
    inputArrayLen++;
}

bool KeyIsDown(SDL_KeyCode kcode){
    if (ev->type == SDL_KEYDOWN && ev->key.keysym.sym == kcode)
    {
        return true;
    }
    return false;
}

bool KeyIsUp(SDL_KeyCode kcode){
    if (ev->type == SDL_KEYUP && ev->key.keysym.sym == kcode)
    {
        return true;
    }
    return false;
}

bool KeyIsPressed(SDL_KeyCode kcode){
    int index = -1;
    for(int i =0; i <inputArrayLen; i++){
        if (inputArray[i].kcode == kcode)
        {
            index = i;
            break;
        }
    }
    if(index == -1){
        inputArray[inputArrayLen].kcode = kcode;
        inputArray[inputArrayLen].pressed = false;
        index = inputArrayLen;
        inputArrayLen++;
    }
    return inputArray[index].pressed;
}

void CheckForPressedKeys(){
    for(int i = 0; i < inputArrayLen; i++){
        if(KeyIsDown(inputArray[i].kcode)){
            inputArray[i].pressed = true;
        }
        if(KeyIsUp(inputArray[i].kcode)){
            inputArray[i].pressed = false;
        }
    }
}
