//
//  GLFWWindow.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 16/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__GLFWWindow__
#define __Nanocraft__GLFWWindow__

#include <stdio.h>

#include "AbstractWindow.h"

class GLFWWindow : public AbstractWindow
{
    public:
        GLFWWindow();
        bool init();
        void create(Model* model);
        void start(Model* model);
        int getScaleFactor();
        float timeOfDay();
        static void on_key(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void on_char(GLFWwindow *window, unsigned int u);
        static void on_scroll(GLFWwindow *window, double xdelta, double ydelta);
        static void on_mouse_button(GLFWwindow *window, int button, int action, int mods);
        static void on_light();
        static void on_left_click();
        static void on_right_click();
        static void on_middle_click();
    
};

#endif /* defined(__Nanocraft__GLFWWindow__) */
