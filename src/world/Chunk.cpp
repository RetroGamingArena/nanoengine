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

int Chunk::distance(int p, int q)
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