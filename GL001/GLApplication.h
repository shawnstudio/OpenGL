#pragma once
#define GLEW_STATIC
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string.h>
#include <ctime>
#include <map>
#include<vector>
#include<sstream>
#include "ShaderHelper.h"
using namespace std;

void _sleep(long ms);

class GLApplication
{
public:
	GLApplication() {
		window = nullptr;
		clearG = clearR = clearB = 0;
		clearA = 1;
		fps = 60;
		application = this;
	}
	~GLApplication() {
		application = nullptr;
	}

	virtual void startup(int x,int y,int w,int h,const char* title) {
		glfwInit();
		glfwSetErrorCallback(&GLApplication::glfwErrorcallback);
		window =glfwCreateWindow(w, h, title, nullptr, nullptr);
		if (window == nullptr)
		{
			cout << "get window failed" << endl;
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glViewport(x, y, w, h);
		if (glewInit() != GLEW_OK)
		{
			throw "GLEW init failed!!!";
			return;
		}
		const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
		const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
		const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
		printf("OpenGL实现厂商的名字：%s\n", name);
		printf("渲染器标识符：%s\n", biaoshifu);
		printf("OpenGL实现的版本号：%s\n", OpenGLVersion);

		int exNum = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &exNum);
		printf("Max_Uniform_buffer_Bindings %d\n", exNum);

		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &exNum);
		printf("Max_Uniform_BlockSize %d\n", exNum);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &exNum);
		printf("Max_Texture_Image_Units %d\n", exNum);

		glGetIntegerv(GL_NUM_EXTENSIONS, &exNum);
		for (int i = 0; i < exNum; i++) {
			auto n = glGetStringi(GL_EXTENSIONS, i);
			cout << "supported extension " << n << endl;
		}
	}

	virtual void run() {
		auto lasttime = std::clock();
		while (!glfwWindowShouldClose(window))
		{
			auto currtime = std::clock();
			auto delta = (currtime - lasttime) / 1000.0f;
			auto nexttime = lasttime + (int)(1000.0f / fps);
			lasttime = currtime;

			processInput();
			this->onUpdate(delta);

			glClearColor(this->clearR, this->clearG, this->clearB, this->clearA);
			glClear(GL_COLOR_BUFFER_BIT);

			this->onRender(currtime / 1000.0f,delta);

			glfwSwapBuffers(window);
			glfwPollEvents();

			processError();
			auto sleeptime = nexttime - std::clock();
			_sleep(sleeptime < 0 ? 0 : sleeptime);
		}
	}

	virtual void shutdown() {
	}

	virtual void onUpdate(double time) {

	}

	virtual void onRender(double time, double delta) {
	}

	void SetClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
		clearA = a;
		clearR = r;
		clearG = g;
		clearB = b;
	}

	void SetFps(int fps) {
		this->fps = fps;
	}

	//int LoadShader(const char* file, GLenum type) {
	//	ifstream s;
	//	s.open(file, ios::in);
	//	if (!s.is_open())
	//	{
	//		cout << "open failed :" << file << endl;
	//		return -1;
	//	}

	//	stringstream sstream;
	//	sstream << s.rdbuf();
	//	s.close();

	//	string sbuff;
	//	sbuff = sstream.str();

	//	int shader = glCreateShader(type);
	//	const GLchar* const str = sbuff.c_str();
	//	int len = strlen(str);
	//	glShaderSource(shader, 1, &str, &len);
	//	glCompileShader(shader);
	//	return shader;
	//}

	int LoadProgram(map<GLenum,const char*> shaders) {
		vector<GLuint> ids;
		for (const auto& s : shaders)
		{
			ids.push_back(CompileShaderFile(s.first, s.second));
		}
		return LinkShaderProgram(false, ids.size(), ids.data());
	}

protected:
	GLFWwindow* window;
	GLfloat clearR, clearG, clearB, clearA;
	int fps;

	void processInput() {

	}

	const char* getGLErrorInfo(int errorId)
	{
		switch (errorId)
		{
		case GL_INVALID_ENUM:
			return ("GL Invalid Enum\n");
		case GL_INVALID_VALUE:
			return ("GL Invalid Value\n");
		case GL_INVALID_OPERATION:
			return ("GL Invalid Operation\n");
		case GL_OUT_OF_MEMORY:
			return ("GL Out Of Memory\n");
			//case GL_INVALID_FRAMEBUFFER_OPERATION:
			//	return ("GL Invalid FrameBuffer Operation\n");
		case  GL_STACK_OVERFLOW:
			return ("GL Stack Overflow\n");
		case GL_STACK_UNDERFLOW:
			return ("GL Stack Underflow\n");
			//case GL_TABLE_TOO_LARGE:
			//	return ("GL Table Too Large\n");
		};

		return "GL Undefined Error";
	}

	void processError() {
		for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
		{
			cout << "gl error id=" << err << "message:" << getGLErrorInfo(err) << endl;
		}
	}

	virtual void onViewport(int width,int height){}

private:
	static GLApplication* application;
	static void glfwErrorcallback(int error, const char* description) {
		cout << "glfw Error:" << error << "\nmessage:" << description << endl;
	}
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
		if (application != nullptr) {
			application->onViewport(width,height);
		}
	}
};