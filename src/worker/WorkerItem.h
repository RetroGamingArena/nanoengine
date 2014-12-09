//
//  WorkerItem.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__WorkerItem__
#define __Nanocraft__WorkerItem__

#include <glew.h>

#include "map.h"
#include "Model.h"
#include "World.h"

class WorkerItem
{
    public:
        int p;
        int q;
        int load;
        Map *block_maps[3][3];
        Map *light_maps[3][3];
        int miny;
        int maxy;
        int faces;
        GLfloat *data;
    
        void loadChunk();
        void computeChunk(World* world);
        void generateChunk(Chunk* chunk);
};

#endif /* defined(__Nanocraft__WorkerItem__) */
