//
//  Model.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 18/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include <cmath>
#include <cstdlib>
#include <string>

#include "AbstractWindow.h"
#include "Engine.h"
#include "Model.h"
#include "util.h"
#include "item.h"
#include "client.h"
#include "cube.h"
#include "matrix.h"
#include "noise.h"
#include "MathUtils.h"

Model::Model()
{
    create_radius = CREATE_CHUNK_RADIUS;
    render_radius = RENDER_CHUNK_RADIUS;
    delete_radius = DELETE_CHUNK_RADIUS;
    sign_radius = RENDER_SIGN_RADIUS;
    
    reset();
}

int Model::highestBlock(float x, float z)
{
    int result = -1;
    int nx = roundf(x);
    int nz = roundf(z);
    int p = Chunk::chunked(x);
    int q = Chunk::chunked(z);
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        Map *map = &chunk->map;
        MAP_FOR_EACH(map, ex, ey, ez, ew) {
            
            int ex2  = Map::getX(i)+p*(CHUNK_SIZE+1)-1;
            int ey2  = Map::getY(i);
            int ez2  = Map::getZ(i)-q*(CHUNK_SIZE+1)-1;
            
            /*int ex2  = ex+p*(CHUNK_SIZE+1)-1;
            int ey2  = ey;
            int ez2  = ez-q*(CHUNK_SIZE+1)-1;*/

            if (is_obstacle(ew) && ex2 == nx && ez2 == nz)
            {
                result = MAX(result, ey2);
            }
            
            /*if (is_obstacle(ew) && ex == nx && ez == nz) {
                result = MAX(result, ey);
            }*/
        } END_MAP_FOR_EACH;
    }
    return result;
}

int Model::chunkVisible(float planes[6][4], int p, int q, int miny, int maxy)
{
    int x = p * CHUNK_SIZE - 1;
    int z = q * CHUNK_SIZE - 1;
    int d = CHUNK_SIZE + 1;
    float points[8][3] = {
        {(float)x + 0, (float)miny, (float)z + 0},
        {(float)x + d, (float)miny, (float)z + 0},
        {(float)x + 0, (float)miny, (float)z + d},
        {(float)x + d, (float)miny, (float)z + d},
        {(float)x + 0, (float)maxy, (float)z + 0},
        {(float)x + d, (float)maxy, (float)z + 0},
        {(float)x + 0, (float)maxy, (float)z + d},
        {(float)x + d, (float)maxy, (float)z + d}
    };
    int n = ortho ? 4 : 6;
    for (int i = 0; i < n; i++) {
        int in = 0;
        int out = 0;
        for (int j = 0; j < 8; j++) {
            float d =
            planes[i][0] * points[j][0] +
            planes[i][1] * points[j][1] +
            planes[i][2] * points[j][2] +
            planes[i][3];
            if (d < 0) {
                out++;
            }
            else {
                in++;
            }
            if (in && out) {
                break;
            }
        }
        if (in == 0) {
            return 0;
        }
    }
    return 1;
}

Player *Model::playerCrosshair(Player *player)
{
    Player *result = 0;
    float threshold = RADIANS(5);
    float best = 0;
    for (int i = 0; i < player_count; i++) {
        Player *other = players + i;
        if (other == player) {
            continue;
        }
        float p = player->crosshairDistance(other);
        float d = player->distance(other);
        if (d < 96 && p / d < threshold) {
            if (best == 0 || d < best) {
                best = d;
                result = other;
            }
        }
    }
    return result;
}

Player *Model::findPlayer(int id)
{
    for (int i = 0; i < player_count; i++) {
        Player *player = players + i;
        if (player->id == id) {
            return player;
        }
    }
    return 0;
}

void Model::deleteAllPlayers()
{
    for (int i = 0; i < player_count; i++) {
        Player *player = players + i;
        del_buffer(player->buffer);
    }
    player_count = 0;
}

void Model::deletePlayer(int id)
{
    Player *player = findPlayer(id);
    if (!player) {
        return;
    }
    int count = player_count;
    del_buffer(player->buffer);
    Player *other = players + (--count);
    memcpy(player, other, sizeof(Player));
    player_count = count;
}

void Model:: reset()
{
    //chunks->clear();
    memset(players, 0, sizeof(Player) * MAX_PLAYERS);
    player_count = 0;
    observe1 = 0;
    observe2 = 0;
    flying = 0;
    item_index = 0;
    memset(typing_buffer, 0, sizeof(char) * MAX_TEXT_LENGTH);
    typing = 0;
    memset(messages, 0, sizeof(char) * MAX_MESSAGES * MAX_TEXT_LENGTH);
    message_index = 0;
    time_changed = 1;
    
    player_count = 1;
}

