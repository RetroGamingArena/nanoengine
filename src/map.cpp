#include <stdlib.h>
#include <string.h>
#include <cmath>
#include "map.h"
#include "util.h"
#include "noise.h"

Map::Map(int dx, int dy, int dz, int mask)
{
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
    this->mask = mask;
    this->size = 0;
    this->data = new nano_row[(this->mask + 1) / (8/ITEM_RANGE)];
    for(int i = 0 ; i < ((this->mask + 1) / (8/ITEM_RANGE)); i++)
        this->data[i] = 0;
}

int Map::_hitTest(float max_distance, int previous, float x, float y, float z, float vx, float vy, float vz, int *hx, int *hy, int *hz)
{
    int m = 32;
    int px = 0;
    int py = 0;
    int pz = 0;
    for (int i = 0; i < max_distance * m; i++) {
        int nx = roundf(x);
        int ny = roundf(y);
        int nz = roundf(z);
        if (nx != px || ny != py || nz != pz) {
            int hw = this->get(nx, ny, nz);
            if (hw > 0) {
                if (previous) {
                    *hx = px; *hy = py; *hz = pz;
                }
                else {
                    *hx = nx; *hy = ny; *hz = nz;
                }
                return hw;
            }
            px = nx; py = ny; pz = nz;
        }
        x += vx / m; y += vy / m; z += vz / m;
    }
    return 0;
}

int Map::set(int x, int y, int z, double dx, double dy, double dz, int w, bool enabled)
{
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    
    unsigned int index = getIndex(x,y%32,z);
    
    if (w > 0)
    {
        setData(index,w);
        this->size++;
        return 1;
    }
    return 0;
}

int Map::get(int x, int y, int z)
{
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    
    unsigned int index = getIndex(x,y%32,z);
    nano entry = this->getData(index);

    return entry;
}

void Map::createWorld(int p, int q)
{
    int pad = 1;
    for (double dx = -pad; dx < CHUNK_SIZE + pad; dx=dx+CHUNK_RES) {
        for (double dz = -pad; dz < CHUNK_SIZE + pad; dz=dz+CHUNK_RES) {
            int flag = 1;
            if (dx < 0 || dz < 0 || dx >= CHUNK_SIZE || dz >= CHUNK_SIZE) {
                flag = -1;
            }
            double x = p * CHUNK_SIZE + dx;
            double z = q * CHUNK_SIZE + dz;
            double dy = 0;
            float f = simplex2(x * 0.01, z * 0.01, 4, 0.5, 2);
            float g = simplex2(-x * 0.01, -z * 0.01, 2, 0.9, 2);
            int mh = g * 32 + 16;
            int h = f * mh;
            int w = 1;
            int t = 12;
            if (h <= t) {
                h = t;
                w = 2;
            }
  
            // sand and grass terrain
            for (float y = 0; y < h; y=y+CHUNK_RES)
            {
                //coord absolues
                set(x, (int)y, z, fmod(dx,1), fmod(y,1), fmod(dz,1), w * flag, true);
                //func(x, (int)y, z, fmod(dx,1), fmod(y,1), fmod(dz,1), w * flag, this);
            }
            for (float y = h; y < CHUNK_SIZE; y=y+CHUNK_RES) // TODO : fait buguer les arbres
            {
                set(x, (int)y, z, fmod(dx,1), fmod(y,1), fmod(dz,1), 0, false);
                //func(x, (int)y, z, fmod(dx,1), fmod(y,1), fmod(dz,1), w * flag, this);
            }
            if (w == 1) {
                if (SHOW_PLANTS) {
                    // grass
                    if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
                        set(x, h, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), 17 * flag, true);
                        //func(x, h, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), 17 * flag, this);
                    }
                    // flowers
                    if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
                        int w = 18 + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2) * 7;
                         set(x, h, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), w * flag,true );
                        //func(x, h, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), w * flag, this);
                    }
                }
                // trees
                int ok = SHOW_TREES;
                if (dx - 4 < 0 || dz - 4 < 0 ||
                    dx + 4 >= CHUNK_SIZE || dz + 4 >= CHUNK_SIZE)
                {
                    ok = 0;
                }
                if (ok && simplex2(x, z, 6, 0.5, 2) > 0.84) {
                    for (int y = h + 3; y < h + 8; y++) {
                        for (int ox = -3; ox <= 3; ox++) {
                            for (int oz = -3; oz <= 3; oz++) {
                                int d = (ox * ox) + (oz * oz) +
                                (y - (h + 4)) * (y - (h + 4));
                                if (d < 11) {
                                    set(x + ox, y, z + oz, fmod(dx,1), fmod(dy,1), fmod(dz,1), 15,true);
                                    //func(x + ox, y, z + oz, fmod(dx,1), fmod(dy,1), fmod(dz,1), 15, this);
                                }
                            }
                        }
                    }
                    for (int y = h; y < h + 7; y++) {
                        set(x, y, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), 5,true);
                        //func(x, y, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), 5, this);
                    }
                }
            }
            // clouds
            if (SHOW_CLOUDS) {
                for (int y = 64; y < 72; y++) {
                    if (simplex3(x * 0.01, y * 0.1, z * 0.01, 8, 0.5, 2) > 0.75)
                    {
                        set(x, y, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), 16 * flag, true);
                        //func(x, y, z, fmod(dx,1), fmod(dy,1), fmod(dz,1), 16 * flag, this);
                    }
                }
            }
        }
    }
}

int Map::getX(int index)
{
    return (index % (34*34)) % 34;
}
           
int Map::getY(int index)
{
    return index / (34*34);
}
           
int Map::getZ(int index)
{
    return (index % (34*34)) / 34;
}
