#ifndef _util_h_
#define _util_h_

#include <glew.h>
#include <GLFW/glfw3.h>
#include "config.h"



#if DEBUG
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

int rand_int(int n);
double rand_double();

GLuint gen_buffer(GLsizei size, GLfloat *data);
void del_buffer(GLuint buffer);
GLfloat *malloc_faces(int components, int faces);
GLuint gen_faces(int components, int faces, GLfloat *data);
GLuint make_shader(GLenum type, const char *source);
GLuint load_shader(GLenum type, const char *path);
char *load_file(const char *path);
GLuint make_program(GLuint shader1, GLuint shader2);
GLuint load_program(const char *path1, const char *path2);
void load_png_texture(const char *file_name);
char *tokenize(char *str, const char *delim, char **key);
int char_width(char input);
int string_width(const char *input);
int wrap(const char *input, int max_width, char *output, int max_length);

#endif