int Model::hitTestFace(Player *player, int *x, int *y, int *z, int *face)
{
    State *s = &player->state;
    int w = this->hitTest(0, s->x, s->y, s->z, s->rx, s->ry, x, y, z);
    if (is_obstacle(w)) {
        int hx, hy, hz;
        this->hitTest(1, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
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

int Model::hitTest(int previous, float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz)
{
    int result = 0;
    float best = 0;
    int p = chunked(x);
    int q = chunked(z);
    float vx, vy, vz;
    MathUtils::getSightVector(rx, ry, &vx, &vy, &vz);
    for (int i = 0; i < chunks->size(); i++) {
        Chunk *chunk = (*chunks)[i];
        if (chunk->distance(p, q) > 1) {
            continue;
        }
        int hx, hy, hz;
        int hw = (&chunk->map)->_hitTest(8, previous, x, y, z, vx, vy, vz, &hx, &hy, &hz);
        if (hw > 0) {
            float d = sqrtf(powf(hx - x, 2) + powf(hy - y, 2) + powf(hz - z, 2));
            if (best == 0 || d < best) {
                best = d;
                *bx = hx; *by = hy; *bz = hz;
                result = hw;
            }
        }
    }
    return result;
}

int Model::chunked(float x)
{
    return floorf(roundf(x) / CHUNK_SIZE);
}

void Model::setSign(int x, int y, int z, int face, const char *text)
{
    int p = chunked(x);
    int q = chunked(z);
    _setSign(p, q, x, y, z, face, text, 1);
    client_sign(x, y, z, face, text);
}

void Model::_setSign(int p, int q, int x, int y, int z, int face, const char *text, int dirty)
{
    if (strlen(text) == 0)
    {
        unsetSignFace(x, y, z, face);
        return;
    }
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        SignList *signs = &chunk->signs;
        sign_list_add(signs, x, y, z, face, text);
        if (dirty) {
            chunk->dirty = 1;
        }
    }
    //db_insert_sign(p, q, x, y, z, face, text);
}

/*Chunk *Model::findChunk(int p, int q)
{
    for (int i = 0; i < chunks->size(); i++) {
        Chunk *chunk = (*chunks)[i];
        if (chunk->p == p && chunk->q == q) {
            return chunk;
        }
    }
    return 0;
}*/

void Model::unsetSignFace(int x, int y, int z, int face)
{
    int p = Model::chunked(x);
    int q = Model::chunked(z);
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        SignList *signs = &chunk->signs;
        if (sign_list_remove(signs, x, y, z, face)) {
            chunk->dirty = 1;
            //db_delete_sign(x, y, z, face);
        }
    }
    else {
        //db_delete_sign(x, y, z, face);
    }
}

void Model::parseCommand(const char *buffer, int forward)
{
    char username[128] = {0};
    char token[128] = {0};
    char server_addr[MAX_ADDR_LENGTH];
    int server_port = DEFAULT_PORT;
    char filename[MAX_PATH_LENGTH];
    int radius, count, xc, yc, zc;
    if (sscanf(buffer, "/identity %128s %128s", username, token) == 2) {
        //db_auth_set(username, token);
        addMessage("Successfully imported identity token!");
    }
    else if (strcmp(buffer, "/logout") == 0) {
        //db_auth_select_none();
    }
    else if (sscanf(buffer, "/login %128s", username) == 1) {
        /*if (db_auth_select(username)) {
         login();
         }
         else {
         add_message("Unknown username.");
         }*/
    }
    else if (sscanf(buffer,
                    "/online %128s %d", server_addr, &server_port) >= 1)
    {
        mode_changed = 1;
        mode = MODE_ONLINE;
        strncpy(server_addr, server_addr, MAX_ADDR_LENGTH);
        server_port = server_port;
        //snprintf(db_path, MAX_PATH_LENGTH,
        //    "cache.%s.%d.db", server_addr, server_port);
    }
    else if (sscanf(buffer, "/offline %128s", filename) == 1) {
        mode_changed = 1;
        mode = MODE_OFFLINE;
        //snprintf(db_path, MAX_PATH_LENGTH, "%s.db", filename);
    }
    else if (strcmp(buffer, "/offline") == 0) {
        mode_changed = 1;
        mode = MODE_OFFLINE;
        //snprintf(db_path, MAX_PATH_LENGTH, "%s", DB_PATH);
    }
    else if (sscanf(buffer, "/view %d", &radius) == 1) {
        if (radius >= 1 && radius <= 24) {
            create_radius = radius;
            render_radius = radius;
            delete_radius = radius + 4;
        }
        else {
            addMessage("Viewing distance must be between 1 and 24.");
        }
    }
    else if (strcmp(buffer, "/copy") == 0) {
        copy();
    }
    else if (strcmp(buffer, "/paste") == 0) {
        paste();
    }
    else if (strcmp(buffer, "/tree") == 0) {
        tree(&block0);
    }
    else if (sscanf(buffer, "/array %d %d %d", &xc, &yc, &zc) == 3) {
        array(&block1, &block0, xc, yc, zc);
    }
    else if (sscanf(buffer, "/array %d", &count) == 1) {
        array(&block1, &block0, count, count, count);
    }
    else if (strcmp(buffer, "/fcube") == 0) {
        cube(&block0, &block1, 1);
    }
    else if (strcmp(buffer, "/cube") == 0) {
        cube(&block0, &block1, 0);
    }
    else if (sscanf(buffer, "/fsphere %d", &radius) == 1) {
        sphere(&block0, radius, 1, 0, 0, 0);
    }
    else if (sscanf(buffer, "/sphere %d", &radius) == 1) {
        sphere(&block0, radius, 0, 0, 0, 0);
    }
    else if (sscanf(buffer, "/fcirclex %d", &radius) == 1) {
        sphere(&block0, radius, 1, 1, 0, 0);
    }
    else if (sscanf(buffer, "/circlex %d", &radius) == 1) {
        sphere(&block0, radius, 0, 1, 0, 0);
    }
    else if (sscanf(buffer, "/fcircley %d", &radius) == 1) {
        sphere(&block0, radius, 1, 0, 1, 0);
    }
    else if (sscanf(buffer, "/circley %d", &radius) == 1) {
        sphere(&block0, radius, 0, 0, 1, 0);
    }
    else if (sscanf(buffer, "/fcirclez %d", &radius) == 1) {
        sphere(&block0, radius, 1, 0, 0, 1);
    }
    else if (sscanf(buffer, "/circlez %d", &radius) == 1) {
        sphere(&block0, radius, 0, 0, 0, 1);
    }
    else if (sscanf(buffer, "/fcylinder %d", &radius) == 1) {
        cylinder(&block0, &block1, radius, 1);
    }
    else if (sscanf(buffer, "/cylinder %d", &radius) == 1) {
        cylinder(&block0, &block1, radius, 0);
    }
    else if (forward) {
        client_talk(buffer);
    }
}

void Model::toggleLight(int x, int y, int z)
{
    int p = chunked(x);
    int q = chunked(z);
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        Map *map = &chunk->lights;
        int w = Map::map_get(map, x, y, z) ? 0 : 15;
        map->set(x, y, z, 0, 0, 0, w, true);
        //db_insert_light(p, q, x, y, z, w);
        client_light(x, y, z, w);
        chunks->dirtyChunk(chunk);
    }
}

