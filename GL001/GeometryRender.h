#pragma once
#include "GLApplication.h"
#include "../vmath.h"


class GeometryRender : public GLApplication
{
protected:
	int rendering_program;
	GLuint vao;
	GLuint buffer;
public:
	void startup(int x, int y, int w, int h, const char* title) override {
		GLApplication::startup(x, y, w, h, title);
		rendering_program = LoadProgram({
			{GL_VERTEX_SHADER,"D:/Workspace/OpenGL/GL001/shaders/vertex.shader"},
			//{GL_TESS_CONTROL_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TCS.shader"},
			//{GL_TESS_EVALUATION_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TES.shader"},
			{GL_FRAGMENT_SHADER,"D:/Workspace/OpenGL/GL001/shaders/fragment.shader"},
			//{GL_GEOMETRY_SHADER,"D:/Workspace/OpenGL/GL001/shaders/Geometry.shader"},
			});
		glGenVertexArrays(1, &vao);//glCreateVertexArrays(1, &vertex_array_object); null function
		glBindVertexArray(vao);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, 1024, nullptr, GL_MAP_WRITE_BIT);

		glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vmath::vec4));
		glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(vao, 0);
	}

	void shutdown() override {
		GLApplication::shutdown();
		glDeleteProgram(rendering_program);
		glDeleteVertexArrays(1, &vao);
	}

	void onRender(double time) override {
		glClearColor(std::sin(time) * 0.5f + 0.5f, std::cos(time) * 0.5f + 0.5f, 0.5, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(this->rendering_program);
		GLfloat offset[4]{ std::sin(time)*0.5 ,std::cos(time)*0.6,0,0 };
		GLfloat color[4]{ std::cos(time) * 0.5f + 0.5f ,std::sin(time) * 0.5f + 0.5f,0,1 };
		glNamedBufferSubData(buffer, 0, sizeof(offset), offset);
		//glVertexAttrib4fv(0, offset);
		//glVertexAttrib4fv(1, color);
		glPointSize(6);
		//glDrawArrays(GL_POINTS, 0, 3);
		glDrawArrays(GL_POINTS, 0, 1);
		//glDrawArrays(GL_PATCHES, 0, 3);
	}
};

