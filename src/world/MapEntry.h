//
//  MapEntry.h
//  Nanocraft
//
//  Created by Julien CAILLABET on 24/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#ifndef __Nanocraft__MapEntry__
#define __Nanocraft__MapEntry__

#include <stdio.h>

typedef union
{
    short value;
    struct
    {
        bool enabled = true;
        bool computed = true;
        char w;
    } e;
} MapEntry;

#endif /* defined(__Nanocraft__MapEntry__) */
