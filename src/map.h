#ifndef _map_h_
#define _map_h_

#include "MapEntry.h"

#define EMPTY_ENTRY(entry) ((entry)->e.enabled == 1)

#define MAP_FOR_EACH_2(map, ex, ey, ez, esx, esy, esz, ew) \
    for (unsigned int i = 0; i < map->mask; i++) { \
        MapEntry *entry = map->data + i; \
        if (EMPTY_ENTRY(entry)) { \
            continue; \
        } \
        int ew = entry->e.w;


#define MAP_FOR_EACH(map, ex, ey, ez, ew) \
    for (unsigned int i = 0; i < map->mask; i++) { \
        MapEntry *entry = map->data + i; \
        if (EMPTY_ENTRY(entry)) { \
            continue; \
        } \
        int ew = entry->e.w;

#define END_MAP_FOR_EACH }

class Map
{
    public:
        int dx;
        int dy;
        int dz;
        unsigned int mask;
        unsigned int size;
        MapEntry *data;
        static int getIndex(int x, int y, int z)
        {
            return (x/*+1*/)+(z/*+1*/)*34+y*34*34;
        };
        static int getX(int index);
        static int getY(int index);
        static int getZ(int index);
        int _hitTest(float max_distance, int previous, float x, float y, float z, float vx, float vy, float vz, int *hx, int *hy, int *hz);
        //int hitTestFace(Player *player, int *x, int *y, int *z, int *face);
    
    void createWorld(int p, int q);
    Map(){};
    Map(int dx, int dy, int dz, int mask);
    //static void map_alloc(Map *map, int dx, int dy, int dz, int mask);
    static void map_free(Map *map);
    static void map_copy(Map *dst, Map *src);
    static void map_grow(Map *map);
    int set(int x, int y, int z, double dx, double dy, double dz, int w, bool enabled);
    static int map_get(Map *map, int x, int y, int z);
    static void mapSetFunc(int x, int y, int z, double dx, double dy, double dz, int w, void *arg);
};



#endif
