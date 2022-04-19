#include "headers/include/SDL2/SDL.h"
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
        Index = (Index + 1) % 4; \
else \
    Index = (Index - 1 + 4) % 4;

using namespace std;

struct vert{
    float x,y;
};

struct vertList{
    int length;
    struct vert *vertPtr;
};

struct HLine{
    float XStart;
    float XEnd;
};

struct HLineList{
    int length;
    float yStart;
    struct HLine *HLinePtr;
};


void ScanEdge(float X1, float Y1, float X2, float Y2, float SetXStart, int SkipFirst, struct HLine **EdgePointPtr);

int main(int argc, char** argv){

    vertList *triVertexList = (vertList *) malloc(sizeof(struct vertList));
    triVertexList->vertPtr = (struct vert *) malloc(sizeof(struct vert) * 5);

    triVertexList->vertPtr[0].x = 100.0; triVertexList->vertPtr[0].y = 200.0;
    triVertexList->vertPtr[1].x = 400.0; triVertexList->vertPtr[1].y = 200.0;
    triVertexList->vertPtr[2].x = 495.0; triVertexList->vertPtr[2].y = 500.0;
    triVertexList->vertPtr[3].x = 50.0;  triVertexList->vertPtr[3].y = 350.0;
    
    triVertexList->length = 5;

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

    vert *VertexPtr = (struct vert *) (malloc(sizeof(triVertexList->vertPtr)));
    VertexPtr = triVertexList->vertPtr;

    float MinPoint_Y, MaxPoint_Y;
    int leftEdgeDir , TopIsFlat , LeftEdgeDir , MinIndexL  , MaxIndex , MinIndexR , SkipFirst , temp = 0;  
    int NextIndex, CurrentIndex, PreviousIndex;
    int DeltaXN, DeltaYN, DeltaXP, DeltaYP;

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

        SDL_SetRenderDrawColor(renderer, 0, 225, 225, 225);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 100, 100, 100);
        SDL_RenderDrawLineF(renderer, movePoint+triVertexList->vertPtr[0].x, movePoint+triVertexList->vertPtr[0].y,
                                      movePoint+triVertexList->vertPtr[1].x, movePoint+triVertexList->vertPtr[1].y);
        SDL_RenderDrawLineF(renderer, movePoint+triVertexList->vertPtr[1].x, movePoint+triVertexList->vertPtr[1].y, 
                                      movePoint+triVertexList->vertPtr[2].x, movePoint+triVertexList->vertPtr[2].y);
        SDL_RenderDrawLineF(renderer, movePoint+triVertexList->vertPtr[2].x, movePoint+triVertexList->vertPtr[2].y, 
                                      movePoint+triVertexList->vertPtr[3].x, movePoint+triVertexList->vertPtr[3].y);
        SDL_RenderDrawLineF(renderer, movePoint+triVertexList->vertPtr[3].x, movePoint+triVertexList->vertPtr[3].y, 
                                      movePoint+triVertexList->vertPtr[0].x, movePoint+triVertexList->vertPtr[0].y);

       // polygon horizontal fill algorithm
        //pg 528
        // scan list to find the top vertex
        int MinIndexR = 0;
        int MaxIndex = 0;
        int MinIndexL = 0;
        MaxPoint_Y = VertexPtr[0].y;
        MinPoint_Y = VertexPtr[0].y;

        for (int i = 1; i < 4; i++) {
        if (VertexPtr[i].y < MinPoint_Y){
            MinIndexL = i;
            MinPoint_Y = VertexPtr[i].y; /* new top */
        } else if (VertexPtr[i].y > MaxPoint_Y){
             MaxIndex = i;
             MaxPoint_Y = VertexPtr[i].y; /* new bottom */
            }
        }

        // GDB stuff
        // set follow-fork-mode
        // set detach-on-fork off
        MinIndexR = MinIndexL;

        if (MinPoint_Y == MaxPoint_Y)
            return(1); /* polygon is 0-height; avoid infinite loop below */
        
        /* Scan in ascending order to find the last top-edge point */
       
       while (VertexPtr[MinIndexR].y == MinPoint_Y)
           INDEX_FORWARD(MinIndexR);
       INDEX_BACKWARD(MinIndexR); /* back up to last top-edge point */
        /* Now scan in descending order to find the first top-edge point */
        while (VertexPtr[MinIndexL].y == MinPoint_Y)
            INDEX_BACKWARD(MinIndexL);
        INDEX_FORWARD(MinIndexL);

        struct HLineList WorkingHLineList;
        struct HLine *EdgePointPtr;

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
            NextIndex = MinIndexR;
            INDEX_FORWARD(NextIndex);
            PreviousIndex = MinIndexL;
            INDEX_BACKWARD(PreviousIndex);
            /* Calculate X and Y lengths from the top vertex to the end of
            the first line down each edge; use those to compare slopes
            and see which line is leftmost */
            DeltaXN = VertexPtr[NextIndex].x - VertexPtr[MinIndexL].x;
            DeltaYN = VertexPtr[NextIndex].y - VertexPtr[MinIndexL].y;
            DeltaXP = VertexPtr[PreviousIndex].x - VertexPtr[MinIndexL].x;
            DeltaYP = VertexPtr[PreviousIndex].y - VertexPtr[MinIndexL].y;
            if (((long)DeltaXN * DeltaYP - (long)DeltaYN * DeltaXP) < 0L) {
            LeftEdgeDir = 1; /* left edge runs up through vertex list */
            temp = MinIndexL; /* swap the indices so MinIndexL */
            MinIndexL = MinIndexR; /* points to the start of the left */
            MinIndexR = temp; /* edge, similarly for MinIndexR */
            }
        }
        /* Set the # of scan lines in the polygon, skipping the bottom edge
            and also skipping the top vertex if the top isn't flat because
            in that case the top vertex has a right edge component, and set
            the top scan line to draw, which is likewise the second line of
            the polygon unless the top is flat */
        if ((WorkingHLineList.length = MaxPoint_Y - MinPoint_Y - 1 + TopIsFlat) <= 0)
            return(1); /* there's nothing to draw, so we're done */

        WorkingHLineList.yStart = 0 + MinPoint_Y + 1 - TopIsFlat;
        /* Get memory in which to store the line list we generate */
        if ((WorkingHLineList.HLinePtr = (struct HLine *) (malloc(sizeof(struct HLine) * WorkingHLineList.length))) == NULL)
            return(0); /* couldn't get memory for the line list */

        /* Scan the left edge and store the boundary points in the list */
        /* Initial pointer for storing scan converted left-edge coords */
        EdgePointPtr = WorkingHLineList.HLinePtr;
        /* Start from the top of the left edge */
        PreviousIndex = CurrentIndex = MinIndexL;
        /* Skip the first point of the first line unless the top is flat;
        if the top isn't flat, the top vertex is exactly on a right
        edge and isn't drawn */
        SkipFirst = TopIsFlat ? 0 : 1;
        /* Scan convert each line in the left edge from top to bottom */
        do {
            INDEX_MOVE(CurrentIndex,LeftEdgeDir);
            ScanEdge(VertexPtr[PreviousIndex].x + 0,
            VertexPtr[PreviousIndex].y,
            VertexPtr[CurrentIndex].x + 0,
            VertexPtr[CurrentIndex].y, 1, SkipFirst, &EdgePointPtr);
            PreviousIndex = CurrentIndex;
            SkipFirst = 0; /* scan convert the first point from now on */
        } while (CurrentIndex != MaxIndex);


        /* Scan the right edge and store the boundary points in the list */
        EdgePointPtr = WorkingHLineList.HLinePtr;
        PreviousIndex = CurrentIndex = MinIndexR;
        SkipFirst = TopIsFlat ? 0 : 1;
        /* Scan convert the right edge, top to bottom. X coordinates are
        adjusted 1 to the left, effectively causing scan conversion of
        the nearest points to the left of but not exactly on the edge */
        do {
            INDEX_MOVE(CurrentIndex,-LeftEdgeDir);
            ScanEdge(VertexPtr[PreviousIndex].x + 0 - 1,
            VertexPtr[PreviousIndex].y,
            VertexPtr[CurrentIndex].x + 0 - 1,
            VertexPtr[CurrentIndex].y, 0.0, SkipFirst, &EdgePointPtr);
            PreviousIndex = CurrentIndex;
            SkipFirst = 0; /* scan convert the first point from now on */
        } while (CurrentIndex != MaxIndex);
        /* Draw the line list representing the scan converted polygon */

        for(int i = 0; i < WorkingHLineList.length; i++){
            SDL_RenderDrawLineF(renderer, WorkingHLineList.HLinePtr[i].XStart, WorkingHLineList.yStart+(float)i, 
                                         WorkingHLineList.HLinePtr[i].XEnd, WorkingHLineList.yStart+(float)i);
        }
        /* Release the line list's memory and we're successfully done */
        free(WorkingHLineList.HLinePtr);

        if (frameCount == 50){
            frameCount = 0;
            movePoint++;
        }
        frameCount++;

       SDL_RenderPresent(renderer);
       SDL_RenderPresent(renderer);
    }

    return 0;
}

