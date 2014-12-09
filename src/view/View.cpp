//
//  View.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 23/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cstdlib>
#include <string>

#include "View.h"
#include "util.h"

View::View(const char *path1, const char *path2)
{
    attrib = new Attrib();
    
    vertexShaderPath = new char[strlen(path1)];
    fragmentShaderPath = new char[strlen(path2)];
    strcpy(vertexShaderPath, path1);
    strcpy(fragmentShaderPath, path2);
}

void View::render(Model* model, Player* player)
{
    
}

void View::loadShader()
{
    program = loadProgram(vertexShaderPath, fragmentShaderPath);
}

GLuint View::loadProgram(const char *path1, const char *path2)
{
    GLuint shader1 = loadShader(GL_VERTEX_SHADER, path1);
    GLuint shader2 = loadShader(GL_FRAGMENT_SHADER, path2);
    GLuint program = makeProgram(shader1, shader2);
    return program;
}

GLuint View::loadShader(GLenum type, const char *path)
{
    char *data = load_file(path);
    GLuint result = makeShader(type, data);
    free(data);
    return result;
}

GLuint View::makeProgram(GLuint shader1, GLuint shader2)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, shader1);
    glAttachShader(program, shader2);
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar *info = (GLchar *)calloc(length, sizeof(GLchar));
        glGetProgramInfoLog(program, length, NULL, info);
        fprintf(stderr, "glLinkProgram failed: %s\n", info);
        free(info);
    }
    glDetachShader(program, shader1);
    glDetachShader(program, shader2);
    glDeleteShader(shader1);
    glDeleteShader(shader2);
    return program;
}

GLuint View::makeShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar *info = (GLchar *)calloc(length, sizeof(GLchar));
        glGetShaderInfoLog(shader, length, NULL, info);
        fprintf(stderr, "glCompileShader failed:\n%s\n", info);
        free(info);
    }
    return shader;
}

void View::drawTriangles3d(Attrib *attrib, GLuint buffer, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(attrib->position);
    glEnableVertexAttribArray(attrib->normal);
    glEnableVertexAttribArray(attrib->uv);
    glVertexAttribPointer(attrib->position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 8, 0);
    glVertexAttribPointer(attrib->normal, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 8, (GLvoid *)(sizeof(GLfloat) * 3));
    glVertexAttribPointer(attrib->uv, 2, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 8, (GLvoid *)(sizeof(GLfloat) * 6));
    glDrawArrays(GL_TRIANGLES, 0, count);
    glDisableVertexAttribArray(attrib->position);
    glDisableVertexAttribArray(attrib->normal);
    glDisableVertexAttribArray(attrib->uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}