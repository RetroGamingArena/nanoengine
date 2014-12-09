//
//  CrossHairView.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "CrossHairView.h"
#include "matrix.h"
#include "util.h"

void CrossHairView::render(Model* model, Player* player)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    
    float matrix[16];
    set_matrix_2d(matrix, model->width, model->height);
    glUseProgram(attrib->program);
    glLineWidth(4 * model->scale);
    glEnable(GL_COLOR_LOGIC_OP);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    GLuint crosshair_buffer = genBuffer(model);
    attrib->drawLines(crosshair_buffer, 2, 4);
    del_buffer(crosshair_buffer);
    glDisable(GL_COLOR_LOGIC_OP);
}

GLuint CrossHairView::genBuffer(Model* model)
{
    int x = model->width / 2;
    int y = model->height / 2;
    int p = 10 * model->scale;
    float data[] =
    {
        (float)x, (float)y - p, (float)x, (float)y + p,
        (float)x - p, (float)y, (float)x + p, (float)y
    };
    return gen_buffer(sizeof(data), data);
 }