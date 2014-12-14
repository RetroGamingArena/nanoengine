//
//  World.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 27/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "World.h"
#include "WorkerItem.h"

World::World(int p, int q)
{
    this->p = p;
    this->q = q;
    
    push_back(new Chunk(p, q));
    Chunk* chunk = (*this)[this->size()-1];
    createChunk(chunk, p, q);
    genChunkBuffer(chunk);
    
}

Chunk* World::getChunk(int p, int q)
{
    return 0;
}

void World::createChunk(Chunk *chunk, int p, int q)
{
    initChunk(chunk, p, q);
    
    WorkerItem *item = new WorkerItem();
    item->p = chunk->p;
    item->q = chunk->q;
    item->block_map = &chunk->map;
    item->light_map = &chunk->lights;
    //item->loadChunk();
    /**/item->block_map->createWorld(item->p,item->q);
}

void World::initChunk(Chunk *chunk, int p, int q)
{
    chunk->p = p;
    chunk->q = q;
    chunk->faces = 0;
    chunk->sign_faces = 0;
    chunk->buffer = 0;
    chunk->sign_buffer = 0;
    dirtyChunk(chunk);
    int dx = p * CHUNK_SIZE - 1;
    int dy = 0;
    int dz = q * CHUNK_SIZE - 1;
    chunk->map = Map(dx, dy, dz, 34*34*32);
    chunk->lights = Map(dx, dy, dz, 0xf);
}

void World::dirtyChunk(Chunk *chunk)
{
    chunk->dirty = 1;
    if (hasLights(chunk)) {
        for (int dp = -1; dp <= 1; dp++) {
            for (int dq = -1; dq <= 1; dq++) {
                Chunk *other = findChunk(chunk->p + dp, chunk->q + dq);
                if (other) {
                    other->dirty = 1;
                }
            }
        }
    }
}

void World::genChunkBuffer(Chunk *chunk)
{
    WorkerItem _item;
    WorkerItem *item = &_item;
    item->p = chunk->p;
    item->q = chunk->q;
    int dp=0;
    int dq=0;
    Chunk *other = chunk;
    if (dp || dq)
    {
        other = findChunk(chunk->p + dp, chunk->q + dq);
    }
    if (other)
    {
        item->block_map = &other->map;
        item->light_map = &other->lights;
    }
    else
    {
        item->block_map = 0;
        item->light_map = 0;
    }
    chunk->compute();
    //item->computeChunk(this);
    chunk->generate();
    chunk->dirty = 0;
}

int World::hasLights(Chunk *chunk)
{
    if (!SHOW_LIGHTS) {
        return 0;
    }
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            Chunk *other = chunk;
            if (dp || dq) {
                other = findChunk(chunk->p + dp, chunk->q + dq);
            }
            if (!other) {
                continue;
            }
            Map *map = &other->lights;
            if (map->size) {
                return 1;
            }
        }
    }
    return 0;
}

Chunk *World::findChunk(int p, int q)
{
    for (int i = 0; i < size(); i++) {
        Chunk *chunk = (*this)[i];
        if (chunk->p == p && chunk->q == q) {
            return chunk;
        }
    }
    return 0;
}