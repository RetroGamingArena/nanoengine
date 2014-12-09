//
//  AbstractWindow.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 16/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__AbstractWindow__
#define __Nanocraft__AbstractWindow__

#include <stdio.h>
#include "model.h"

#define FULLSCREEN 0
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class AbstractWindow
{
    public:
        int day_length;
        virtual bool init() = 0;
        virtual void create(Model* model) = 0;
        virtual void start(Model* model) = 0;
        virtual int getScaleFactor() = 0;
        virtual float getDayLight();
        virtual float timeOfDay() = 0;
};

#endif /* defined(__Nanocraft__AbstractWindow__) */
