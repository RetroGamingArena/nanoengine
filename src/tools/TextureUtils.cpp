//
//  TextureUtils.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 19/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <glew.h>

#include "TextureUtils.h"
#include "lodepng.h"
#include "util.h"

void TextureUtils::loadTexture(const char* texturePath, GLenum activeTexture, int minMagFilter, bool clampToEdge)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(activeTexture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFilter);
    if(clampToEdge)
    {
        
    }
    load_png_texture(texturePath);
}