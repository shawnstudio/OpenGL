#pragma once
#define GLEW_STATIC
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string.h>
#include <ctime>
#include <Windows.h>
#include <map>
#include<vector>
using namespace std;

class GLApplication
{
public:
	GLApplication() {
		window = nullptr;
		clearG = clearR = clearB = 0;
		clearA = 1;
		fps = 60;
	}
	~GLApplication() {

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

			this->onRender(currtime / 1000.0f);

			glfwSwapBuffers(window);
			glfwPollEvents();

			processError();
			auto sleeptime = nexttime - std::clock();
			Sleep(sleeptime < 0 ? 0 : sleeptime);
		}
	}

	virtual void shutdown() {
	}

	virtual void onUpdate(double time) {

	}

	virtual void onRender(double time) {
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

	int LoadShader(const char* file, GLenum type) {
		ifstream s;
		s.open(file, ios::in);
		if (!s.is_open())
		{
			cout << "open failed :" << file << endl;
			return -1;
		}
		char buffer[2048];
		s.getline(buffer, 2048, '\0');
		s.close();

		int shader = glCreateShader(type);
		int len = std::strlen(buffer);
		const GLchar* const str = buffer;
		glShaderSource(shader, 1, &str, &len);
		glCompileShader(shader);
		return shader;
	}

	int LoadProgram(map<GLenum,const char*> shaders) {
		vector<int> ids;
		for (const auto& s : shaders)
		{
			ids.push_back(LoadShader(s.second, s.first));
		}
		int program = glCreateProgram();
		for (const auto& s : ids) {
			glAttachShader(program, s);
		}
		glLinkProgram(program);

		for (const auto& s : ids) {
			glDeleteShader(s);
		}
		return program;
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

private:

	static void glfwErrorcallback(int error, const char* description) {
		cout << "glfw Error:" << error << "\nmessage:" << description << endl;
	}
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}
};