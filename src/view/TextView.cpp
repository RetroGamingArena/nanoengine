//
//  TextView.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 28/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "TextView.h"
#include "Engine.h"

TextView::TextView() : View("shaders/text_vertex.glsl", "shaders/text_fragment.glsl")
{
    buffer = genBuffer();
}

void TextView::loadShader()
{
    View::loadShader();
    attrib->program = program;
    attrib->position = glGetAttribLocation(program, "position");
    attrib->uv = glGetAttribLocation(program, "uv");
    attrib->matrix = glGetUniformLocation(program, "matrix");
    attrib->sampler = glGetUniformLocation(program, "sampler");
    attrib->extra1 = glGetUniformLocation(program, "is_sign");
}

void TextView::render(Model* model, Player* player)
{
    Model* g = model;
    State* s = &player->state;
    Engine* engine = Engine::getInstance();
    int face_count = 0;
    
    char text_buffer[1024];
    float ts = 12 * g->scale;
    float tx = ts / 2;
    float ty = g->height - ts;
    if (SHOW_INFO_TEXT) {
        int hour = engine->getAbstractWindow()->timeOfDay() * 24;
        char am_pm = hour < 12 ? 'a' : 'p';
        hour = hour % 12;
        hour = hour ? hour : 12;
        snprintf(
                 text_buffer, 1024,
                 "(%d, %d) (%.2f, %.2f, %.2f) [%d, %d, %d] %d%cm %dfps",
                 Chunk::chunked(s->x), Chunk::chunked(s->z), s->x, s->y, s->z,
                 g->player_count, (int)g->chunks->size(),
                 face_count * 2, hour, am_pm, engine->fps->fps);
        attrib->renderText(g, ALIGN_LEFT, tx, ty, ts, text_buffer);
        ty -= ts * 2;
    }
    if (SHOW_CHAT_TEXT) {
        for (int i = 0; i < MAX_MESSAGES; i++) {
            int index = (g->message_index + i) % MAX_MESSAGES;
            if (strlen(g->messages[index])) {
                attrib->renderText(g, ALIGN_LEFT, tx, ty, ts,
                                               g->messages[index]);
                ty -= ts * 2;
            }
        }
    }
    if (g->typing) {
        snprintf(text_buffer, 1024, "> %s", g->typing_buffer);
        attrib->renderText(g, ALIGN_LEFT, tx, ty, ts, text_buffer);
        ty -= ts * 2;
    }
    if (SHOW_PLAYER_NAMES) {
        /*if (player != me) {
            engine->text_attrib.renderText(g, ALIGN_CENTER,
                                           g->width / 2, ts, ts, player->name);
        }*/
        Player *other = g->playerCrosshair(player);
        if (other) {
            attrib->renderText(g, ALIGN_CENTER,
                                           g->width / 2, g->height / 2 - ts - 24, ts,
                                           other->name);
        }
    }

}

GLuint TextView::genBuffer()
{
    return 0;
}