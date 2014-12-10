//
//  WorkerItem.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cstdlib>

#include "WorkerItem.h"
#include "MathUtils.h"
#include "cube.h"
#include "item.h"
#include "noise.h"
#include "util.h"

void WorkerItem::loadChunk()
{
    Map *block_map = this->block_map;
    block_map->createWorld(p, q);
}


/*void WorkerItem::generateChunk(Chunk *chunk)
{
    chunk->miny = miny;
    chunk->maxy = maxy;
    chunk->faces = faces;
    
    del_buffer(chunk->buffer);
    chunk->buffer = gen_faces(10, faces, data);
}*/