void Model::setBlock(int x, int y, int z, int w)
{
    int p = chunked(x);
    int q = chunked(z);
    _setBlock(p, q, x, y, z, w, 1);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            if (dx == 0 && dz == 0) {
                continue;
            }
            if (dx && chunked(x + dx) == p) {
                continue;
            }
            if (dz && chunked(z + dz) == q) {
                continue;
            }
            _setBlock(p + dx, q + dz, x, y, z, -w, 1);
        }
    }
    client_block(x, y, z, w);
}

void Model::_setBlock(int p, int q, int x, int y, int z, int w, int dirty)
{
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        Map *map = &chunk->map;
        if (map->set(x, y, z, 0, 0, 0, w, 1)) {
            if (dirty) {
                chunks->dirtyChunk(chunk);
            }
            //db_insert_block(p, q, x, y, z, w);
        }
    }
    else {
        //db_insert_block(p, q, x, y, z, w);
    }
    if (w == 0 && chunked(x) == p && chunked(z) == q) {
        unsetSign(x, y, z);
        setLight(p, q, x, y, z, 0);
    }
}

void Model::recordBlock(int x, int y, int z, int w)
{
    memcpy(&block1, &block0, sizeof(Block));
    block0.x = x;
    block0.y = y;
    block0.z = z;
    block0.w = w;
}

int Model::getBlock(int x, int y, int z)
{
    int p = chunked(x);
    int q = chunked(z);
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        Map *map = &chunk->map;
        return Map::map_get(map, x, y, z);
    }
    return 0;
}

int Model::playerIntersectsBlock(int height, float x, float y, float z, int hx, int hy, int hz)
{
    int nx = roundf(x);
    int ny = roundf(y);
    int nz = roundf(z);
    for (int i = 0; i < height; i++) {
        if (nx == hx && ny - i == hy && nz == hz) {
            return 1;
        }
    }
    return 0;
}

void Model::addMessage(const char *text)
{
    printf("%s\n", text);
    snprintf(
             messages[message_index], MAX_TEXT_LENGTH, "%s", text);
    message_index = (message_index + 1) % MAX_MESSAGES;
}

void Model::copy() {
    memcpy(&copy0, &block0, sizeof(Block));
    memcpy(&copy1, &block1, sizeof(Block));
}

void Model::paste() {
    Block *c1 = &copy1;
    Block *c2 = &copy0;
    Block *p1 = &block1;
    Block *p2 = &block0;
    int scx = SIGN(c2->x - c1->x);
    int scz = SIGN(c2->z - c1->z);
    int spx = SIGN(p2->x - p1->x);
    int spz = SIGN(p2->z - p1->z);
    int oy = p1->y - c1->y;
    int dx = ABS(c2->x - c1->x);
    int dz = ABS(c2->z - c1->z);
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x <= dx; x++) {
            for (int z = 0; z <= dz; z++) {
                int w = getBlock(c1->x + x * scx, y, c1->z + z * scz);
                builderBlock(p1->x + x * spx, y + oy, p1->z + z * spz, w);
            }
        }
    }
}

void Model::tree(Block *block)
{
    int bx = block->x;
    int by = block->y;
    int bz = block->z;
    for (int y = by + 3; y < by + 8; y++) {
        for (int dx = -3; dx <= 3; dx++) {
            for (int dz = -3; dz <= 3; dz++) {
                int dy = y - (by + 4);
                int d = (dx * dx) + (dy * dy) + (dz * dz);
                if (d < 11) {
                    builderBlock(bx + dx, y, bz + dz, 15);
                }
            }
        }
    }
    for (int y = by; y < by + 7; y++) {
        builderBlock(bx, y, bz, 5);
    }
}

