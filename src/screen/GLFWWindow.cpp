//
//  GLFWWindow.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 16/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "GLFWWindow.h"
#include "client.h"
#include "engine.h"
#include "item.h"
#include "MathUtils.h"

GLFWWindow::GLFWWindow()
{
    day_length = DAY_LENGTH;
}

bool GLFWWindow::init()
{
    // WINDOW INITIALIZATION //
    int  res = glfwInit();
    glfwSetTime(day_length / 3.0);
    return res;
}

float GLFWWindow::timeOfDay()
{
    if (/*g->*/day_length <= 0)
    {
        return 0.5;
    }
    float t;
    t = glfwGetTime();
    t = t / /*g->*/day_length;
    t = t - (int)t;
    return t;
}

void GLFWWindow::create(Model* model)
{
    int window_width = WINDOW_WIDTH;
    int window_height = WINDOW_HEIGHT;
    GLFWmonitor *monitor = NULL;
    if (FULLSCREEN)
    {
        int mode_count;
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *modes = glfwGetVideoModes(monitor, &mode_count);
        window_width = modes[mode_count - 1].width;
        window_height = modes[mode_count - 1].height;
    }
    model->window = glfwCreateWindow(window_width, window_height, "Craft", monitor, NULL);
}

void GLFWWindow::start(Model* model)
{
    glfwMakeContextCurrent(model->window);
    glfwSwapInterval(VSYNC);
    glfwSetInputMode(model->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(model->window, on_key);
    glfwSetCharCallback(model->window, on_char);
    glfwSetMouseButtonCallback(model->window, on_mouse_button);
    glfwSetScrollCallback(model->window, on_scroll);
}

void GLFWWindow::on_key(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Model* g = Engine::getInstance()->getModel();
    
    int control = mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER);
    int exclusive =
    glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    if (action == GLFW_RELEASE) {
        return;
    }
    if (key == GLFW_KEY_BACKSPACE) {
        if (g->typing) {
            int n = (int)strlen(g->typing_buffer);
            if (n > 0) {
                g->typing_buffer[n - 1] = '\0';
            }
        }
    }
    if (action != GLFW_PRESS) {
        return;
    }
    if (key == GLFW_KEY_ESCAPE) {
        if (g->typing) {
            g->typing = 0;
        }
        else if (exclusive) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    if (key == GLFW_KEY_ENTER) {
        if (g->typing) {
            if (mods & GLFW_MOD_SHIFT) {
                int n = (int)strlen(g->typing_buffer);
                if (n < MAX_TEXT_LENGTH - 1) {
                    g->typing_buffer[n] = '\r';
                    g->typing_buffer[n + 1] = '\0';
                }
            }
            else {
                g->typing = 0;
                if (g->typing_buffer[0] == CRAFT_KEY_SIGN) {
                    Player *player = g->players;
                    int x, y, z, face;
                    if (g->hitTestFace(player, &x, &y, &z, &face))
                    {
                        g->setSign(
                        /*set_sign(*/x, y, z, face, g->typing_buffer + 1);
                    }
                }
                else if (g->typing_buffer[0] == '/') {
                    g->parseCommand(g->typing_buffer, 1);
                }
                else {
                    client_talk(g->typing_buffer);
                }
            }
        }
        else {
            if (control) {
                on_right_click();
            }
            else {
                on_left_click();
            }
        }
    }
    if (control && key == 'V') {
        const char *buffer = glfwGetClipboardString(window);
        if (g->typing) {
            g->suppress_char = 1;
            strncat(g->typing_buffer, buffer,
                    MAX_TEXT_LENGTH - strlen(g->typing_buffer) - 1);
        }
        else {
            g->parseCommand(buffer, 0);
        }
    }
    if (!g->typing) {
        if (key == CRAFT_KEY_FLY) {
            g->flying = !g->flying;
        }
        if (key >= '1' && key <= '9') {
            g->item_index = key - '1';
        }
        if (key == '0') {
            g->item_index = 9;
        }
        if (key == CRAFT_KEY_ITEM_NEXT) {
            g->item_index = (g->item_index + 1) % item_count;
        }
        if (key == CRAFT_KEY_ITEM_PREV) {
            g->item_index--;
            if (g->item_index < 0) {
                g->item_index = item_count - 1;
            }
        }
        if (key == CRAFT_KEY_OBSERVE) {
            g->observe1 = (g->observe1 + 1) % g->player_count;
        }
        if (key == CRAFT_KEY_OBSERVE_INSET) {
            g->observe2 = (g->observe2 + 1) % g->player_count;
        }
    }
}

void GLFWWindow::on_char(GLFWwindow *window, unsigned int u) {
    Model* g = Engine::getInstance()->getModel();
    
    if (g->suppress_char) {
        g->suppress_char = 0;
        return;
    }
    if (g->typing) {
        if (u >= 32 && u < 128) {
            char c = (char)u;
            int n = (int)strlen(g->typing_buffer);
            if (n < MAX_TEXT_LENGTH - 1) {
                g->typing_buffer[n] = c;
                g->typing_buffer[n + 1] = '\0';
            }
        }
    }
    else {
        if (u == CRAFT_KEY_CHAT) {
            g->typing = 1;
            g->typing_buffer[0] = '\0';
        }
        if (u == CRAFT_KEY_COMMAND) {
            g->typing = 1;
            g->typing_buffer[0] = '/';
            g->typing_buffer[1] = '\0';
        }
        if (u == CRAFT_KEY_SIGN) {
            g->typing = 1;
            g->typing_buffer[0] = CRAFT_KEY_SIGN;
            g->typing_buffer[1] = '\0';
        }
    }
}

void GLFWWindow::on_scroll(GLFWwindow *window, double xdelta, double ydelta)
{
    Model* g = Engine::getInstance()->getModel();
    
    static double ypos = 0;
    ypos += ydelta;
    if (ypos < -SCROLL_THRESHOLD) {
        g->item_index = (g->item_index + 1) % item_count;
        ypos = 0;
    }
    if (ypos > SCROLL_THRESHOLD) {
        g->item_index--;
        if (g->item_index < 0) {
            g->item_index = item_count - 1;
        }
        ypos = 0;
    }
}

void GLFWWindow::on_mouse_button(GLFWwindow *window, int button, int action, int mods) {
    int control = mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER);
    int exclusive =
    glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    if (action != GLFW_PRESS) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (exclusive) {
            if (control) {
                on_right_click();
            }
            else {
                on_left_click();
            }
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (exclusive) {
            if (control) {
                on_light();
            }
            else {
                on_right_click();
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (exclusive) {
            on_middle_click();
        }
    }
}

void GLFWWindow::on_light()
{
    Model* g = Engine::getInstance()->getModel();
    
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = g->hitTest(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    if (hy > 0 && hy < 256 && is_destructable(hw)) {
        g->toggleLight(hx, hy, hz);
    }
}

void GLFWWindow::on_left_click()
{
    Model* g = Engine::getInstance()->getModel();
    
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = g->hitTest(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    if (hy > 0 && hy < 256 && is_destructable(hw)) {
        g->setBlock(hx, hy, hz, 0);
        g->recordBlock(hx, hy, hz, 0);
        if (is_plant(g->getBlock(hx, hy + 1, hz))) {
            g->setBlock(hx, hy + 1, hz, 0);
        }
    }
}

void GLFWWindow::on_right_click()
{
    Model* g = Engine::getInstance()->getModel();
    
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = g->hitTest(1, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    if (hy > 0 && hy < 256 && is_obstacle(hw)) {
        if (!g->playerIntersectsBlock(2, s->x, s->y, s->z, hx, hy, hz)) {
            g->setBlock(hx, hy, hz, items[g->item_index]);
            g->recordBlock(hx, hy, hz, items[g->item_index]);
        }
    }
}

void GLFWWindow::on_middle_click()
{
     Model* g = Engine::getInstance()->getModel();
    
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = g->hitTest(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    for (int i = 0; i < item_count; i++) {
        if (items[i] == hw) {
            g->item_index = i;
            break;
        }
    }
}

int GLFWWindow::getScaleFactor()
{
    Model* g = Engine::getInstance()->getModel();
    
    int window_width, window_height;
    int buffer_width, buffer_height;
    glfwGetWindowSize(g->window, &window_width, &window_height);
    glfwGetFramebufferSize(g->window, &buffer_width, &buffer_height);
    int result = buffer_width / window_width;
    result = MAX(1, result);
    result = MIN(2, result);
    return result;
}