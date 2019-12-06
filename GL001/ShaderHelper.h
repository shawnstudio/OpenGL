#pragma once
#include<GL/glew.h>

GLuint CompileShaderFile(GLenum type,const char* file);

//GLuint LinkShaderProgram(bool seperate...);

GLuint LinkShaderProgram(bool seperate, int num,GLuint* shaders);