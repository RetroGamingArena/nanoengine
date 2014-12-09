//
//  TextView.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 28/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__TextView__
#define __Nanocraft__TextView__

#include <stdio.h>

#include "View.h"

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

class TextView : public View
{
    public:
        TextView();
    void loadShader();
    void render(Model* model, Player* player);
    GLuint genBuffer();
};

#endif /* defined(__Nanocraft__TextView__) */
