//
//  CubeView.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 28/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "CubeView.h"

CubeView::CubeView() : View("shaders/block_vertex.glsl", "shaders/block_fragment.glsl")
{
    buffer = genBuffer();
}

void CubeView::loadShader()
{
    View::loadShader();
    attrib->program = program;
    attrib->position = glGetAttribLocation(program, "position");
    attrib->normal = glGetAttribLocation(program, "normal");
    attrib->uv = glGetAttribLocation(program, "uv");
    attrib->color = glGetAttribLocation(program, "color");
    attrib->matrix = glGetUniformLocation(program, "matrix");
    attrib->sampler = glGetUniformLocation(program, "sampler");
    attrib->extra1 = glGetUniformLocation(program, "sky_sampler");
    attrib->extra2 = glGetUniformLocation(program, "daylight");
    attrib->extra3 = glGetUniformLocation(program, "fog_distance");
    attrib->extra4 = glGetUniformLocation(program, "ortho");
    attrib->camera = glGetUniformLocation(program, "camera");
    attrib->timer = glGetUniformLocation(program, "timer");
}

void CubeView::render(Model* model, Player* player)
{
    attrib->renderChunks(model, player);
    //glClear(GL_DEPTH_BUFFER_BIT);
    attrib->renderItem(model);
}

GLuint CubeView::genBuffer()
{
    return 0;
}