//
//  Player.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__Player__
#define __Nanocraft__Player__

#include <glew.h>

#include "State.h"
#include "map.h"

#define MAX_NAME_LENGTH 32

class Player
{
    public:
    int id;
    char name[MAX_NAME_LENGTH];
    State state;
    State state1;
    State state2;
    GLuint buffer;
    
    Player();
    float crosshairDistance(Player *p2);
    void update(float x, float y, float z, float rx, float ry, int interpolate);
    void interpolate();
    float distance(Player *p2);
    //int hitTestFace(Player *player, int *x, int *y, int *z, int *face);
    //int _hitTest(Map *map, float max_distance, int previous, float x, float y, float z, float vx, float vy, float vz, int *hx, int *hy, int *hz);
    //int hitTest(int previous, float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz);
};
#endif /* defined(__Nanocraft__Player__) */
