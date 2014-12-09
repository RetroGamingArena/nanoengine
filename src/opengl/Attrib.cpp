//
//  Attrib.cpp
//  Nanocraft
//
//  Created by Julien CAILLABET on 21/10/2014.
//  Copyright (c) 2014 Julien CAILLABET. All rights reserved.
//

#include "Attrib.h"
#include "AbstractWindow.h"
#include "BufferUtils.h"
#include "Engine.h"
#include "Model.h"
#include "matrix.h"
#include "item.h"
#include "util.h"

void Attrib::drawLines(GLuint buffer, int components, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, components, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINES, 0, count);
    glDisableVertexAttribArray(position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Attrib::renderItem(Model* model)
{
    AbstractWindow* window = Engine::getInstance()->getAbstractWindow();
    //Model* g = Engine::getInstance()->getModel();
 float matrix[16];
 set_matrix_item(matrix, model->width, model->height, model->scale);
 glUseProgram(program);
 glUniformMatrix4fv(this->matrix, 1, GL_FALSE, matrix);
 glUniform3f(camera, 0, 0, 5);
 glUniform1i(sampler, 0);
 glUniform1f(timer, window->timeOfDay());
 int w = items[model->item_index];
 if (is_plant(w))
 {
     GLuint buffer = BufferUtils::genPlantBuffer(0, 0, 0, 0.5, w);
     drawPlant(buffer);
     del_buffer(buffer);
 }
 else
 {
     GLuint buffer = BufferUtils::genCubeBuffer(0, 0, 0, 0.5, w);
     drawCube(buffer);
     del_buffer(buffer);
 }
}

void Attrib::drawCube(GLuint buffer)
{
    drawItem(buffer, 36);
}

void Attrib::drawPlant(GLuint buffer)
{
    drawItem(buffer, 24);
}

void Attrib::drawItem(GLuint buffer, int count)
{
    drawTriangles3dAo(buffer, count);
}

void Attrib::drawTriangles3dAo(GLuint buffer, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(normal);
    //glEnableVertexAttribArray(attrib->color);
    glEnableVertexAttribArray(uv);
    
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 10, 0);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 10, (GLvoid *)(sizeof(GLfloat) * 3));
    /*glVertexAttribPointer(attrib->color, 4, GL_FLOAT, GL_FALSE,
     sizeof(GLfloat) * 14, (GLvoid *)(sizeof(GLfloat) * 6));*/
    glVertexAttribPointer(uv, 4, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 10, (GLvoid *)(sizeof(GLfloat) * 6));
    
    glDrawArrays(GL_TRIANGLES, 0, count);
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(normal);
    //glEnableVertexAttribArray(attrib->color);
    glDisableVertexAttribArray(uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Attrib::drawTriangles3dText(GLuint buffer, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(uv);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 5, 0);
    glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 5, (GLvoid *)(sizeof(GLfloat) * 3));
    glDrawArrays(GL_TRIANGLES, 0, count);
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Attrib::drawTriangles3d(GLuint buffer, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(normal);
    glEnableVertexAttribArray(uv);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 8, 0);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 8, (GLvoid *)(sizeof(GLfloat) * 3));
    glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 8, (GLvoid *)(sizeof(GLfloat) * 6));
    glDrawArrays(GL_TRIANGLES, 0, count);
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(normal);
    glDisableVertexAttribArray(uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Attrib::drawTriangles2d(GLuint buffer, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(uv);
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 4, 0);
    glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 4, (GLvoid *)(sizeof(GLfloat) * 2));
    glDrawArrays(GL_TRIANGLES, 0, count);
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Attrib::drawChunk(Chunk *chunk)
{
    drawTriangles3dAo(chunk->buffer, chunk->faces * 6);
}

void Attrib::drawText(GLuint buffer, int length)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawTriangles2d(buffer, length * 6);
    glDisable(GL_BLEND);
}

void Attrib::drawSigns(Chunk *chunk)
{
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-8, -1024);
    drawTriangles3dText(chunk->sign_buffer, chunk->sign_faces * 6);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Attrib::drawSign(GLuint buffer, int length)
{
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-8, -1024);
    drawTriangles3dText(buffer, length * 6);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Attrib::drawPlayer(Player *player)
{
    drawCube(player->buffer);
}

