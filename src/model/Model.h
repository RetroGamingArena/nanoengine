//
//  Model.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__Model__
#define __Nanocraft__Model__

#include <glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Block.h"
#include "config.h"
#include "Chunk.h"
#include "Player.h"
#include "World.h"

#define MAX_CHUNKS 8192
#define MAX_PLAYERS 128

#define MAX_TEXT_LENGTH 256
#define MAX_PATH_LENGTH 256
#define MAX_ADDR_LENGTH 256

#define MODE_OFFLINE 0
#define MODE_ONLINE 1

#define XZ_SIZE (CHUNK_SIZE * 3 + 2)
#define XZ_LO (CHUNK_SIZE)
#define XZ_HI (CHUNK_SIZE * 2 + 1)
#define Y_SIZE 258
#define XYZ(x, y, z) ((y) * XZ_SIZE * XZ_SIZE + (x) * XZ_SIZE + (z))
#define XZ(x, z) ((x) * XZ_SIZE + (z))

using namespace std;

class Model
{
public:
    GLFWwindow *window;
    
    //vector<Chunk*> chunks;
    World* chunks;
    
    int create_radius;
    int render_radius;
    int delete_radius;
    int sign_radius;
    Player players[MAX_PLAYERS];
    int player_count;
    int typing;
    char typing_buffer[MAX_TEXT_LENGTH];
    int message_index;
    char messages[MAX_MESSAGES][MAX_TEXT_LENGTH];
    int width;
    int height;
    int observe1;
    int observe2;
    int flying;
    int item_index;
    int scale;
    int ortho;
    float fov;
    int suppress_char;
    int mode;
    int mode_changed;
    char db_path[MAX_PATH_LENGTH];
    char server_addr[MAX_ADDR_LENGTH];
    int server_port;
    int time_changed;
    Block block0;
    Block block1;
    Block copy0;
    Block copy1;
    
    Model();
    void reset();
    Player *playerCrosshair(Player *player);
    Player *findPlayer(int id);
    int collide(int height, State *s);
    int collide(int height, float *x, float *y, float *z);
    int highestBlock(float x, float z);
    int chunkVisible(float planes[6][4], int p, int q, int miny, int maxy);
    //void createChunk(Chunk *chunk, int p, int q);
    void deletePlayer(int id);
    void deleteAllPlayers();
    void deleteChunks();
    void deleteAllChunks();
    void handleMouseInput();
    void handleMovement(double dt);
    void setBlock(int x, int y, int z, int w);
    void unsetSign(int x, int y, int z);
    void unsetSignFace(int x, int y, int z, int face);
    void _setSign(int p, int q, int x, int y, int z, int face, const char *text, int dirty);
    void parseCommand(const char *buffer, int forward);
    void checkWorkers();
    void forceChunks(Player *player);
    void initWorld(Player *player);
    void parseBuffer(char *buffer);
    void onLight();
    void onMiddleClick();
    void onScroll(GLFWwindow *window, double xdelta, double ydelta);
    void onChar(GLFWwindow *window, unsigned int u);
    int hitTestFace(Player *player, int *x, int *y, int *z, int *face);
    int hitTest(int previous, float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz);
    static int chunked(float x);
    void setSign(int x, int y, int z, int face, const char *text);
    //Chunk *findChunk(int p, int q);
    void addMessage(const char *text);
    void onMouseButton(GLFWwindow *window, int button, int action, int mods);
    void toggleLight(int x, int y, int z);
    void _setBlock(int p, int q, int x, int y, int z, int w, int dirty);
    void recordBlock(int x, int y, int z, int w);
    int getBlock(int x, int y, int z);
    int playerIntersectsBlock(int height, float x, float y, float z, int hx, int hy, int hz);
    void copy();
    void paste();
    void onKey(GLFWwindow *window, int key, int scancode, int action, int mods);
    void tree(Block *block);
    void array(Block *b1, Block *b2, int xc, int yc, int zc);
    void cube(Block *b1, Block *b2, int fill);
    void sphere(Block *center, int radius, int fill, int fx, int fy, int fz);
    void cylinder(Block *b1, Block *b2, int radius, int fill);
    //void dirtyChunk(Chunk *chunk);
    void setLight(int p, int q, int x, int y, int z, int w);
    void builderBlock(int x, int y, int z, int w);
    //int hasLights(Chunk *chunk);
    void requestChunk(int p, int q);
    void initChunk(Chunk *chunk, int p, int q);
    void onLeftClick();
    void onRightClick();
};

#endif /* defined(__Nanocraft__Model__) */
