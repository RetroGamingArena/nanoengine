//
//  WireframeView.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__WireframeView__
#define __Nanocraft__WireframeView__

#include <stdio.h>

#include "SubView.h"

class WireframeView : public SubView
{
    public:
        WireframeView(Attrib* attrib) : SubView(attrib){};
        void render(Model* model, Player* player);
        GLuint genBuffer(float x, float y, float z, float n);
};

#endif /* defined(__Nanocraft__WireframeView__) */
