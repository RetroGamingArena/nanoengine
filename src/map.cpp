#include <stdlib.h>
#include <string.h>
#include <cmath>
#include "map.h"
#include "util.h"
#include "noise.h"

int hash_int(int key) {
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
    return key;
}

int hash(int x, int y, int z) {
    x = hash_int(x);
    y = hash_int(y);
    z = hash_int(z);
    return x ^ y ^ z;
}

Map::Map(int dx, int dy, int dz, int mask)
{
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
    this->mask = mask;
    this->size = 0;
    this->data = (MapEntry *)calloc(this->mask + 1, sizeof(MapEntry));
}

/*void Map::map_alloc(Map *map, int dx, int dy, int dz, int mask)
{
    map->dx = dx;
    map->dy = dy;
    map->dz = dz;
    map->mask = mask;
    map->size = 0;
    map->data = (MapEntry *)calloc(map->mask + 1, sizeof(MapEntry));
}*/

void Map::map_free(Map *map) {
    free(map->data);
}

void Map::map_copy(Map *dst, Map *src) {
    dst->dx = src->dx;
    dst->dy = src->dy;
    dst->dz = src->dz;
    dst->mask = src->mask;
    dst->size = src->size;
    dst->data = (MapEntry *)calloc(dst->mask + 1, sizeof(MapEntry));
    memcpy(dst->data, src->data, (dst->mask + 1) * sizeof(MapEntry));
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
            int hw = map_get(this, nx, ny, nz);
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
    //unsigned int index = hash(x, y, z) & this->mask;
    
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    
    unsigned int index = getIndex(x,y%32,z);
    
    MapEntry *entry = this->data + index;
   
    /*while (!EMPTY_ENTRY(entry)) {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z)
            break;
        index = (index + 1) & this->mask;
        entry = this->data + index;
    }*/

    if(x==0  && index==1)
        int a = 2;
    
    /*entry->e.x = x;
    entry->e.y = y%32;
    entry->e.z = z;*/
    
    entry->e.computed = true;

    if (w)
    {

        /*while (dx<0)
            dx++;
        while (dy<0)
            dy++;
        while (dz<0)
            dz++;*/
        
        /*entry->e.sx = dx/CHUNK_RES;
        entry->e.sy = dy/CHUNK_RES;
        entry->e.sz = dz/CHUNK_RES;*/
        
        entry->e.w = w;
        this->size++;
        if (this->size * 2 > this->mask)
        {
            //map_grow(this);
        }
        return 1;
    }
    return 0;
}

int Map::map_get(Map *map, int x, int y, int z) {
    unsigned int index = hash(x, y, z) & map->mask;
    x -= map->dx;//-1;
    y -= map->dy;
    z -= map->dz;//-1;
    
    if (x < 0 || x > 255) return 0;
    if (y < 0 || y > 255) return 0;
    if (z < 0 || z > 255) return 0;
    
    index = getIndex(x,y%32,z);
    
    MapEntry *entry = map->data + index;
    /*while (!EMPTY_ENTRY(entry))
    {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z)
        {
            return entry->e.w;
        }
        index = (index + 1) & map->mask;
        entry = map->data + index;
    }
    return 0;*/
    return entry->e.w;
}

void Map::map_grow(Map *map) {
    /*Map new_map(map->dx, map->dy, map->dz, (map->mask << 1) | 1);
    
    new_map.size = 0;
    new_map.data = (MapEntry *)calloc(new_map.mask + 1, sizeof(MapEntry));
    MAP_FOR_EACH(map, ex, ey, ez, ew) {
        (&new_map)->set(ex, ey, ez, 0, 0, 0, ew,true);
    } END_MAP_FOR_EACH;
    free(map->data);
    map->mask = new_map.mask;
    map->size = new_map.size;
    map->data = new_map.data;*/
}

/*void Map::mapSetFunc(int x, int y, int z, double dx, double dy, double dz, int w, void *arg)
{
    Map *map = (Map *)arg;
    Map::map_set(map, x, y, z, dx, dy, dz, w);
}*/

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
            
            if(q==0)
                int a = 2;
            
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
