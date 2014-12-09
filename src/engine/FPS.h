//
//  FPS.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 22/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__FPS__
#define __Nanocraft__FPS__

#include <stdio.h>

class FPS
{
public:
    unsigned int fps;
    unsigned int frames;
    double since;
    FPS();
};

#endif /* defined(__Nanocraft__FPS__) */
