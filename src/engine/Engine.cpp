//
//  Engine.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 16/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <stdlib.h>
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

int Engine::WORKERS = 4;

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
    /*for(int i = 0 ; i < views.size(); i++)
        views[i]->render(model, player);*/
}

void Engine::checkWorkers()
{
    for (int i = 0; i < workers.size(); i++)
    {
        Worker *worker = workers[i];
        mtx_lock(&worker->mtx);
        if (worker->state == WORKER_DONE) {
            WorkerItem *item = &worker->item;
            Chunk *chunk = model->chunks->findChunk(item->p, item->q);
            if (chunk) {
                if (item->load) {
                    Map *block_map = item->block_maps[1][1];
                    Map *light_map = item->light_maps[1][1];
                    Map::map_free(&chunk->map);
                    Map::map_free(&chunk->lights);
                    Map::map_copy(&chunk->map, block_map);
                    Map::map_copy(&chunk->lights, light_map);
                    model->requestChunk(item->p, item->q);
                }
                item->generateChunk(chunk);
            }
            for (int a = 0; a < 3; a++) {
                for (int b = 0; b < 3; b++) {
                    Map *block_map = item->block_maps[a][b];
                    Map *light_map = item->light_maps[a][b];
                    if (block_map) {
                        Map::map_free(block_map);
                        free(block_map);
                    }
                    if (light_map) {
                        Map::map_free(light_map);
                        free(light_map);
                    }
                }
            }
            worker->state = WORKER_IDLE;
        }
        mtx_unlock(&worker->mtx);
    }
}

void Engine::ensureChunks(Player *player)
{
    checkWorkers();
    //forceChunks(player);
    for (int i = 0; i < workers.size(); i++) {
        Worker *worker = workers[i];
        mtx_lock(&worker->mtx);
        if (worker->state == WORKER_IDLE) {
            worker->ensureChunks(player, this->model);
        }
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
