//
//  SkyView.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__SkyView__
#define __Nanocraft__SkyView__

#include <stdio.h>

#include "View.h"
#include "Model.h"
#include "Player.h"

class SkyView : public View
{
    public:
        SkyView();
        void render(Model* model, Player* player);
        void loadShader();
        GLuint genBuffer();
};

#endif /* defined(__Nanocraft__SkyView__) */
