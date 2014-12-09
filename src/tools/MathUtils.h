//
//  MathUtils.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__MathUtils__
#define __Nanocraft__MathUtils__

#include <cmath>
#include "config.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PI 3.14159265359
#define DEGREES(radians) ((radians) * 180 / PI)
#define RADIANS(degrees) ((degrees) * PI / 180)
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define SIGN(x) (((x) > 0) - ((x) < 0))

#define XZ_SIZE (CHUNK_SIZE * 3 + 2)
#define XZ_LO (CHUNK_SIZE)
#define XZ_HI (CHUNK_SIZE * 2 + 1)
#define Y_SIZE 258
#define XYZ(x, y, z) ((y) * XZ_SIZE * XZ_SIZE + (x) * XZ_SIZE + (z))
#define XZ(x, z) ((x) * XZ_SIZE + (z))

class MathUtils
{
    public:
        static void getSightVector(float rx, float ry, float *vx, float *vy, float *vz);
        static void getMotionVector(int flying, int sz, int sx, float rx, float ry,float *vx, float *vy, float *vz);
        static int playerIntersectsBlock(int height, float x, float y, float z, int hx, int hy, int hz);
        static void occlusion(char neighbors[27], char lights[27], float shades[27], float ao[6][4], float light[6][4]);
        static void lightFill(char *opaque, char *light, int x, int y, int z, int w, int force);
};

#endif /* defined(__Nanocraft__MathUtils__) */
