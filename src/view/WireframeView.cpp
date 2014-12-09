//
//  WireframeView.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "WireframeView.h"
#include "matrix.h"
#include "item.h"
#include "util.h"
#include "cube.h"

void WireframeView::render(Model *model, Player *player)
{
    State *s = &player->state;
    float matrix[16];
    set_matrix_3d(matrix, model->width, model->height, s->x, s->y, s->z, s->rx, s->ry, model->fov, model->ortho, model->render_radius);
    int hx, hy, hz;
    int hw = model->hitTest(0, s->x+0.5, s->y, s->z+0.5, s->rx, s->ry, &hx, &hy, &hz);

    if (is_obstacle(hw))
    {
        glUseProgram(attrib->program);
        glLineWidth(1);
        glEnable(GL_COLOR_LOGIC_OP);
        glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
        GLuint wireframe_buffer = genBuffer(hx, hy, hz, 0.53);
        attrib->drawLines(wireframe_buffer, 3, 24);
        del_buffer(wireframe_buffer);
        glDisable(GL_COLOR_LOGIC_OP);
    }
}

GLuint WireframeView::genBuffer(float x, float y, float z, float n)
{
    float data[72];
    make_cube_wireframe(data, x, y, z, n);
    return gen_buffer(sizeof(data), data);
}