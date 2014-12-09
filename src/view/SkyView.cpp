//
//  SkyView.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "SkyView.h"
#include "Engine.h"
#include "matrix.h"
#include "cube.h"
#include "util.h"

SkyView::SkyView() : View("shaders/sky_vertex.glsl", "shaders/sky_fragment.glsl")
{
    buffer = genBuffer();
}

void SkyView::render(Model* model, Player* player)
{
    View::render(model, player);
    AbstractWindow* window = Engine::getInstance()->getAbstractWindow();
    State *s = &player->state;
    float matrix[16];
    set_matrix_3d(matrix, model->width, model->height, 0, 0, 0, s->rx, s->ry, model->fov, 0, model->render_radius);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    glUniform1i(attrib->sampler, 2);
    glUniform1f(attrib->timer, window->timeOfDay());
    drawTriangles3d(attrib, buffer, 512 * 3);
}

void SkyView::loadShader()
{
    View::loadShader();
    attrib->program = program;
    attrib->position = glGetAttribLocation(program, "position");
    attrib->normal = glGetAttribLocation(program, "normal");
    attrib->uv = glGetAttribLocation(program, "uv");
    attrib->matrix = glGetUniformLocation(program, "matrix");
    attrib->sampler = glGetUniformLocation(program, "sampler");
    attrib->timer = glGetUniformLocation(program, "timer");
}

GLuint SkyView::genBuffer()
{
    float data[12288];
    make_sphere(data, 1, 3);
    return gen_buffer(sizeof(data), data);
}