void Model::array(Block *b1, Block *b2, int xc, int yc, int zc) {
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int dx = b2->x - b1->x;
    int dy = b2->y - b1->y;
    int dz = b2->z - b1->z;
    xc = dx ? xc : 1;
    yc = dy ? yc : 1;
    zc = dz ? zc : 1;
    for (int i = 0; i < xc; i++) {
        int x = b1->x + dx * i;
        for (int j = 0; j < yc; j++) {
            int y = b1->y + dy * j;
            for (int k = 0; k < zc; k++) {
                int z = b1->z + dz * k;
                builderBlock(x, y, z, w);
            }
        }
    }
}

void Model::cube(Block *b1, Block *b2, int fill)
{
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int x1 = MIN(b1->x, b2->x);
    int y1 = MIN(b1->y, b2->y);
    int z1 = MIN(b1->z, b2->z);
    int x2 = MAX(b1->x, b2->x);
    int y2 = MAX(b1->y, b2->y);
    int z2 = MAX(b1->z, b2->z);
    int a = (x1 == x2) + (y1 == y2) + (z1 == z2);
    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            for (int z = z1; z <= z2; z++) {
                if (!fill) {
                    int n = 0;
                    n += x == x1 || x == x2;
                    n += y == y1 || y == y2;
                    n += z == z1 || z == z2;
                    if (n <= a) {
                        continue;
                    }
                }
                builderBlock(x, y, z, w);
            }
        }
    }
}

void Model::sphere(Block *center, int radius, int fill, int fx, int fy, int fz) {
    static const float offsets[8][3] = {
        {-0.5, -0.5, -0.5},
        {-0.5, -0.5, 0.5},
        {-0.5, 0.5, -0.5},
        {-0.5, 0.5, 0.5},
        {0.5, -0.5, -0.5},
        {0.5, -0.5, 0.5},
        {0.5, 0.5, -0.5},
        {0.5, 0.5, 0.5}
    };
    int cx = center->x;
    int cy = center->y;
    int cz = center->z;
    int w = center->w;
    for (int x = cx - radius; x <= cx + radius; x++) {
        if (fx && x != cx) {
            continue;
        }
        for (int y = cy - radius; y <= cy + radius; y++) {
            if (fy && y != cy) {
                continue;
            }
            for (int z = cz - radius; z <= cz + radius; z++) {
                if (fz && z != cz) {
                    continue;
                }
                int inside = 0;
                int outside = fill;
                for (int i = 0; i < 8; i++) {
                    float dx = x + offsets[i][0] - cx;
                    float dy = y + offsets[i][1] - cy;
                    float dz = z + offsets[i][2] - cz;
                    float d = sqrtf(dx * dx + dy * dy + dz * dz);
                    if (d < radius) {
                        inside = 1;
                    }
                    else {
                        outside = 1;
                    }
                }
                if (inside && outside) {
                    builderBlock(x, y, z, w);
                }
            }
        }
    }
}

void Model::cylinder(Block *b1, Block *b2, int radius, int fill)
{
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int x1 = MIN(b1->x, b2->x);
    int y1 = MIN(b1->y, b2->y);
    int z1 = MIN(b1->z, b2->z);
    int x2 = MAX(b1->x, b2->x);
    int y2 = MAX(b1->y, b2->y);
    int z2 = MAX(b1->z, b2->z);
    int fx = x1 != x2;
    int fy = y1 != y2;
    int fz = z1 != z2;
    if (fx + fy + fz != 1) {
        return;
    }
    Block block = {x1, y1, z1, w};
    if (fx) {
        for (int x = x1; x <= x2; x++) {
            block.x = x;
            sphere(&block, radius, fill, 1, 0, 0);
        }
    }
    if (fy) {
        for (int y = y1; y <= y2; y++) {
            block.y = y;
            sphere(&block, radius, fill, 0, 1, 0);
        }
    }
    if (fz) {
        for (int z = z1; z <= z2; z++) {
            block.z = z;
            sphere(&block, radius, fill, 0, 0, 1);
        }
    }
}

/*void Model::dirtyChunk(Chunk *chunk)
{
    chunk->dirty = 1;
    if (hasLights(chunk)) {
        for (int dp = -1; dp <= 1; dp++) {
            for (int dq = -1; dq <= 1; dq++) {
                Chunk *other = findChunk(chunk->p + dp, chunk->q + dq);
                if (other) {
                    other->dirty = 1;
                }
            }
        }
    }
}*/

void Model::unsetSign(int x, int y, int z)
{
    int p = chunked(x);
    int q = chunked(z);
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        SignList *signs = &chunk->signs;
        if (sign_list_remove_all(signs, x, y, z)) {
            chunk->dirty = 1;
            //db_delete_signs(x, y, z);
        }
    }
    else {
        //db_delete_signs(x, y, z);
    }
}

void Model::setLight(int p, int q, int x, int y, int z, int w)
{
    Chunk *chunk = chunks->findChunk(p, q);
    if (chunk) {
        Map *map = &chunk->lights;
        if (map->set(x, y, z, 0, 0, 0, w, true)) {
            chunks->dirtyChunk(chunk);
            //db_insert_light(p, q, x, y, z, w);
        }
    }
    else {
        //db_insert_light(p, q, x, y, z, w);
    }
}

