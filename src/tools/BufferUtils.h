//
//  BufferUtils.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 24/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__BufferUtils__
#define __Nanocraft__BufferUtils__

#include <stdio.h>
#include <glew.h>

class BufferUtils
{
    public:
    static GLuint genPlantBuffer(float x, float y, float z, float n, int w);
    static GLuint genCubeBuffer(float x, float y, float z, float n, int w);
    static GLuint genPlayerBuffer(float x, float y, float z, float rx, float ry);
    static GLuint genTextBuffer(float x, float y, float n, char *text);
    static int _genSignBuffer(GLfloat *data, float x, float y, float z, int face, const char *text);
};

#endif /* defined(__Nanocraft__BufferUtils__) */