int Attrib::renderChunks(Model *model, Player *player)
{
    
    AbstractWindow* window = Engine::getInstance()->getAbstractWindow();
    
    int result = 0;
    State *s = &player->state;
    Engine::getInstance()->ensureChunks(player);
    int p = Chunk::chunked(s->x);
    int q = Chunk::chunked(s->z);
    float light = window->getDayLight();
    float matrix[16];
    set_matrix_3d(matrix, model->width, model->height,s->x, s->y, s->z, s->rx, s->ry, model->fov, model->ortho, model->render_radius);
    float planes[6][4];
    frustum_planes(planes, model->render_radius, matrix);
    glUseProgram(program);
    glUniformMatrix4fv(this->matrix, 1, GL_FALSE, matrix);
    glUniform3f(camera, s->x, s->y, s->z);
    glUniform1i(sampler, 0);
    glUniform1i(extra1, 2);
    glUniform1f(extra2, light);
    glUniform1f(extra3, model->render_radius * CHUNK_SIZE);
    glUniform1i(extra4, model->ortho);
    glUniform1f(timer, window->timeOfDay());
    for (int i = 0; i < model->chunks->size(); i++)
    {
        Chunk *chunk = (*model->chunks)[i];
        if (chunk->distance(p, q) > model->render_radius)
        {
            continue;
        }
        if (!model->chunkVisible(planes, chunk->p, chunk->q, chunk->miny, chunk->maxy))
        {
            continue;
        }
        drawChunk(chunk);
        result += chunk->faces;
    }
    return result;
}

void Attrib::renderPlayers(Model *model, Player *player)
{
 AbstractWindow* window = Engine::getInstance()->getAbstractWindow();
 
 State *s = &player->state;
 float matrix[16];
 set_matrix_3d(matrix, model->width, model->height, s->x, s->y, s->z, s->rx, s->ry, model->fov,model->ortho, model->render_radius);
 glUseProgram(program);
 glUniformMatrix4fv(this->matrix, 1, GL_FALSE, matrix);
 glUniform3f(camera, s->x, s->y, s->z);
 glUniform1i(sampler, 0);
 glUniform1f(timer, window->timeOfDay());
 for (int i = 0; i < model->player_count; i++) {
 Player *other = model->players + i;
 if (other != player) {
 drawPlayer(other);
 }
 }
 }

void Attrib::renderSign(Model *model, Player *player) {
 if (!model->typing || model->typing_buffer[0] != CRAFT_KEY_SIGN) {
 return;
 }
 int x, y, z, face;
 if (!model->hitTestFace(player, &x, &y, &z, &face)) {
 return;
 }
 State *s = &player->state;
 float matrix[16];
 set_matrix_3d(
 matrix, model->width, model->height,
 s->x, s->y, s->z, s->rx, s->ry, model->fov, model->ortho, model->render_radius);
 glUseProgram(program);
 glUniformMatrix4fv(this->matrix, 1, GL_FALSE, matrix);
 glUniform1i(sampler, 3);
 glUniform1i(extra1, 1);
 char text[MAX_SIGN_LENGTH];
 strncpy(text, model->typing_buffer + 1, MAX_SIGN_LENGTH);
 text[MAX_SIGN_LENGTH - 1] = '\0';
 GLfloat *data = malloc_faces(5, (int)strlen(text));
 int length = BufferUtils::_genSignBuffer(data, x, y, z, face, text);
 GLuint buffer = gen_faces(5, length, data);
 drawSign(buffer, length);
 del_buffer(buffer);
 }
 
void Attrib::renderText(Model *model, int justify, float x, float y, float n, char *text)
 {
 float matrix[16];
 set_matrix_2d(matrix, model->width, model->height);
 glUseProgram(program);
 glUniformMatrix4fv(this->matrix, 1, GL_FALSE, matrix);
 glUniform1i(sampler, 1);
 glUniform1i(extra1, 0);
 int length = (int)strlen(text);
 x -= n * justify * (length - 1) / 2;
 GLuint buffer = BufferUtils::genTextBuffer(x, y, n, text);
 drawText(buffer, length);
 del_buffer(buffer);
 }

void Attrib::renderSigns(Model *model, Player *player)
{
    State *s = &player->state;
    int p = Chunk::chunked(s->x);
    int q = Chunk::chunked(s->z);
    float matrix[16];
    set_matrix_3d(matrix, model->width, model->height, s->x, s->y, s->z, s->rx, s->ry, model->fov, model->ortho, model->render_radius);
    float planes[6][4];
    frustum_planes(planes, model->render_radius, matrix);
    glUseProgram(program);
    glUniformMatrix4fv(this->matrix, 1, GL_FALSE, matrix);
    glUniform1i(sampler, 3);
    glUniform1i(extra1, 1);
    for (int i = 0; i < model->chunks->size(); i++) {
        Chunk *chunk = (*model->chunks)[i];
        if (chunk->distance(p, q) > model->sign_radius) {
            continue;
        }
        if (!model->chunkVisible(planes, chunk->p, chunk->q, chunk->miny, chunk->maxy))
        {
            continue;
        }
        drawSigns(chunk);
    }
}
