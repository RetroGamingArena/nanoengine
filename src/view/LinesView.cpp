//
//  LinesView.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "LinesView.h"
#include "CrossHairView.h"
#include "WireframeView.h"

LinesView::LinesView() : View("shaders/line_vertex.glsl", "shaders/line_fragment.glsl")
{
    subViews.push_back(new WireframeView(attrib));
    subViews.push_back(new CrossHairView(attrib));
}

void LinesView::loadShader()
{
    View::loadShader();
    attrib->program = program;
    attrib->position = glGetAttribLocation(program, "position");
    attrib->matrix = glGetUniformLocation(program, "matrix");
}

void LinesView::render(Model* model, Player* player)
{
    for(int i = 0; i < subViews.size(); i++)
        subViews[i]->render(model, player);
     //glClear(GL_DEPTH_BUFFER_BIT); entre les deux
}