void ScanEdge(float X1, float Y1, float X2, float Y2, float SetXStart, int SkipFirst, struct HLine **EdgePointPtr) {

    int Y, DeltaX, Height, Width, AdvanceAmt, ErrorTerm, i;
    int ErrorTermAdvance, XMajorAdvanceAmt;
    struct HLine *WorkingEdgePointPtr;

    WorkingEdgePointPtr = *EdgePointPtr; /* avoid double dereference */
    AdvanceAmt = ((DeltaX = X2 - X1) > 0) ? 1 : -1;

    /* direction in which X moves (Y2 is
    always > Y1, so Y always counts up) */
    if ((Height = Y2 - Y1) <= 0) /* Y length of the edge */
        return; /* guard against 0-length and horizontal edges */
    /* Figure out whether the edge is vertical, diagonal, X-major
    (mostly horizontal), or Y-major (mostly vertical) and handle
    appropriately */
    
    if ((Width = abs(DeltaX)) == 0) {
    /* The edge is vertical; special-case by just storing the same
    X coordinate for every scan line */
    /* Scan the edge for each scan line in turn */
    for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
    /* Store the X coordinate in the appropriate edge list */
        if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1;
        else
            WorkingEdgePointPtr->XEnd = X1;
    }

    } else if (Width == Height) {
    /* The edge is diagonal; special-case by advancing the X
    coordinate 1 pixel for each scan line */
        if (SkipFirst) /* skip the first point if so indicated */
            X1 += AdvanceAmt; /* move 1 pixel to the left or right */
    /* Scan the edge for each scan line in turn */
    for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
    /* Store the X coordinate in the appropriate edge list */
        if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1;
        else
            WorkingEdgePointPtr->XEnd = X1;
            X1 += AdvanceAmt; /* move 1 pixel to the left or right */
        }
    } else if (Height > Width) {
    /* Edge is closer to vertical than horizontal (Y-major) */
        if (DeltaX >= 0)
            ErrorTerm = 0; /* initial error term going left->right */
        else
            ErrorTerm = -Height + 1; /* going right->left */
    if (SkipFirst) { /* skip the first point if so indicated */
    /* Determine whether it's time for the X coord to advance */
        if ((ErrorTerm += Width) > 0) {
            X1 += AdvanceAmt; /* move 1 pixel to the left or right */
            ErrorTerm -= Height; /* advance ErrorTerm to next point */
        }
    }
    /* Scan the edge for each scan line in turn */
    for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
    /* Store the X coordinate in the appropriate edge list */
        if (SetXStart == 1)
            WorkingEdgePointPtr->XStart = X1;
        else
            WorkingEdgePointPtr->XEnd = X1;
    /* Determine whether it's time for the X coord to advance */
        if ((ErrorTerm += Width) > 0) {
            X1 += AdvanceAmt; /* move 1 pixel to the left or right */
            ErrorTerm -= Height; /* advance ErrorTerm to correspond */
        }
    }
    } else {
    /* Edge is closer to horizontal than vertical (X-major) */
    /* Minimum distance to advance X each time */
        XMajorAdvanceAmt = (Width / Height) * AdvanceAmt;
    /* Error term advance for deciding when to advance X 1 extra */
        ErrorTermAdvance = Width % Height;
        if (DeltaX >= 0)
            ErrorTerm = 0; /* initial error term going left->right */
        else
            ErrorTerm = -Height + 1; /* going right->left */
        if (SkipFirst) { /* skip the first point if so indicated */
            X1 += XMajorAdvanceAmt; /* move X minimum distance */
    /* Determine whether it's time for X to advance one extra */
        if ((ErrorTerm += ErrorTermAdvance) > 0) {
            X1 += AdvanceAmt; /* move X one more */
            ErrorTerm -= Height; /* advance ErrorTerm to correspond */
        }
    }
    /* Scan the edge for each scan line in turn */
        for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++) {
    /* Store the X coordinate in the appropriate edge list */
            if (SetXStart == 1)
                WorkingEdgePointPtr->XStart = X1;
            else
                WorkingEdgePointPtr->XEnd = X1;
                X1 += XMajorAdvanceAmt; /* move X minimum distance */
                /* Determine whether it's time for X to advance one extra */
                if ((ErrorTerm += ErrorTermAdvance) > 0) {
                X1 += AdvanceAmt; /* move X one more */
                ErrorTerm -= Height; /* advance ErrorTerm to correspond */
                }
        }
    }
    *EdgePointPtr = WorkingEdgePointPtr;
}