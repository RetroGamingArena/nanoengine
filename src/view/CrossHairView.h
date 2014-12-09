//
//  CrossHairView.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__CrossHairView__
#define __Nanocraft__CrossHairView__

#include <stdio.h>

#include "SubView.h"

class CrossHairView : public SubView
{
    public:
        CrossHairView(Attrib* attrib) : SubView(attrib){};
        void render(Model* model, Player* player);
        GLuint genBuffer(Model* model);
    
};

#endif /* defined(__Nanocraft__CrossHairView__) */
