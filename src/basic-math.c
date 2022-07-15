#include <stdio.h>
#include "core.h"

const double pi = 355.0 / 113.0;

//General Maths functions
int absolute(int a){
    if (a >= 0) return a;
    else return -a;
}

int roundOff(double d){
    int a =  (int) d;
    if (d - a >= 0.5 && d >= 0) a++;
    else if (d - a >= 0.5 && d < 0) a--;
    return a;
}

double distBetweenPoints(SDL_Point a, SDL_Point b){
    double c = ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
    c =  SDL_sqrt(c);
    return c;
}

SDL_Point addPoints(SDL_Point a, SDL_Point b){
    return newPoint(a.x + b.x, a.y + b.y);
}

vector2D addVectors(vector2D a, vector2D b){
    return newVector(a.x + b.x, a.y + b.y);
}

//This function returns angle in radians
double getVectorAngle(vector2D v){
    double value = v.x / SDL_sqrt(v.x * v.x + v.y * v.y);
    double angle = SDL_acos(value);
    if (v.y >= 0) return angle;
    else return (2 * pi - angle);
}

SDL_Point newPoint(int x, int y){
    SDL_Point t = {x,y};
    return t;
}

vector2D newVector(float x, float y){
    vector2D v = {x,y};
    return v;
}

//Check if two rectangles are colliding
/*bool _ObjectCollision(_object a, _object b){
    //if(distBetweenPoints(a.midPoint, b.midPoint) > (a.radius + b.radius)) return false;
    int delta = a.upperLeft.y - b.upperLeft.y;
    int omega = b.upperLeft.x - a.upperLeft.x;
    if (delta <= b.height && delta >= -a.height && omega >= -b.width && omega <= a.width) return true;
    return false;
}*/
