#include "SDL2/SDL.h"
#include <stdio.h>
//#include <alloc.h>
#include <malloc.h>

#include <math.h>


#include <iostream>

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 480

#define INDEX_FORWARD(Index) \
    Index = (Index + 1) % triVertexList->length;

#define INDEX_BACKWARD(Index) \
    Index = (Index - 1 + triVertexList->length) % triVertexList->length;

#define INDEX_MOVE(Index,Direction) \
    if (Direction > 0) \
        Index = (Index + 1) % 3; \
else \
    Index = (Index - 1 + 3) % 3;

using namespace std;

struct vert{
    float x,y;
};

struct vertList{
    int length;
    struct vert *vertPtr;
};

struct HLine{
    int startX;
    int endX;
};

struct HLineList{
    int length;
    HLine *HLinePtr;
};

int main(int argc, char** argv){

    vertList *triVertexList;
    triVertexList->vertPtr = (struct vert *) (malloc(sizeof(struct vert) * 3));

    triVertexList->vertPtr[0].x = 100.0; triVertexList->vertPtr[0].y = 200.0;
    triVertexList->vertPtr[1].x = 400.0; triVertexList->vertPtr[1].y = 300.0;
    triVertexList->vertPtr[2].x = 175.0; triVertexList->vertPtr[2].y = 500.0;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_WIDTH, 0);
    if(!window){
        printf("Error: Failed to open window \nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(!renderer){
        printf("Error: Failed to create renderer \nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    bool running = true;
    float movePoint = 0;
    int frameCount = 1;

    vert *VertexPtr = triVertexList->vertPtr;
    float MinPoint_Y, MaxPoint_Y;
    int leftEdgeDir, TopIsFlat, LeftEdgeDir,  MinIndexL, MaxIndex, MinIndexR, SkipFirst, temp;
    int i = 0;

    struct HLineList WorkingHLineList;
    struct HLine *EdgePointPtr;
    //struct Point *VertexPtr;


    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                running = false;
                    break;

                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 200, 200, 200);
        SDL_RenderDrawLineF(renderer, movePoint+triVertexList->vertPtr[0].x, movePoint+triVertexList->vertPtr[0].y,
                                      movePoint+triVertexList->vertPtr[1].x, movePoint+triVertexList->vertPtr[1].y);
        SDL_RenderDrawLineF(renderer, movePoint+triVertexList->vertPtr[1].x, movePoint+triVertexList->vertPtr[1].y, 
                                      movePoint+triVertexList->vertPtr[2].x, movePoint+triVertexList->vertPtr[2].y);
        SDL_RenderDrawLineF(renderer, movePoint+triVertexList->vertPtr[2].x, movePoint+triVertexList->vertPtr[2].y, 
                                      movePoint+triVertexList->vertPtr[0].x, movePoint+triVertexList->vertPtr[0].y);


       // polygon horizontal fill algorithm
        //pg 528
        MinIndexL = MaxIndex = 0;// scan list to find the top vertex
        MaxPoint_Y = MinPoint_Y = VertexPtr[0].y;
        for (i = 1; i < 3; i++) {
        if (VertexPtr[i].y < MinPoint_Y){
            MinIndexL = i;
            MinPoint_Y = VertexPtr[i].y; /* new top */
        } else if (VertexPtr[i].y > MaxPoint_Y){
             MaxIndex = i;
             MaxPoint_Y = VertexPtr[i].y; /* new bottom */
            }
        }

        if (MinPoint_Y == MaxPoint_Y)
            return(1); /* polygon is 0-height; avoid infinite loop below */

        /* Scan in ascending order to find the last top-edge point */
        MinIndexR = MinIndexL;
        while (VertexPtr[MinIndexR].y == MinPoint_Y)
            INDEX_FORWARD(MinIndexR);
        INDEX_BACKWARD(MinIndexR); /* back up to last top-edge point */
        /* Now scan in descending order to find the first top-edge point */
        while (VertexPtr[MinIndexL].y == MinPoint_Y)
            INDEX_BACKWARD(MinIndexL);
        INDEX_FORWARD(MinIndexL); 

        // find wich edge from top vertex is left edge, right edge
        LeftEdgeDir = -1;
        if ((TopIsFlat = (VertexPtr[MinIndexL].x != VertexPtr[MinIndexR].x) ? 1 : 0) == 1){
            if (VertexPtr[MinIndexL].x > VertexPtr[MinIndexR].x){
                LeftEdgeDir = 1;
                temp = MinIndexL;
                MinIndexL = MinIndexR;
                MinIndexR = temp;
            }
        } else{
            

        }

        if (frameCount == 50){
            frameCount = 0;
            movePoint++;
            //cout<<"max point: "<<MaxPoint_Y;
        }
        frameCount++;

       SDL_RenderPresent(renderer);
       SDL_RenderPresent(renderer);
    }

    return 0;
}