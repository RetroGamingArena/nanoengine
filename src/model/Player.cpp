//
//  Player.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cmath>
#include <string>

#include "BufferUtils.h"
#include "item.h"
#include "Player.h"
#include "State.h"
#include "util.h"
#include "MathUtils.h"

Player::Player()
{
    id = 0;
    name[0] = '\0';
    buffer = 0;
}

float Player::crosshairDistance(Player *p2)
{
    State *s1 = &state;
    State *s2 = &p2->state;
    float d = distance(p2);
    float vx, vy, vz;
    MathUtils::getSightVector(s1->rx, s1->ry, &vx, &vy, &vz);
    vx *= d; vy *= d; vz *= d;
    float px, py, pz;
    px = s1->x + vx; py = s1->y + vy; pz = s1->z + vz;
    float x = s2->x - px;
    float y = s2->y - py;
    float z = s2->z - pz;
    return sqrtf(x * x + y * y + z * z);
}

float Player::distance(Player *p2)
{
    State *s1 = &state;
    State *s2 = &p2->state;
    float x = s2->x - s1->x;
    float y = s2->y - s1->y;
    float z = s2->z - s1->z;
    return sqrtf(x * x + y * y + z * z);
}

void Player::update(float x, float y, float z, float rx, float ry, int interpolate)
{
    if (interpolate)
    {
        State *s1 = &state1;
        State *s2 = &state2;
        memcpy(s1, s2, sizeof(State));
        s2->x = x; s2->y = y; s2->z = z; s2->rx = rx; s2->ry = ry;
        s2->t = glfwGetTime();
        if (s2->rx - s1->rx > PI)
        {
            s1->rx += 2 * PI;
        }
        if (s1->rx - s2->rx > PI)
        {
            s1->rx -= 2 * PI;
        }
    }
    else
    {
        State *s = &state;
        s->x = x; s->y = y; s->z = z; s->rx = rx; s->ry = ry;
        del_buffer(buffer);
        buffer = BufferUtils::genPlayerBuffer(s->x, s->y, s->z, s->rx, s->ry);
    }
}

float player_player_distance(Player *p1, Player *p2)
{
    State *s1 = &p1->state;
    State *s2 = &p2->state;
    float x = s2->x - s1->x;
    float y = s2->y - s1->y;
    float z = s2->z - s1->z;
    return sqrtf(x * x + y * y + z * z);
}

void Player::interpolate()
{
    State *s1 = &state1;
    State *s2 = &state2;
    float t1 = s2->t - s1->t;
    float t2 = glfwGetTime() - s2->t;
    t1 = MIN(t1, 1);
    t1 = MAX(t1, 0.1);
    float p = MIN(t2 / t1, 1);
    update(
                   s1->x + (s2->x - s1->x) * p,
                   s1->y + (s2->y - s1->y) * p,
                   s1->z + (s2->z - s1->z) * p,
                   s1->rx + (s2->rx - s1->rx) * p,
                   s1->ry + (s2->ry - s1->ry) * p,
                   0);
}

/*int Player::hitTestFace(Player *player, int *x, int *y, int *z, int *face)
{
    State *s = &player->state;
    int w = hitTest(0, s->x, s->y, s->z, s->rx, s->ry, x, y, z);
    if (is_obstacle(w)) {
        int hx, hy, hz;
        hitTest(1, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
        int dx = hx - *x;
        int dy = hy - *y;
        int dz = hz - *z;
        if (dx == -1 && dy == 0 && dz == 0) {
            *face = 0; return 1;
        }
        if (dx == 1 && dy == 0 && dz == 0) {
            *face = 1; return 1;
        }
        if (dx == 0 && dy == 0 && dz == -1) {
            *face = 2; return 1;
        }
        if (dx == 0 && dy == 0 && dz == 1) {
            *face = 3; return 1;
        }
        if (dx == 0 && dy == 1 && dz == 0) {
            int degrees = roundf(DEGREES(atan2f(s->x - hx, s->z - hz)));
            if (degrees < 0) {
                degrees += 360;
            }
            int top = ((degrees + 45) / 90) % 4;
            *face = 4 + top; return 1;
        }
    }
    return 0;
}

int _hitTest(Map *map, float max_distance, int previous, float x, float y, float z, float vx, float vy, float vz, int *hx, int *hy, int *hz)
{
    int m = 32;
    int px = 0;
    int py = 0;
    int pz = 0;
    for (int i = 0; i < max_distance * m; i++) {
        int nx = roundf(x);
        int ny = roundf(y);
        int nz = roundf(z);
        if (nx != px || ny != py || nz != pz) {
            int hw = Map::map_get(map, nx, ny, nz);
            if (hw > 0) {
                if (previous) {
                    *hx = px; *hy = py; *hz = pz;
                }
                else {
                    *hx = nx; *hy = ny; *hz = nz;
                }
                return hw;
            }
            px = nx; py = ny; pz = nz;
        }
        x += vx / m; y += vy / m; z += vz / m;
    }
    return 0;
}*/

/*int hitTest(int previous, float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz)
{
    int result = 0;
    float best = 0;
    int p = chunked(x);
    int q = chunked(z);
    float vx, vy, vz;
    MathUtils::getSightVector(rx, ry, &vx, &vy, &vz);
    for (int i = 0; i < g->chunk_count; i++) {
        Chunk *chunk = g->chunks + i;
        if (chunk_distance(chunk, p, q) > 1) {
            continue;
        }
        int hx, hy, hz;
        int hw = _hit_test(&chunk->map, 8, previous,
                           x, y, z, vx, vy, vz, &hx, &hy, &hz);
        if (hw > 0) {
            float d = sqrtf(
                            powf(hx - x, 2) + powf(hy - y, 2) + powf(hz - z, 2));
            if (best == 0 || d < best) {
                best = d;
                *bx = hx; *by = hy; *bz = hz;
                result = hw;
            }
        }
    }
    return result;
}*/