//
//  SubView.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__SubView__
#define __Nanocraft__SubView__

#include <stdio.h>

#include "Model.h"
#include "Player.h"
#include "Attrib.h"

class SubView
{
    protected:
        Attrib* attrib;

    public:
        SubView(Attrib* attrib){this->attrib = attrib;};
        virtual void render(Model* model, Player* player) = 0;
};

#endif /* defined(__Nanocraft__SubView__) */
