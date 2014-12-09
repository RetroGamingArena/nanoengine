//
//  AbstractWindow.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 16/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cmath>

#include "AbstractWindow.h"

float AbstractWindow::getDayLight()
{
    float timer = timeOfDay();
    if (timer < 0.5)
    {
        float t = (timer - 0.25) * 100;
        return 1 / (1 + powf(2, -t));
    }
    else
    {
        float t = (timer - 0.85) * 100;
        return 1 - 1 / (1 + powf(2, -t));
    }
}