//
//  View.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__View__
#define __Nanocraft__View__

#include <glew.h>
#include <vector>

#include "Attrib.h"
#include "Model.h"
#include "Player.h"
#include "SubView.h"

class View
{
    protected:
        Attrib* attrib;
        GLuint program;
        GLuint buffer;
        char* vertexShaderPath;
        char* fragmentShaderPath;
    
        public:
        View(const char *path1, const char *path2);
        virtual void render(Model* model, Player* player) = 0;
        virtual void loadShader();
        virtual GLuint genBuffer() = 0; // TODO ?
        static GLuint loadProgram(const char *path1, const char *path2);
        static GLuint loadShader(GLenum type, const char *path);
        static GLuint makeProgram(GLuint shader1, GLuint shader2);
        static GLuint makeShader(GLenum type, const char *source);
        static void drawTriangles3d(Attrib *attrib, GLuint buffer, int count);
};

#endif /* defined(__Nanocraft__View__) */
