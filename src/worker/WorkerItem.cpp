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
    Map *block_map = block_maps[1][1];
    block_map->createWorld(p, q);
}

void WorkerItem::computeChunk(World *world)
{
    char *opaque = (char *)calloc(XZ_SIZE * XZ_SIZE * Y_SIZE, sizeof(float));
    char *light = (char *)calloc(XZ_SIZE * XZ_SIZE * Y_SIZE, sizeof(char));
    char *highest = (char *)calloc(XZ_SIZE * XZ_SIZE, sizeof(char));
    
    int ox = p * CHUNK_SIZE - CHUNK_SIZE - 1;
    int oy = -1;
    int oz = q * CHUNK_SIZE - CHUNK_SIZE - 1;
    
    // check for lights
    int has_light = 0;
    if (SHOW_LIGHTS) {
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                Map *map = light_maps[a][b];
                if (map && map->size) {
                    has_light = 1;
                }
            }
        }
    }
    
    // populate opaque array
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            Map *map = block_maps[a][b];
            if (!map) {
                continue;
            }
            MAP_FOR_EACH(map, ex, ey, ez, ew) {
                
                int rawx = Map::getX(i);
                int rawy = Map::getY(i);
                int rawz = Map::getZ(i);
                
                int x2 = rawx + map->dx - ox;
                int y2 = rawy + map->dy - oy;
                int z2 = rawz + map->dz - oz;
                
                int x = 0;
                int y = 0;
                int z = 0;
                
                if(entry->e.computed)
                {
                    x = x2;
                    y = y2;
                    z = z2;
                }

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
    }
    
    // flood fill light intensities
    if (has_light) {
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                Map *map = light_maps[a][b];
                if (!map) {
                    continue;
                }
                MAP_FOR_EACH(map, ex, ey, ez, ew)
                {
                    int rawx = Map::getX(i);
                    int rawy = Map::getY(i);
                    int rawz = Map::getZ(i);
                    
                    int x2 = rawx + map->dx - ox;
                    int y2 = rawy + map->dy - oy;
                    int z2 = rawz + map->dz - oz;
                    
                    int x = 0;
                    int y = 0;
                    int z = 0;
                    
                    if(entry->e.computed)
                    {
                        x = x2;
                        y = y2;
                        z = z2;
                    }
                    
                    //int x = //ex - ox;
                    //int y = //ey - oy;
                    //int z = //ez - oz;
                    
                    MathUtils::lightFill(opaque, light, x, y, z, ew, 1);
                } END_MAP_FOR_EACH;
            }
        }
    }
    
    Map *map = block_maps[1][1];
    
    // count exposed faces
    int miny = 256;
    int maxy = 0;
    int faces = 0;
    MAP_FOR_EACH(map, ex, ey, ez, ew) {
        if (ew <= 0) {
            continue;
        }
        
        int rawx = Map::getX(i);
        int rawy = Map::getY(i);
        int rawz = Map::getZ(i);
        
        int x2 = rawx + map->dx - ox;
        int y2 = rawy + map->dy - oy;
        int z2 = rawz + map->dz - oz;
        
        int x = 0;
        int y = 0;
        int z = 0;
        
        if(entry->e.computed)
        {
            x = x2;
            y = y2;
            z = z2;
        }
        
        int ey = y + map->dy;
        
        //int x = //ex - ox;
        //int y = //ey - oy;
        //int z = //ez - oz;
        
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
    GLfloat *data = malloc_faces(10, faces);
    int offset = 0;
    MAP_FOR_EACH_2(map, ex, ey, ez, esx, esy, esz, ew) {
        if (ew <= 0) {
            continue;
        }
        
        int rawx = Map::getX(i);
        int rawy = Map::getY(i);
        int rawz = Map::getZ(i);
        
        int x2 = rawx + map->dx - ox;
        int y2 = rawy + map->dy - oy;
        int z2 = rawz + map->dz - oz;
        
        int x = 0;
        int y = 0;
        int z = 0;
        
        if(entry->e.computed)
        {
            x = x2;
            y = y2;
            z = z2;
        }
        
        //int x = //ex - ox;
        //int y = //ey - oy;
        //int z = //ez - oz;
        
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
        if (total == 0) {
            continue;
        }
        char neighbors[27] = {0};
        char lights[27] = {0};
        float shades[27] = {0};
        int index = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dz = -1; dz <= 1; dz++) {
                    neighbors[index] = opaque[XYZ(x + dx, y + dy, z + dz)];
                    lights[index] = light[XYZ(x + dx, y + dy, z + dz)];
                    shades[index] = 0;
                    if (y + dy <= highest[XZ(x + dx, z + dz)]) {
                        for (int oy = 0; oy < 8; oy++) {
                            if (opaque[XYZ(x + dx, y + dy + oy, z + dz)]) {
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

            int mapX = Map::getX(i);
            int mapY = Map::getY(i);
            int mapZ = Map::getZ(i);
            
            int testX = mapX+p*CHUNK_SIZE-1;
            int testY = mapY;
            int testZ = mapZ+q*CHUNK_SIZE-1;

            /*ex = testX;
            ey = testY;
            ez = testZ;*/
        
            //printf("%i %i %i : %i\n", testX - ex, testY - ey, testZ - ez, i);
            
            make_cube(
                      data + offset, ao, light,
                      f1, f2, f3, f4, f5, f6,
                      testX, testY, testZ, 0, 0, 0, 0.5*CHUNK_RES, ew);
            /*make_cube(
                      data + offset, ao, light,
                      f1, f2, f3, f4, f5, f6,
                      ex, ey, ez, 0, 0, 0, 0.5*CHUNK_RES, ew);*/
        }
        offset += total * 60;
    } END_MAP_FOR_EACH;
    
    free(opaque);
    free(light);
    free(highest);
    
    this->miny = miny;
    this->maxy = maxy;
    this->faces = faces;
    this->data = data;
}

void WorkerItem::generateChunk(Chunk *chunk)
{
    chunk->miny = miny;
    chunk->maxy = maxy;
    chunk->faces = faces;
    
    del_buffer(chunk->buffer);
    chunk->buffer = gen_faces(10, faces, data);
    chunk->genSignBuffer();
}