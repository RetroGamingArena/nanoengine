#include <glew.h>
#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "auth.h"
#include "BufferUtils.h"
#include "client.h"
#include "config.h"
#include "cube.h"
#include "item.h"
#include "map.h"
#include "matrix.h"
#include "../deps/noise/noise.h"
#include "sign.h"
#include "../deps/tinycthread/tinycthread.h"
#include "util.h"
#include "engine.h"

#include "TextView.h"

#define MAX_CHUNKS 8192
#define MAX_PLAYERS 128
#define WORKERS 4
#define MAX_TEXT_LENGTH 256
#define MAX_NAME_LENGTH 32
#define MAX_PATH_LENGTH 256
#define MAX_ADDR_LENGTH 256

#define MODE_OFFLINE 0
#define MODE_ONLINE 1

#define WORKER_IDLE 0
#define WORKER_BUSY 1
#define WORKER_DONE 2

Engine* engine = Engine::getInstance();

int main(int argc, char **argv)
{
    engine->run();
    
    return 0;
}
