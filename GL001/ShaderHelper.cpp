#include "ShaderHelper.h"
#include<fstream>
#include<sstream>
#include<string>
#include<iostream>
#include <cstdarg>

using namespace std;

GLuint CompileShaderFile(GLenum type,const char* file) {
	ifstream fs(file, ios::in);
	if (!fs.is_open()) {
		cout << "read file" << file << "error!!!" << endl;
		return 0;
	}
	stringstream tmpBuffer;
	tmpBuffer << fs.rdbuf();
	fs.close();

	string str = tmpBuffer.str();
	const GLchar* code = str.c_str();
	GLuint shader = glCreateShader(type);
	const int len = str.length();
	glShaderSource(shader, 1, &code, &len);

	glCompileShader(shader);

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE) {
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		char* log = new char[logLen + 1];
		glGetShaderInfoLog(shader, logLen + 1, &logLen, log);
		cout << "compile shader " << file << " error:\n" << log << endl;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

//GLuint LinkShaderProgram(bool seperate...)
//{
//	int count;
//	va_list args;
//	va_start(args, count);
//	GLuint* shaders = new GLuint[count];
//	for (int i = 0; i < count; ++i)
//	{
//		GLuint arg = va_arg(args, GLuint);
//		shaders[i] = arg;
//	}
//	va_end(args);
//	GLuint pro = LinkShaderProgram(seperate, count, shaders);
//	delete[] shaders;
//	return pro;
//}

GLuint LinkShaderProgram(bool seperate, int num, GLuint* shaders)
{
	GLuint pro = glCreateProgram();
	if (seperate) {
		glProgramParameteri(pro, GL_PROGRAM_SEPARABLE, GL_TRUE);
	}
	for (int i = 0; i < num; ++i){
		glAttachShader(pro, shaders[i]);
	}
	glLinkProgram(pro);
	GLint lnkStatus;
	glGetProgramiv(pro, GL_LINK_STATUS, &lnkStatus);
	if (lnkStatus != GL_TRUE) {
		GLint logLen;
		glGetProgramiv(pro, GL_INFO_LOG_LENGTH, &logLen);
		char* log = new char[logLen + 1];
		glGetProgramInfoLog(pro, logLen + 1, &logLen, log);
		cout << "link program error:\n" << log << endl;
		glDeleteProgram(pro);
		return 0;
	}
	return pro;
}
