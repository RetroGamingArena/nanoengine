//
//  Worker.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cstdlib>

#include "Worker.h"
#include "engine.h"
#include "matrix.h"
#include "MathUtils.h"

int Worker::worker_run(void *arg)
{
    Model* model = Engine::getInstance()->getModel();
    
    Worker *worker = (Worker *)arg;
    int running = 1;
    while (running)
    {
        mtx_lock(&worker->mtx);
        while (worker->state != WORKER_BUSY)
        {
            cnd_wait(&worker->cnd, &worker->mtx);
        }
        mtx_unlock(&worker->mtx);
        WorkerItem *item = &worker->item;
        if (item->chunk->load/*item->load*/)
        {
            item->chunk->map.createWorld(item->chunk->p, item->chunk->q);
        }
        model->chunks->at(0)->compute();
        //item->computeChunk(model->chunks);
        mtx_lock(&worker->mtx);
        worker->state = WORKER_DONE;
        mtx_unlock(&worker->mtx);
    }
    return 0;
}

void Worker::ensureChunks(Player *player, Model* model)
{
    State *s = &player->state;
    float matrix[16];
    set_matrix_3d(matrix, model->width, model->height, s->x, s->y, s->z, s->rx, s->ry, model->fov, model->ortho, model->render_radius);
    float planes[6][4];
    frustum_planes(planes, model->render_radius, matrix);
    int p = Chunk::chunked(s->x);
    int q = Chunk::chunked(s->z);
    int r = model->create_radius;
    int start = 0x0fffffff;
    int best_score = start;
    int best_a = 0;
    int best_b = 0;
    for (int dp = -r; dp <= r; dp++) {
        for (int dq = -r; dq <= r; dq++) {
            int a = p + dp;
            int b = q + dq;
            int index = (ABS(a) ^ ABS(b)) % Engine::WORKERS;
            if (index != this->index) {
                continue;
            }
            Chunk *chunk = model->chunks->findChunk(a, b);
            if (chunk && !chunk->dirty) {
                continue;
            }
            int distance = MAX(ABS(dp), ABS(dq));
            int invisible = !model->chunkVisible(planes, a, b, 0, 256);
            int priority = 0;
            if (chunk) {
                priority = chunk->buffer && chunk->dirty;
            }
            int score = (invisible << 24) | (priority << 16) | distance;
            if (score < best_score) {
                best_score = score;
                best_a = a;
                best_b = b;
            }
        }
    }
    if (best_score == start) {
        return;
    }
    int a = best_a;
    int b = best_b;
    int load = 0;
    Chunk *chunk = model->chunks->findChunk(a, b);
    if (!chunk) {
        load = 1;
        if (model->chunks->size() < MAX_CHUNKS) {
            model->chunks->push_back(new Chunk(a, b));
            chunk = (*model->chunks)[model->chunks->size()-1];
            model->chunks->initChunk(chunk, a, b);
        }
        else {
            return;
        }
    }
    item.chunk = chunk;
    item.chunk->load = load;
    chunk->dirty = 0;
    state = WORKER_BUSY;
    cnd_signal(&cnd);
}