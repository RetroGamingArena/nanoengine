//
//  LinesView.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__LinesView__
#define __Nanocraft__LinesView__

#include <stdio.h>

#include "View.h"

class LinesView : public View
{
    vector<SubView*> subViews;

    public:
        LinesView();
        void loadShader();
    GLuint genBuffer(){return 0;};
        void render(Model* model, Player* player);
};

#endif /* defined(__Nanocraft__LinesView__) */
