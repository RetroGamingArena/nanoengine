//
//  Chunk.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cmath>
#include <string>
#include <cstdlib>
#include <iostream>

#include "Chunk.h"
#include "util.h"
#include "MathUtils.h"
#include "BufferUtils.h"
#include "item.h"
#include "noise.h"
#include "cube.h"

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

void Chunk::empty()
{
    delete map;
    delete lights;
}

void Chunk::generate()
{
    del_buffer(buffer);
    buffer = gen_faces(10, faces, data);
}

using namespace std;

void Chunk::compute()
{
    clock_t begin = clock();
    int count = XZ_SIZE * XZ_SIZE * Y_SIZE*4;
    char* opaque = new char[count];
    char light[XZ_SIZE * XZ_SIZE * Y_SIZE];
    char highest[XZ_SIZE * XZ_SIZE];
    
    int ox = p * CHUNK_SIZE - CHUNK_SIZE - 1;
    int oy = -1;
    int oz = q * CHUNK_SIZE - CHUNK_SIZE - 1;
    
    // check for lights
    int has_light = 0;
    /*if (SHOW_LIGHTS)
    {
        Map *map = lights;
        if (map && map->size)
            has_light = 1;
    }*/
    
    GLfloat *data = malloc_faces(10, faces);
    
    // populate opaque array
    if (&map)
    {
        MAP_FOR_EACH((map), ex, ey, ez, ew)
        {
            int rawx = Map::getX(i);
            int rawy = Map::getY(i);
            int rawz = Map::getZ(i);
            
            int x = rawx + map->dx - ox;
            int y = rawy + map->dy - oy;
            int z = rawz + map->dz - oz;
            
            int w = ew;
            // TODO: this should be unnecessary
            if (x < 0 || y < 0 || z < 0) {
                continue;
            }
            if (x >= XZ_SIZE || y >= Y_SIZE || z >= XZ_SIZE) {
                continue;
            }
            // END TODO
            opaque[XYZ(x, y, z)] = !is_transparent(w);
            if (opaque[XYZ(x, y, z)]) {
                highest[XZ(x, z)] = MAX(highest[XZ(x, z)], y);
            }
        } END_MAP_FOR_EACH;
    }
    //}
    
    // flood fill light intensities
    if (has_light)
    {
        Map *map = lights;
        if (map)
        {
            MAP_FOR_EACH(map, ex, ey, ez, ew)
            {
                int rawx = Map::getX(i);
                int rawy = Map::getY(i);
                int rawz = Map::getZ(i);
                
                int x = rawx + map->dx - ox;
                int y = rawy + map->dy - oy;
                int z = rawz + map->dz - oz;
                
                MathUtils::lightFill(opaque, light, x, y, z, ew, 1);
            } END_MAP_FOR_EACH;
        }
        
    }
    
    // count exposed faces
    int miny = 256;
    int maxy = 0;
    int faces = 0;
    MAP_FOR_EACH((map), ex, ey, ez, ew) {
        if (ew <= 0) {
            continue;
        }
        
        int rawx = Map::getX(i);
        int rawy = Map::getY(i);
        int rawz = Map::getZ(i);
        
        int x = rawx + map->dx - ox;
        int y = rawy + map->dy - oy;
        int z = rawz + map->dz - oz;
        
        int ey = y + map->dy;
        
        int f1 = !opaque[XYZ(x - 1, y, z)];
        int f2 = !opaque[XYZ(x + 1, y, z)];
        int f3 = !opaque[XYZ(x, y + 1, z)];
        int f4 = !opaque[XYZ(x, y - 1, z)] && (ey > 0);
        int f5 = !opaque[XYZ(x, y, z - 1)];
        int f6 = !opaque[XYZ(x, y, z + 1)];
        int total = f1 + f2 + f3 + f4 + f5 + f6;
        if (total == 0) {
            continue;
        }
        if (is_plant(ew)) {
            total = 4;
        }
        miny = MIN(miny, ey);
        maxy = MAX(maxy, ey);
        faces += total;
    } END_MAP_FOR_EACH;
    
    // generate geometry
    //GLfloat *data = malloc_faces(10, faces);
    int offset = 0;
    MAP_FOR_EACH_2(map, ex, ey, ez, esx, esy, esz, ew) {
        if (ew <= 0) {
            continue;
        }
        
        int rawx = Map::getX(i);
        int rawy = Map::getY(i);
        int rawz = Map::getZ(i);
        
        int x = rawx + map->dx - ox;
        int y = rawy + map->dy - oy;
        int z = rawz + map->dz - oz;
        
        int ex = x + map->dx;
        int ey = y + map->dy;
        int ez = z + map->dz;
        
        int f1 = !opaque[XYZ(x - 1, y, z)];
        int f2 = !opaque[XYZ(x + 1, y, z)];
        int f3 = !opaque[XYZ(x, y + 1, z)];
        int f4 = !opaque[XYZ(x, y - 1, z)] && (ey > 0);
        int f5 = !opaque[XYZ(x, y, z - 1)];
        int f6 = !opaque[XYZ(x, y, z + 1)];
        int total = f1 + f2 + f3 + f4 + f5 + f6;
        if (total == 0)
        {
            continue;
        }
        char neighbors[27] = {0};
        char lights[27] = {0};
        float shades[27] = {0};
        int index = 0;
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dz = -1; dz <= 1; dz++)
                {
                    neighbors[index] = opaque[XYZ(x + dx, y + dy, z + dz)];
                    lights[index] = light[XYZ(x + dx, y + dy, z + dz)];
                    shades[index] = 0;
                    if (y + dy <= highest[XZ(x + dx, z + dz)])
                    {
                        for (int oy = 0; oy < 8; oy++)
                        {
                            if (opaque[XYZ(x + dx, y + dy + oy, z + dz)])
                            {
                                shades[index] = 1.0 - oy * 0.125;
                                break;
                            }
                        }
                    }
                    index++;
                }
            }
        }
        float ao[6][4];
        float light[6][4];
        MathUtils::occlusion(neighbors, lights, shades, ao, light);
        if (is_plant(ew)) {
            total = 4;
            float min_ao = 1;
            float max_light = 0;
            for (int a = 0; a < 6; a++) {
                for (int b = 0; b < 4; b++) {
                    min_ao = MIN(min_ao, ao[a][b]);
                    max_light = MAX(max_light, light[a][b]);
                }
            }
            float rotation = simplex2(ex, ez, 4, 0.5, 2) * 360;
            make_plant(
                       data + offset, min_ao, max_light,
                       ex, ey, ez, 0.5, ew, rotation);
        }
        else {
            
            int mapX = Map::getX(i)+p*CHUNK_SIZE-1;
            int mapY = Map::getY(i);
            int mapZ = Map::getZ(i)+q*CHUNK_SIZE-1;
            
            make_cube(data + offset, ao, light, f1, f2, f3, f4, f5, f6, mapX, mapY, mapZ, 0, 0, 0, CHUNK_RES, ew);
        }
        offset += total * 60;
    } END_MAP_FOR_EACH;
    
    delete[] opaque;
    //delete[] light;
    //delete[] highest;
    
    this->miny = miny;
    this->maxy = maxy;
    this->faces = faces;
    this->data = data;
    
    clock_t end = clock();
    cout << "compute: " << (end - begin) / (CLOCKS_PER_SEC/1000) << endl;
}
