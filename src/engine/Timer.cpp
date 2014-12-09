//
//  Timer.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 22/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <glew.h>
#include <GLFW/glfw3.h>

#include "Timer.h"
#include "util.h"
#include "MathUtils.h"

Timer::Timer()
{
    previous = 0;
}

double Timer::getDt()
{
    return dt;
}

void Timer::refresh()
{
    double now = glfwGetTime();
    dt = now - previous;
    dt = MIN(dt, 0.2);
    dt = MAX(dt, 0.0);
    previous = now;
}