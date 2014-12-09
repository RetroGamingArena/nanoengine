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
        if (item->load)
        {
            item->loadChunk();
        }
        item->computeChunk(model->chunks);
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
    printf("%i %i\n",a,b);
    Chunk *chunk = model->chunks->findChunk(a, b);
    if (!chunk) {
        load = 1;
        if (model->chunks->size() < MAX_CHUNKS) {
            model->chunks->push_back(new Chunk(a, b));
            //chunks.resize(chunks.size()+1);
            chunk = (*model->chunks)[model->chunks->size()-1];
            model->chunks->initChunk(chunk, a, b);
        }
        else {
            return;
        }
    }
    item.p = chunk->p;
    item.q = chunk->q;
    item.load = load;
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            Chunk *other = chunk;
            if (dp || dq) {
                other = model->chunks->findChunk(chunk->p + dp, chunk->q + dq);
            }
            if (other) {
                Map *block_map = (Map*)malloc(sizeof(Map));
                Map::map_copy(block_map, &other->map);
                Map *light_map = (Map*)malloc(sizeof(Map));
                Map::map_copy(light_map, &other->lights);
                item.block_maps[dp + 1][dq + 1] = block_map;
                item.light_maps[dp + 1][dq + 1] = light_map;
            }
            else {
                item.block_maps[dp + 1][dq + 1] = 0;
                item.light_maps[dp + 1][dq + 1] = 0;
            }
        }
    }
    chunk->dirty = 0;
    state = WORKER_BUSY;
    cnd_signal(&cnd);
}