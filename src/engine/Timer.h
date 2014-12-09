//
//  Timer.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 22/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__Timer__
#define __Nanocraft__Timer__

#include <stdio.h>

class Timer
{
    double previous;
    double dt;
    
    public:
    Timer();
        double getDt();
        void refresh();
};

#endif /* defined(__Nanocraft__Timer__) */
