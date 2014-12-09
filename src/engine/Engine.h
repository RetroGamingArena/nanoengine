//
//  Engine.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 16/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__Engine__
#define __Nanocraft__Engine__

#include <stdio.h>
#include <vector>

#include "AbstractWindow.h"
#include "Attrib.h"
#include "model.h"
#include "FPS.h"
#include "Timer.h"
#include "View.h"
#include "Worker.h"

using namespace std;

class Engine
{
    vector<View*> views;
    vector<Worker*> workers;
    AbstractWindow* abstractWindow;
    Model* model;
    Timer* timer;
    Engine();
    static Engine *instance;

    public:
    static int WORKERS;
    Attrib block_attrib;
    //Attrib line_attrib;
    Attrib text_attrib;
    Attrib sky_attrib;
    FPS* fps;
    
    static Engine* getInstance();
    vector<View*> getViews(){ return views;};
    vector<Worker*> getWorkers(){ return workers;};
    void updateFPS();
    void checkWorkers();
    Model* getModel(){return model;};
    Timer* getTimer(){return timer;};
    AbstractWindow* getAbstractWindow(){return abstractWindow;};
    bool init();
    int create();
    int start();
    void initViews();
    void initWorkers();
    void ensureChunks(Player *player);
    //void ensureChunksWorker(Player *player, Worker *worker);
    void loadShader();
    void resize();
    void render(Player* player);
    void stop();
};

#endif /* defined(__Nanocraft__Engine__) */
