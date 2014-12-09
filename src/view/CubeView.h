//
//  CubeView.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 28/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__CubeView__
#define __Nanocraft__CubeView__

#include <stdio.h>

#include "View.h"

class CubeView : public View
{
    public:
        CubeView();
        void loadShader();
        void render(Model* model, Player* player);
        GLuint genBuffer();
};

#endif /* defined(__Nanocraft__CubeView__) */
