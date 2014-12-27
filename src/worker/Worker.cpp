//
//  Worker.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cstdlib>

#include <ctime>
#include <iostream>

#include "Worker.h"
#include "engine.h"
#include "matrix.h"
#include "MathUtils.h"

int Worker::worker_run(void *arg)
{
    clock_t begin = clock();
    
    Worker *worker = (Worker *)arg;
    int running = 1;
    while (running)
    {
        mtx_lock(&worker->mtx);
        while (worker->state != WORKER_BUSY)
            cnd_wait(&worker->cnd, &worker->mtx);
        mtx_unlock(&worker->mtx);
        worker->state = WORKER_DONE;
        mtx_unlock(&worker->mtx);
    }
    clock_t end = clock();
    cout << "rhread " << (end - begin) / (CLOCKS_PER_SEC/1000) << endl;
    
    return 0;
}

void Worker::ensureChunks(Player *player, Model* model)
{
    clock_t begin = clock();
    
    int start = 0x0fffffff;
    int best_score = start;
    int best_a = 0;
    int best_b = 0;
    Chunk *chunk = model->chunks->requestChunk(best_score, best_a, best_b);
    if (best_score == start)
    {
        return;
    }
    int a = best_a;
    int b = best_b;
    int load = 0;
    if (!chunk)
    {
        load = 1;
        if (model->chunks->size() < MAX_CHUNKS)
        {
            model->chunks->push_back(new Chunk(a, b));
            chunk = (*model->chunks)[model->chunks->size()-1];
        }
        else
        {
            return;
        }
    }
    item.chunk = chunk;
    item.chunk->load = load;
    chunk->dirty = 0;
    state = WORKER_BUSY;
    cnd_signal(&cnd);
    
    clock_t end = clock();
    cout << "worker " << (end - begin) / (CLOCKS_PER_SEC/1000) << endl;
}