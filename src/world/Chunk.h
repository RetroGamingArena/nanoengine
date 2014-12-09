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
#include "sign.h"

class Chunk
{
    public:
    Map map;
    Map lights;
    SignList signs;
    int p;
    int q;
    int faces;
    int sign_faces;
    int dirty;
    int miny;
    int maxy;
    GLuint buffer;
    GLuint sign_buffer;
    
    Chunk(int p, int q){this->p = p; this->q = q; signs.size = 0;};
    int distance(int p, int q);
    static int chunked(float x);
    void genSignBuffer();
};

#endif /* defined(__Nanocraft__Chunk__) */
