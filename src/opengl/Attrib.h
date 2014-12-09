//
//  Attrib.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 21/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__Attrib__
#define __Nanocraft__Attrib__

#include <stdio.h>
#include <glew.h>

#include "Chunk.h"
#include "Player.h"
#include "Model.h"
#include "Worker.h"

class Attrib
{
public:
    GLuint program;
    GLuint position;
    GLuint normal;
    GLuint uv;
    GLuint matrix;
    GLuint sampler;
    GLuint camera;
    GLuint timer;
    GLuint color;
    GLuint extra1;
    GLuint extra2;
    GLuint extra3;
    GLuint extra4;
    
    void drawLines(GLuint buffer, int components, int count);
    void renderItem(Model* model);
    void drawCube(GLuint buffer);
    void drawPlant(GLuint buffer);
    int renderChunks(Model *model, Player *player);
    void drawItem(GLuint buffer, int count);
    void drawTriangles3dAo(GLuint buffer, int count);
    void drawTriangles3dText(GLuint buffer, int count);
    void drawTriangles3d(GLuint buffer, int count);
    void drawTriangles2d(GLuint buffer, int count);
    void drawSign(GLuint buffer, int length);
    void drawSigns(Chunk *chunk);
    void drawText(GLuint buffer, int length);
    void drawChunk(Chunk *chunk);
    void drawPlayer(Player *player);
    void renderPlayers(Model *model, Player *player);
    void renderSign(Model *model, Player *player);
    void renderSigns(Model *model, Player *player);
    void renderText(Model *model, int justify, float x, float y, float n, char *text);
};

#endif /* defined(__Nanocraft__Attrib__) */
