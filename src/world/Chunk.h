//
//  Chunk.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__Chunk__
#define __Nanocraft__Chunk__

#include "glew.h"

#include "map.h"

class Chunk
{
    public:
    Map* map;
    Map* lights;
    int p;
    int q;
    int faces;
    int sign_faces;
    int dirty;
    int miny;
    int maxy;
    bool load;
    bool busy;
    GLuint buffer;
    GLuint sign_buffer;
    GLfloat *data;
    
    Chunk(int p, int q)
    {
        this->p = p;
        this->q = q;
        this->faces = 0;
        this->sign_faces = 0;
        this->buffer = 0;
        this->sign_buffer = 0;
        
        int dx = p * CHUNK_SIZE - 1;
        int dy = 0;
        int dz = q * CHUNK_SIZE - 1;
        
        this->map = new Map(dx, dy, dz, 34*34*32);
        this->lights = new Map(dx, dy, dz, 0xf);
        
        this->map->createWorld(this->p,this->q);
        busy = false;
    };
    int distance(int p, int q);
    static int chunked(float x);
    void generate();
    void compute();
    void empty();
};

#endif /* defined(__Nanocraft__Chunk__) */