void Model::builderBlock(int x, int y, int z, int w) {
    if (y <= 0 || y >= 256) {
        return;
    }
    if (is_destructable(getBlock(x, y, z))) {
        setBlock(x, y, z, 0);
    }
    if (w) {
        setBlock(x, y, z, w);
    }
}

/*int Model::hasLights(Chunk *chunk)
{
    if (!SHOW_LIGHTS) {
        return 0;
    }
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            Chunk *other = chunk;
            if (dp || dq) {
                other = findChunk(chunk->p + dp, chunk->q + dq);
            }
            if (!other) {
                continue;
            }
            Map *map = &other->lights;
            if (map->size) {
                return 1;
            }
        }
    }
    return 0;
}*/

int Model::collide(int height, State *s)
{
    int result = 0;
    int p = Chunk::chunked(s->x + s->dx);
    int q = Chunk::chunked(s->z + s->dz);
    Chunk *chunk = chunks->findChunk(p, q);
    if (!chunk)
    {
        return result;
    }

    Map *map = &chunk->map;
    
    if (!is_obstacle(Map::map_get(map, s->x-1, s->y-height, s->z-1)))
    {
        s->y += -0.015625;
    }
    else
    {
        s->dy = 0;
        s->dz = 0;
        result = true;
    }
    if (!is_obstacle(Map::map_get(map, s->x + s->dx-1, s->y-1, s->z + s->dz-1)))
    {
        s->x = s->x + s->dx;
        s->z = s->z + s->dz;
    }
    else
    {
        //s->x = s->x - s->dx;
        //s->z = s->z - s->dz;
        s->dx = 0;
        s->dz = 0;
    }
    
    return result;
}

int Model::collide(int height, float *x, float *y, float *z)
{
    int result = 0;
    int p = Chunk::chunked(*x);
    int q = Chunk::chunked(*z);
    Chunk *chunk = chunks->findChunk(p, q);
    if (!chunk) {
        return result;
    }
    Map *map = &chunk->map;
    int nx = roundf(*x); // - (chunk->p*CHUNK_SIZE);
    int ny = roundf(*y);
    int nz = roundf(*z);
    
    /*
     int mapX = Map::getX(i);
     int mapY = Map::getY(i);
     int mapZ = Map::getZ(i);
     
     int testX = mapX+p*CHUNK_SIZE-1;
     int testY = mapY%32;
     int testZ = mapZ+q*CHUNK_SIZE-1;
     */
    
    nx = nx - (chunk->p*CHUNK_SIZE);
    nz = nz - (chunk->q*CHUNK_SIZE);
    
    float px = *x - nx;
    float py = *y - ny;
    float pz = *z - nz;
    float pad = 0.25;
    for (int dy = 0; dy < height; dy++)
    {
        if (px < -pad && is_obstacle(Map::map_get(map, nx, ny - dy, nz)))
        {  //nx - 1, ny - dy, nz))) {
            *x = roundf(*x);//nx - pad;
        }
        /*if (px < -pad && is_obstacle(Map::map_get(map, nx-1, ny - dy, nz))){  //nx - 1, ny - dy, nz))) {
            *x = roundf(*x);//nx - pad;
        }
        if (px > pad && is_obstacle(Map::map_get(map, nx+1, ny - dy, nz))) {
            *x = nx + pad;
        }*/
        if (py < -pad && is_obstacle(Map::map_get(map, nx, ny - dy - 1, nz))) {
            *y = ny - pad;
            result = 1;
        }
        if (py > pad && is_obstacle(Map::map_get(map, nx, ny - dy + 1, nz))) {
            *y = ny + pad;
            result = 1;
        }
        if (pz < -pad && is_obstacle(Map::map_get(map, nx, ny - dy, nz - 1))) {
            *z = /*roundf(*z)+pad*2;*/nz - pad;
        }
        if (pz > pad && is_obstacle(Map::map_get(map, nx, ny - dy, nz + 1))) {
            *z = /*roundf(*z);*/nz + pad;
        }
    }
    return result;
}

void Model::requestChunk(int p, int q)
{
    int key = 0;
    client_chunk(p, q, key);
}

/*void Model::initChunk(Chunk *chunk, int p, int q)
{
    chunkchun p;
    chunk->q = q;
    chunk->faces = 0;
    chunk->sign_faces = 0;
    chunk->buffer = 0;
    chunk->sign_buffer = 0;
    dirtyChunk(chunk);
    Map *block_map = &chunk->map;
    Map *light_map = &chunk->lights;
    int dx = p * CHUNK_SIZE - 1;
    int dy = 0;
    int dz = q * CHUNK_SIZE - 1;
    Map::map_alloc(block_map, dx, dy, dz, 0x7fff);
    Map::map_alloc(light_map, dx, dy, dz, 0xf);
}*/

/*void Model::createChunk(Chunk *chunk, int p, int q)
{
    initChunk(chunk, p, q);
    
    WorkerItem _item;
    WorkerItem *item = &_item;
    item->p = chunk->p;
    item->q = chunk->q;
    item->block_maps[1][1] = &chunk->map;
    item->light_maps[1][1] = &chunk->lights;
    item->loadChunk();
    
    requestChunk(p, q);
}*/

