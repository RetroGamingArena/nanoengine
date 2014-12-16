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
    Map map;
    Map lights;
    int p;
    int q;
    int faces;
    int sign_faces;
    int dirty;
    int miny;
    int maxy;
    bool load;
    GLuint buffer;
    GLuint sign_buffer;
    GLfloat *data;
    
    Chunk(int p, int q){this->p = p; this->q = q;};
    int distance(int p, int q);
    static int chunked(float x);
    void generate(/*int minx, int miny, int faces*/);
    void compute();
};

#endif /* defined(__Nanocraft__Chunk__) */
