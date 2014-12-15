#ifndef _map_h_
#define _map_h_

#define EMPTY_ENTRY(entry) ((entry) == 0)//enabled == 1)

#define MAP_FOR_EACH_2(map, ex, ey, ez, esx, esy, esz, ew) \
    for (unsigned int i = 0; i < map->mask; i++) { \
        char entry = map->getData(i); \
        if (EMPTY_ENTRY(entry)) { \
            continue; \
        } \
        int ew = entry;//->e.w;


#define MAP_FOR_EACH(map, ex, ey, ez, ew) \
    for (unsigned int i = 0; i < map->mask; i++) { \
        char entry = map->getData(i); \
        if (EMPTY_ENTRY(entry)) { \
            continue; \
        } \
        int ew = entry;//->e.w;

#define END_MAP_FOR_EACH }

#include "config.h"
#include "nanotypes.h"

class Map
{
    nano_row *data;
    
    public:
        int dx;
        int dy;
        int dz;
        unsigned int mask;
        unsigned int size;
    
        static int getIndex(int x, int y, int z)
        {
            return (x/*+1*/)+(z/*+1*/)*34+y*34*34;
        };
        static int getX(int index);
        static int getY(int index);
        static int getZ(int index);
        int _hitTest(float max_distance, int previous, float x, float y, float z, float vx, float vy, float vz, int *hx, int *hy, int *hz);
        //int hitTestFace(Player *player, int *x, int *y, int *z, int *face);
    
        nano_row* getDatas(){return data;};
        nano getData(int i)
        {
            int itemsPerRow = 8 / ITEM_RANGE;
            int itemSize = 8 / itemsPerRow;
            int rowIndex = i / itemsPerRow;
            int colIndex = i % itemsPerRow;
            
            nano_row current = *(this->data+rowIndex);
            
            current = (current << (colIndex*itemSize)) >> ((colIndex+itemsPerRow-colIndex-1)*itemSize);
            
            return current;
        }
        void setData(int i, nano data)
        {
            int itemsPerRow = 8 / ITEM_RANGE;
            int itemSize = 8 / itemsPerRow;
            int rowIndex = i / itemsPerRow;
            int colIndex = i % itemsPerRow;
            
            if(colIndex>0)
            {
                int a = 0;
                a++;
            }
            
            nano current = *(this->data+rowIndex);
            
            char left = (current >> (itemsPerRow-colIndex)*itemSize) << (itemsPerRow-colIndex)*itemSize;
            char center = data << ((itemsPerRow-colIndex-1)*itemSize);
            char right = (current << (itemsPerRow)*itemSize) >> (itemsPerRow-1)*itemSize;

            *(this->data+rowIndex) = left | center | right;
        }
    void createWorld(int p, int q);
    Map(){};
    Map(int dx, int dy, int dz, int mask);
    Map(Map* other)
    {
        this->dx = other->dx;
        this->dy = other->dy;
        this->dz = other->dz;
        this->mask = other->mask;
        this->size = other->size;
        this->data = new nano_row[(this->mask + 1) / (8/ITEM_RANGE)];
        for(int i = 0 ; i < ((this->mask + 1) / (8/ITEM_RANGE)); i++)
            this->data[i] = other->data[i];
    };
    //static void map_free(Map *map);
    static void map_copy(Map *dst, Map *src);
    static void map_grow(Map *map);
    int set(int x, int y, int z, double dx, double dy, double dz, int w, bool enabled);
    static int map_get(Map *map, int x, int y, int z);
    static void mapSetFunc(int x, int y, int z, double dx, double dy, double dz, int w, void *arg);
};



#endif
