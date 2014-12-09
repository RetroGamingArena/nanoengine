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
    char value;
    struct
    {
        //bool computed = true;
        char w = 0;
    } e;
} MapEntry;

#endif /* defined(__Nanocraft__MapEntry__) */
