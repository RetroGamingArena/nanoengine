//
//  Engine.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 16/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <stdlib.h>

#include <ctime>
#include <iostream>

#include <cmath>
#include <curl/curl.h>
#include "Engine.h"
#include "GLFWWindow.h"
#include "SkyView.h"
#include "LinesView.h"
#include "util.h"
#include "client.h"
#include "CubeView.h"
#include "TextView.h"

Engine* Engine::instance = NULL;

int Engine::WORKERS = 2;

bool Engine::init()
{
    // INITIALIZATION //
    curl_global_init(CURL_GLOBAL_DEFAULT);
    srand(static_cast<unsigned int>(time(NULL)));
    rand();
    
    return abstractWindow->init();
}

void Engine::initWorkers()
{
    // INITIALIZE WORKER THREADS
    //
    for (int i = 0; i < WORKERS; i++)
        workers.push_back(new Worker());
                          
    for (int i = 0; i < workers.size(); i++)
    {
        Worker *worker = workers[i];
        worker->index = i;
        worker->state = WORKER_IDLE;
        mtx_init(&worker->mtx, mtx_plain);
        cnd_init(&worker->cnd);
        thrd_create(&worker->thrd, Worker::worker_run, worker);
    }
}

void Engine::initViews()
{
    views.push_back(new SkyView());
    views.push_back(new LinesView());
    views.push_back(new CubeView());
    views.push_back(new TextView());
}

Engine::Engine()
{
    fps = new FPS();
    timer = new Timer();
    abstractWindow = new GLFWWindow();
    model = new Model();
    g = getModel();
    
    
}

Engine* Engine::getInstance()
{
    if(instance == NULL)
        instance = new Engine();
    return instance;
}

int Engine::create()
{
    abstractWindow->create(model);
    
    if (!model->window)
    {
        glfwTerminate();
        return -1;
    }
    return 1;
}

int Engine::start()
{
    abstractWindow->start(model);
    
    if (glewInit() != GLEW_OK)
    {
        return -1;
    }
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glLogicOp(GL_INVERT);
    glClearColor(0, 0, 0, 1);
    
    initWorkers();
    
    return 1;
}

void Engine::loadShader()
{    
    for(int i = 0 ; i < views.size(); i++)
        views[i]->loadShader();
}

void Engine::updateFPS()
{
    fps->frames++;
    double now = glfwGetTime();
    double elapsed = now - fps->since;
    if (elapsed >= 1) {
        fps->fps = round(fps->frames / elapsed);
        fps->frames = 0;
        fps->since = now;
    }
}

void Engine::resize()
{
    model->scale = getAbstractWindow()->getScaleFactor();
    glfwGetFramebufferSize(model->window, &model->width, &model->height);
    glViewport(0, 0, model->width, model->height);
}

void Engine::render(Player* player)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Engine::checkWorkers()
{
    for (int i = 0; i < workers.size(); i++)
    {
        Worker *worker = workers[i];
        mtx_lock(&worker->mtx);
        if (worker->state == WORKER_DONE)
        {
            Chunk *chunk = worker->workingChunk;
            if (chunk)
            {
                chunk->compute();
                chunk->generate();
                chunk->empty();
            }
            worker->state = WORKER_IDLE;
        }
        mtx_unlock(&worker->mtx);
    }
}

void Engine::ensureChunks(Player *player)
{
    checkWorkers();
    
    for (int i = 0; i < workers.size(); i++)
    {
        Worker *worker = workers[i];
        mtx_lock(&worker->mtx);
        if (worker->state == WORKER_IDLE)
            worker->ensureChunks(player, this->model);
        mtx_unlock(&worker->mtx);
    }
}

void Engine::stop()
{
    // SHUTDOWN //
    client_stop();
    client_disable();
    //del_buffer(sky_buffer); TODO
    model->deleteAllChunks();
    model->deleteAllPlayers();
    
    glfwTerminate();
    curl_global_cleanup();
}

using namespace std;

int Engine::run()
{
    if(!init())
        return -1;
    
    if(!create())
        return -1;
    
    start();
    
    // LOAD TEXTURES
    TextureUtils::loadTexture("textures/texture.png", GL_TEXTURE0, GL_NEAREST, false);
    
    TextureUtils::loadTexture("textures/font.png", GL_TEXTURE1, GL_LINEAR, false);
    
    TextureUtils::loadTexture("textures/sky.png", GL_TEXTURE2, GL_LINEAR, true);
    
    TextureUtils::loadTexture("textures/sign.png", GL_TEXTURE3, GL_NEAREST, false);
    
    initViews();
    
    // LOAD SHADERS //
    loadShader();
    
    // LOCAL VARIABLES //
    g->reset();
    
    Player *me = g->players;
    g->initWorld(me);
    
    Player *player = g->players + g->observe1;
    player->state.y = -1;
    
    // BEGIN MAIN LOOP //
    //GLuint sky_buffer = gen_sky_buffer();
    while (!glfwWindowShouldClose(g->window))
    {
        // WINDOW SIZE AND SCALE //
        resize();
        
        //TIMER
        updateFPS();
        getTimer()->refresh();
        
        // HANDLE MOUSE INPUT //
        g->handleMouseInput();
        
        // HANDLE MOVEMENT //
        g->handleMovement(getTimer()->getDt());
        
        // PREPARE TO RENDER //
        g->deleteChunks();
        del_buffer(me->buffer);
        
        Player *player = g->players + g->observe1;
        render(player);
        
        //render_sky(&engine->sky_attrib, player, sky_buffer);
        
        
        getViews()[0]->render(g, me);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        getViews()[2]->render(g, me);
        
        // RENDER HUD //
        getViews()[1]->render(g, me);
        getViews()[3]->render(g, me);
        
        // SWAP AND POLL //
        glfwSwapBuffers(g->window);
        glfwPollEvents();
        if (g->mode_changed)
        {
            g->mode_changed = 0;
            break;
        }
    }
    stop();
    return 0;
}
