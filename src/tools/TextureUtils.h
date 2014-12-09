//
//  TextureUtils.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 19/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__TextureUtils__
#define __Nanocraft__TextureUtils__

class TextureUtils
{
    public:
        static void loadTexture(const char* texturePath, GLenum activeTexture, int minMagFilter, bool clampToEdge);
};

#endif /* defined(__Nanocraft__TextureUtils__) */
