#pragma once
#include "GLApplication.h"


class GeometryRender : public GLApplication
{
protected:
	int rendering_program;
	GLuint vertex_array_object;
public:
	void startup(int x, int y, int w, int h, const char* title) override {
		GLApplication::startup(x, y, w, h, title);
		rendering_program = LoadProgram({
			{GL_VERTEX_SHADER,"D:/Workspace/OpenGL/GL001/shaders/vertex.shader"},
			{GL_TESS_CONTROL_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TCS.shader"},
			{GL_TESS_EVALUATION_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TES.shader"},
			{GL_FRAGMENT_SHADER,"D:/Workspace/OpenGL/GL001/shaders/fragment.shader"},
			{GL_GEOMETRY_SHADER,"D:/Workspace/OpenGL/GL001/shaders/Geometry.shader"},
			});
		glGenVertexArrays(1, &vertex_array_object);//glCreateVertexArrays(1, &vertex_array_object); null function
		glBindVertexArray(vertex_array_object);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void shutdown() override {
		GLApplication::shutdown();
		glDeleteProgram(rendering_program);
		glDeleteVertexArrays(1, &vertex_array_object);
	}

	void onRender(double time) override {
		glClearColor(std::sin(time) * 0.5f + 0.5f, std::cos(time) * 0.5f + 0.5f, 0.5, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(this->rendering_program);
		GLfloat offset[4]{ std::sin(time)*0.5 ,std::cos(time)*0.6,0,0 };
		GLfloat color[4]{ std::cos(time) * 0.5f + 0.5f ,std::sin(time) * 0.5f + 0.5f,0,1 };
		glVertexAttrib4fv(0, offset);
		glVertexAttrib4fv(1, color);
		glPointSize(6);
		//glDrawArrays(GL_POINTS, 0, 3);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_PATCHES, 0, 3);
	}
};

