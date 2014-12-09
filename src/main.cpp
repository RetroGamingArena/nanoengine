#include <glew.h>
#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "auth.h"
#include "BufferUtils.h"
#include "client.h"
#include "config.h"
#include "cube.h"
#include "db.h"
#include "item.h"
#include "map.h"
#include "matrix.h"
#include "../deps/noise/noise.h"
#include "sign.h"
#include "../deps/tinycthread/tinycthread.h"
#include "util.h"
#include "engine.h"
#include "TextureUtils.h"
#include "TextView.h"

#define MAX_CHUNKS 8192
#define MAX_PLAYERS 128
#define WORKERS 4
#define MAX_TEXT_LENGTH 256
#define MAX_NAME_LENGTH 32
#define MAX_PATH_LENGTH 256
#define MAX_ADDR_LENGTH 256

#define MODE_OFFLINE 0
#define MODE_ONLINE 1

#define WORKER_IDLE 0
#define WORKER_BUSY 1
#define WORKER_DONE 2

Engine* engine = Engine::getInstance();

static Model *g = Engine::getInstance()->getModel();

int main(int argc, char **argv)
{
    if(!engine->init())
        return -1;
    
    if(!engine->create())
        return -1;

    engine->start();

    // LOAD TEXTURES
    TextureUtils::loadTexture("textures/texture.png", GL_TEXTURE0, GL_NEAREST, false);
    
    TextureUtils::loadTexture("textures/font.png", GL_TEXTURE1, GL_LINEAR, false);
    
    TextureUtils::loadTexture("textures/sky.png", GL_TEXTURE2, GL_LINEAR, true);

    TextureUtils::loadTexture("textures/sign.png", GL_TEXTURE3, GL_NEAREST, false);

    engine->initViews();
    
    // LOAD SHADERS //
    engine->loadShader();

    // LOCAL VARIABLES //
    g->reset();//reset_model();
    
    Player *me = g->players;
    g->initWorld(me);
    
    Player *player = g->players + g->observe1;
    player->state.y = -1;
    //player->state.x += 1;
    //player->state.z += 1;
    
    // BEGIN MAIN LOOP //
    //GLuint sky_buffer = gen_sky_buffer();
    while (!glfwWindowShouldClose(g->window))
    {
        // WINDOW SIZE AND SCALE //
        engine->resize();
        
        //TIMER
        engine->updateFPS();
        engine->getTimer()->refresh();

        // HANDLE MOUSE INPUT //
        g->handleMouseInput();

        // HANDLE MOVEMENT //
        g->handleMovement(engine->getTimer()->getDt());

        // PREPARE TO RENDER //
        g->deleteChunks();
        del_buffer(me->buffer);
        /*me->buffer = BufferUtils::genPlayerBuffer(s->x, s->y, s->z, s->rx, s->ry);
        for (int i = 1; i < g->player_count; i++) {
            (g->players + i)->interpolate();
        }*/
        Player *player = g->players + g->observe1;
        //player->state.y = 19;
        
        /*player->state.x += 1;
        player->state.z += 1;*/
        
        // RENDER 3-D SCENE //
        engine->render(player);
        
        //render_sky(&engine->sky_attrib, player, sky_buffer);
        engine->getViews()[0]->render(g, me);
            glClear(GL_DEPTH_BUFFER_BIT);
        
        //int face_count = 0;//engine->block_attrib.renderChunks(g, player);
        
        engine->getViews()[2]->render(g, me);
        
        // RENDER HUD //
        //glClear(GL_DEPTH_BUFFER_BIT);
        engine->getViews()[1]->render(g, me);
        
        engine->getViews()[3]->render(g, me);

        // SWAP AND POLL //
        glfwSwapBuffers(g->window);
        glfwPollEvents();
        if (g->mode_changed)
        {
            g->mode_changed = 0;
            break;
        }
    }

    engine->stop();
    
    return 0;
}
