//
//  Chunk.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cmath>
#include <string>

#include "Chunk.h"
#include "util.h"
#include "MathUtils.h"
#include "BufferUtils.h"

int Chunk::distance(/*Chunk *chunk,*/ int p, int q)
{
    int dp = ABS(this->p - p);
    int dq = ABS(this->q - q);
    return MAX(dp, dq);
}

int Chunk::chunked(float x)
{
    int base = 0;
    
    if(x>=0)
        base = floorf(roundf(x) / CHUNK_SIZE);
    else
        base = floorf(roundf(x*-1) / CHUNK_SIZE);
    
    if(x>=0)
        return base;
    else
        return (base + 1)*-1;
}

void Chunk::genSignBuffer()
{
    // first pass - count characters
    int max_faces = 0;
    for (int i = 0; i < signs.size; i++) {
        Sign *e = signs.data + i;
        max_faces += strlen(e->text);
    }
    
    // second pass - generate geometry
    GLfloat *data = malloc_faces(5, max_faces);
    int faces = 0;
    for (int i = 0; i < signs.size; i++) {
        Sign *e = signs.data + i;
        faces += BufferUtils::_genSignBuffer(data + faces * 30, e->x, e->y, e->z, e->face, e->text);
    }
    
    del_buffer(sign_buffer);
    sign_buffer = gen_faces(5, faces, data);
    sign_faces = faces;
}