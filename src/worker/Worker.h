//
//  Worker.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__Worker__
#define __Nanocraft__Worker__

#include "tinycthread.h"
#include "WorkerItem.h"
#include "Model.h"

#define WORKER_IDLE 0
#define WORKER_BUSY 1
#define WORKER_DONE 2

class Worker
{
    public:
        int index;
        int state;
        thrd_t thrd;
        mtx_t mtx;
        cnd_t cnd;
        WorkerItem item;
        static int worker_run(void *arg);
        void ensureChunks(Player *player, Model* model);
};

#endif /* defined(__Nanocraft__Worker__) */