void Model::deleteChunks()
{
    int count = (int)chunks->size();
    State *s1 = &players->state;
    State *s2 = &(players + observe1)->state;
    State *s3 = &(players + observe2)->state;
    State *states[3] = {s1, s2, s3};
    for (int i = 0; i < count; i++) {
        Chunk *chunk = (*chunks)[i];
        int _delete = 1;
        for (int j = 0; j < 3; j++) {
            State *s = states[j];
            int p = Chunk::chunked(s->x);
            int q = Chunk::chunked(s->z);
            if (chunk->distance(p, q) < delete_radius) {
                _delete = 0;
                break;
            }
        }
        if (_delete) {
            Map::map_free(&chunk->map);
            Map::map_free(&chunk->lights);
            sign_list_free(&chunk->signs);
            del_buffer(chunk->buffer);
            del_buffer(chunk->sign_buffer);
            Chunk *other = (*chunks)[(--count)];
            memcpy(chunk, other, sizeof(Chunk));
        }
    }
    chunks->resize(count);
}

void Model::deleteAllChunks()
{
    for (int i = 0; i < chunks->size(); i++)
    {
        Chunk *chunk = (*chunks)[i];
        Map::map_free(&chunk->map);
        Map::map_free(&chunk->lights);
        sign_list_free(&chunk->signs);
        del_buffer(chunk->buffer);
        del_buffer(chunk->sign_buffer);
    }
    chunks->clear();
}

void Model::forceChunks(Player *player)
{
    State *s = &player->state;
    int p = Chunk::chunked(s->x);
    int q = Chunk::chunked(s->z);
    chunks = new World(p, q);
    int r = 1;
    for (int dp = -r; dp <= r; dp++)
    {
        for (int dq = -r; dq <= r; dq++)
        {
            int a = p + dp;
            int b = q + dq;
            Chunk *chunk = chunks->findChunk(a, b);
            if (chunk) {
                if (chunk->dirty) {
                    chunks->genChunkBuffer(chunk);
                }
            }
            else if (chunks->size() < MAX_CHUNKS) {
                chunks->push_back(new Chunk(a, b));
                chunk = (*chunks)[chunks->size()-1];
                chunks->createChunk(chunk, a, b);
                chunks->genChunkBuffer(chunk);
            }
        }
    }
}

void Model::initWorld(Player *player)
{
    State *s = &player->state;
    int p = Chunk::chunked(s->x);
    int q = Chunk::chunked(s->z);
    chunks = new World(p,q);
}

