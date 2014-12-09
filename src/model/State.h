//
//  State.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__State__
#define __Nanocraft__State__

class State
{
    public:
        float x;
        float y;
        float z;
        float rx;
        float ry;
        float t;
    
        float dx;
        float dy = 0;
        float dz;
};

#endif /* defined(__Nanocraft__State__) */