void Model::onLeftClick()
{
    State *s = &players->state;
    int hx, hy, hz;
    int hw = hitTest(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    if (hy > 0 && hy < 256 && is_destructable(hw)) {
        setBlock(hx, hy, hz, 0);
        recordBlock(hx, hy, hz, 0);
        if (is_plant(getBlock(hx, hy + 1, hz))) {
            setBlock(hx, hy + 1, hz, 0);
        }
    }
}

void Model::onRightClick()
{
    State *s = &players->state;
    int hx, hy, hz;
    int hw = hitTest(1, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    if (hy > 0 && hy < 256 && is_obstacle(hw)) {
        if (!MathUtils::playerIntersectsBlock(2, s->x, s->y, s->z, hx, hy, hz)) {
            setBlock(hx, hy, hz, items[item_index]);
            recordBlock(hx, hy, hz, items[item_index]);
        }
    }
}

void Model::onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    int control = mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER);
    int exclusive =
    glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    if (action == GLFW_RELEASE) {
        return;
    }
    if (key == GLFW_KEY_BACKSPACE) {
        if (typing) {
            int n = (int)strlen(typing_buffer);
            if (n > 0) {
                typing_buffer[n - 1] = '\0';
            }
        }
    }
    if (action != GLFW_PRESS) {
        return;
    }
    if (key == GLFW_KEY_ESCAPE) {
        if (typing) {
            typing = 0;
        }
        else if (exclusive) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    if (key == GLFW_KEY_ENTER) {
        if (typing) {
            if (mods & GLFW_MOD_SHIFT) {
                int n = (int)strlen(typing_buffer);
                if (n < MAX_TEXT_LENGTH - 1) {
                    typing_buffer[n] = '\r';
                    typing_buffer[n + 1] = '\0';
                }
            }
            else {
                typing = 0;
                if (typing_buffer[0] == CRAFT_KEY_SIGN) {
                    Player *player = players;
                    int x, y, z, face;
                    if (hitTestFace(player, &x, &y, &z, &face)) {
                        setSign(x, y, z, face, typing_buffer + 1);
                    }
                }
                else if (typing_buffer[0] == '/') {
                    parseCommand(typing_buffer, 1);
                }
                else {
                    client_talk(typing_buffer);
                }
            }
        }
        else {
            if (control) {
                onRightClick();
            }
            else {
                onLeftClick();
            }
        }
    }
    if (control && key == 'V') {
        const char *buffer = glfwGetClipboardString(window);
        if (typing) {
            suppress_char = 1;
            strncat(typing_buffer, buffer,
                    MAX_TEXT_LENGTH - strlen(typing_buffer) - 1);
        }
        else {
            parseCommand(buffer, 0);
        }
    }
    if (!typing) {
        if (key == CRAFT_KEY_FLY) {
            flying = !flying;
        }
        if (key >= '1' && key <= '9') {
            item_index = key - '1';
        }
        if (key == '0') {
            item_index = 9;
        }
        if (key == CRAFT_KEY_ITEM_NEXT) {
            item_index = (item_index + 1) % item_count;
        }
        if (key == CRAFT_KEY_ITEM_PREV) {
            item_index--;
            if (item_index < 0) {
                item_index = item_count - 1;
            }
        }
        if (key == CRAFT_KEY_OBSERVE) {
            observe1 = (observe1 + 1) % player_count;
        }
        if (key == CRAFT_KEY_OBSERVE_INSET) {
            observe2 = (observe2 + 1) % player_count;
        }
    }
}

void Model::onMouseButton(GLFWwindow *window, int button, int action, int mods)
{
    int control = mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER);
    int exclusive =
    glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    if (action != GLFW_PRESS) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (exclusive) {
            if (control) {
                onRightClick();
            }
            else {
                onLeftClick();
            }
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (exclusive) {
            if (control) {
                onLight();
            }
            else {
                onRightClick();
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (exclusive) {
            onMiddleClick();
        }
    }
}

void Model::onLight()
{
    State *s = &players->state;
    int hx, hy, hz;
    int hw = hitTest(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    if (hy > 0 && hy < 256 && is_destructable(hw)) {
        toggleLight(hx, hy, hz);
    }
}

void Model::onMiddleClick()
{
    State *s = &players->state;
    int hx, hy, hz;
    int hw = hitTest(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
    for (int i = 0; i < item_count; i++) {
        if (items[i] == hw) {
            item_index = i;
            break;
        }
    }
}

void Model::parseBuffer(char *buffer)
{
    Player *me = players;
    State *s = &players->state;
    char *key;
    char *line = tokenize(buffer, "\n", &key);
    while (line) {
        int pid;
        float ux, uy, uz, urx, ury;
        if (sscanf(line, "U,%d,%f,%f,%f,%f,%f",
                   &pid, &ux, &uy, &uz, &urx, &ury) == 6)
        {
            me->id = pid;
            s->x = ux; s->y = uy; s->z = uz; s->rx = urx; s->ry = ury;
            forceChunks(me);
            if (uy == 0) {
                s->y = highestBlock(s->x, s->z) + 2;
            }
        }
        int bp, bq, bx, by, bz, bw;
        if (sscanf(line, "B,%d,%d,%d,%d,%d,%d",
                   &bp, &bq, &bx, &by, &bz, &bw) == 6)
        {
            _setBlock(bp, bq, bx, by, bz, bw, 0);
            if (MathUtils::playerIntersectsBlock(2, s->x, s->y, s->z, bx, by, bz)) {
                s->y = highestBlock(s->x, s->z) + 2;
            }
        }
        if (sscanf(line, "L,%d,%d,%d,%d,%d,%d",
                   &bp, &bq, &bx, &by, &bz, &bw) == 6)
        {
            setLight(bp, bq, bx, by, bz, bw);
        }
        float px, py, pz, prx, pry;
        if (sscanf(line, "P,%d,%f,%f,%f,%f,%f",
                   &pid, &px, &py, &pz, &prx, &pry) == 6)
        {
            Player *player = findPlayer(pid);
            if (!player && player_count < MAX_PLAYERS) {
                player = players + player_count;
                player_count++;
                player->id = pid;
                player->buffer = 0;
                snprintf(player->name, MAX_NAME_LENGTH, "player%d", pid);
                player->update(px, py, pz, prx, pry, 1); // twice
            }
            if (player) {
                player->update(px, py, pz, prx, pry, 1);
            }
        }
        if (sscanf(line, "D,%d", &pid) == 1) {
            deletePlayer(pid);
        }
        int kp, kq, kk;
        if (sscanf(line, "K,%d,%d,%d", &kp, &kq, &kk) == 3) {
            //db_set_key(kp, kq, kk);
        }
        if (sscanf(line, "R,%d,%d", &kp, &kq) == 2) {
            Chunk *chunk = chunks->findChunk(kp, kq);
            if (chunk) {
                chunks->dirtyChunk(chunk);
            }
        }
        double elapsed;
        int day_length;
        if (sscanf(line, "E,%lf,%d", &elapsed, &day_length) == 2) {
            AbstractWindow* window = Engine::getInstance()->getAbstractWindow();
            glfwSetTime(fmod(elapsed, day_length));
            window->day_length = day_length;
            time_changed = 1;
        }
        if (line[0] == 'T' && line[1] == ',') {
            char *text = line + 2;
            addMessage(text);
        }
        char format[64];
        snprintf(
                 format, sizeof(format), "N,%%d,%%%ds", MAX_NAME_LENGTH - 1);
        char name[MAX_NAME_LENGTH];
        if (sscanf(line, format, &pid, name) == 2) {
            Player *player = findPlayer(pid);
            if (player) {
                strncpy(player->name, name, MAX_NAME_LENGTH);
            }
        }
        snprintf(
                 format, sizeof(format),
                 "S,%%d,%%d,%%d,%%d,%%d,%%d,%%%d[^\n]", MAX_SIGN_LENGTH - 1);
        int face;
        char text[MAX_SIGN_LENGTH] = {0};
        if (sscanf(line, format,
                   &bp, &bq, &bx, &by, &bz, &face, text) >= 6)
        {
            _setSign(bp, bq, bx, by, bz, face, text, 0);
        }
        line = tokenize(NULL, "\n", &key);
    }
}

void Model::handleMouseInput()
{
    int exclusive =
    glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    static double px = 0;
    static double py = 0;
    State *s = &players->state;
    if (exclusive && (px || py)) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        float m = 0.0025;
        s->rx += (mx - px) * m;
        if (INVERT_MOUSE) {
            s->ry += (my - py) * m;
        }
        else {
            s->ry -= (my - py) * m;
        }
        if (s->rx < 0) {
            s->rx += RADIANS(360);
        }
        if (s->rx >= RADIANS(360)){
            s->rx -= RADIANS(360);
        }
        s->ry = MAX(s->ry, -RADIANS(90));
        s->ry = MIN(s->ry, RADIANS(90));
        px = mx;
        py = my;
    }
    else {
        glfwGetCursorPos(window, &px, &py);
    }
}

void Model::handleMovement(double dt)
{
    static int jump = 0;
    //static float dy = 0;
    State *s = &players->state;
    int sz = 0;
    int sx = 0;
    if (!typing) {
        float m = dt * 1.0;
        ortho = glfwGetKey(window, CRAFT_KEY_ORTHO) ? 64 : 0;
        fov = glfwGetKey(window, CRAFT_KEY_ZOOM) ? 15 : 65;
        if (glfwGetKey(window, CRAFT_KEY_FORWARD))
            sz--;
        if (glfwGetKey(window, CRAFT_KEY_BACKWARD))
            sz++;
        if (glfwGetKey(window, CRAFT_KEY_LEFT))
            sx--;
        if (glfwGetKey(window, CRAFT_KEY_RIGHT)) sx++;
        if (glfwGetKey(window, GLFW_KEY_LEFT)) s->rx -= m;
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) s->rx += m;
        if (glfwGetKey(window, GLFW_KEY_UP))
            s->ry += m;
        if (glfwGetKey(window, GLFW_KEY_DOWN))
            s->ry -= m;
    }
    float vx, vy, vz;
    MathUtils::getMotionVector(flying, sz, sx, s->rx, s->ry, &vx, &vy, &vz);
    if (!typing) {
        if (glfwGetKey(window, CRAFT_KEY_JUMP) && !jump)
        {
            if (flying)
            {
                vy = 1;
            }
            else if (s->dy == 0)
            {
                s->dy += 3;
                s->y += 3;
            }
        }
    }
    float speed = flying ? 20 : 5;
    int estimate = roundf(sqrtf(
                                powf(vx * speed, 2) +
                                powf(vy * speed + ABS(s->dy) * 2, 2) +
                                powf(vz * speed, 2)) * dt * 8);
    int step = MAX(8, estimate);
    float ut = dt / step;
    vx = vx * ut * speed;
    vy = vy * ut * speed;
    vz = vz * ut * speed;
    
    if(vx!=0.0)
    printf("%f\n", vx);
    
    if (s->y < 0)
    {
        s->y = highestBlock(s->x, s->z)+2;
    }
    
    for (int i = 0; i < step; i++)
    {
        if (flying)
        {
            s->dy = 0;
        }
        else if (s->dy >  0){
            s->dy -= ut * 25;
            s->dy = MAX(s->dy, -250);
        }
        
        s->x += vx;
        s->y += vy + s->dy * ut;
        s->z += vz;
        
        s->dx = vx;
        s->dz = vz;
        
        if(collide(1, s))
        {
            s->dy = 0;
        }
        
        /*if (collide(3, &s->x, &s->y, &s->z))
        {
            s->dy = 0;
        }*/
    }
    
}

void Model::onChar(GLFWwindow *window, unsigned int u)
{
    if (suppress_char) {
        suppress_char = 0;
        return;
    }
    if (typing) {
        if (u >= 32 && u < 128) {
            char c = (char)u;
            int n = (int)strlen(typing_buffer);
            if (n < MAX_TEXT_LENGTH - 1) {
                typing_buffer[n] = c;
                typing_buffer[n + 1] = '\0';
            }
        }
    }
    else {
        if (u == CRAFT_KEY_CHAT) {
            typing = 1;
            typing_buffer[0] = '\0';
        }
        if (u == CRAFT_KEY_COMMAND) {
            typing = 1;
            typing_buffer[0] = '/';
            typing_buffer[1] = '\0';
        }
        if (u == CRAFT_KEY_SIGN) {
            typing = 1;
            typing_buffer[0] = CRAFT_KEY_SIGN;
            typing_buffer[1] = '\0';
        }
    }
}

void Model::onScroll(GLFWwindow *window, double xdelta, double ydelta)
{
    static double ypos = 0;
    ypos += ydelta;
    if (ypos < -SCROLL_THRESHOLD) {
        item_index = (item_index + 1) % item_count;
        ypos = 0;
    }
    if (ypos > SCROLL_THRESHOLD)
    {
        item_index--;
        if (item_index < 0)
        {
            item_index = item_count - 1;
        }
        ypos = 0;
    }
}