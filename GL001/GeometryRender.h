#pragma once
#include "GLApplication.h"
#include "../vmath.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>


class GeometryRender : public GLApplication
{
protected:
	int rendering_program;
	GLuint vao;
	GLuint buffer[2];
	GLuint unibuffer;
	GLuint storageBuffer;
public:
	void startup(int x, int y, int w, int h, const char* title) override {
		GLApplication::startup(x, y, w, h, title);
		rendering_program = LoadProgram({
			{GL_VERTEX_SHADER,"D:/OpenGL/shaders/vertex.shader"},
			//{GL_TESS_CONTROL_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TCS.shader"},
			//{GL_TESS_EVALUATION_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TES.shader"},
			{GL_FRAGMENT_SHADER,"D:/OpenGL/shaders/fragment.shader"},
			//{GL_GEOMETRY_SHADER,"D:/Workspace/OpenGL/GL001/shaders/Geometry.shader"},
			});
		glGenVertexArrays(1, &vao);//glCreateVertexArrays(1, &vertex_array_object); null function
		glBindVertexArray(vao);
		//glBindVertexArray(vao[1]);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glCreateBuffers(2, buffer);

		/*由两个Buffer提供数据*/
		//pos
		//glNamedBufferStorage(buffer[0], 1024, nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		//glVertexArrayVertexBuffer(vao, 0, buffer[0], 0, sizeof(vmath::vec4));
		//glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
		//glVertexArrayAttribBinding(vao, 0, 0);
		//glEnableVertexArrayAttrib(vao, 0);

		////colors
		//glNamedBufferStorage(buffer[1], 1024, nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
		//glVertexArrayVertexBuffer(vao, 1, buffer[1], 0, sizeof(vmath::vec4));
		//glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
		//glVertexArrayAttribBinding(vao, 1, 1);
		//glEnableVertexArrayAttrib(vao, 1);

		//由一个Buffer提供数据
		glNamedBufferStorage(buffer[0], 1024, nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayVertexBuffer(vao, 0, buffer[0], 0, sizeof(vmath::vec4)*2);
		glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, sizeof(vmath::vec4));

		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribBinding(vao, 1, 0);

		glEnableVertexArrayAttrib(vao, 0);
		glEnableVertexArrayAttrib(vao, 1);
		//glEnableVertexAttribArray(vao);

		//Uniform Block
		glCreateBuffers(1, &unibuffer);
		glNamedBufferStorage(unibuffer, 512, nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		GLuint blockIdx = glGetUniformBlockIndex(this->rendering_program, "TransformBlock");
		glUniformBlockBinding(rendering_program, blockIdx, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, unibuffer);

		//Storage Block
		glCreateBuffers(1, &storageBuffer);
		glNamedBufferStorage(storageBuffer, 1024, nullptr, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, storageBuffer);

		//Texture
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, 1, GL_RGBA32F, 256, 256);
		//glTextureSubImage2D(texture, 0, 0, 0, 256, 256, GL_RGBA32F, GL_FLOAT, nullptr);
		float imgColor[4]{ 1,1,0,1 };
		glClearTexSubImage(texture, 0, 0, 0, 0, 256, 256, 0, GL_RGBA32F, GL_FLOAT, imgColor);
		//int ix, iy,ichannels;
		//auto data = stbi_load("D:/OpenGL/image.jpg", &ix, &iy, &ichannels, 0);
		//glTextureSubImage2D(texture, 0, 0, 0, x, y,GL_RGB,GL_FLOAT,data);
	}

	void shutdown() override {
		GLApplication::shutdown();
		glDeleteProgram(rendering_program);
		glDeleteVertexArrays(1, &vao);
	}

	void onRender(double time) override {
		//glClearColor(std::sin(time) * 0.5f + 0.5f, std::cos(time) * 0.5f + 0.5f, 0.5, 1);
		//glClearColor(0, 0, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT);
		float clearColor[]{0,0,0,1};
		glClearBufferfv(GL_COLOR, 0, clearColor);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0, 0);

		//vmath::vec4 pos(std::sin(time) * 0.5, std::cos(time) * 0.6, 0.5, 1);
		glUseProgram(this->rendering_program);
		//GLfloat offset[4]{ std::sin(time)*0.5 ,std::cos(time)*0.6,0,0 };
		//GLfloat color[4]{ std::cos(time) * 0.5f + 0.5f ,std::sin(time) * 0.5f + 0.5f,0,1 };
		
		//const char* names[]{
		//	"TransformBlock.scale",
		//	"TransformBlock.scaleX",
		//	"TransformBlock.scalea",
		//	"TransformBlock.scale2",
		//	"TransformBlock.scale3",
		//	"TransformBlock.scale4",
		//	"TransformBlock.m2",
		//	"TransformBlock.m3",
		//	"TransformBlock.m4",
		//	"TransformBlock.fe",
		//	//"TransformBlock.translation",
		//	//"TransformBlock.offset",
		//	//"TransformBlock.projection_matrix"
		//};
		//GLuint ids[10];
		//glGetUniformIndices(this->rendering_program, sizeof(names)/sizeof(void*), names, ids);
		//GLint offsets[10];
		//glGetActiveUniformsiv(this->rendering_program, sizeof(names) / sizeof(void*), ids,GL_UNIFORM_OFFSET,offsets);

		//GLint id = glGetUniformLocation(rendering_program, "mtRot");
		//float maxtrix[16]{
		//	std::cos(time),-std::sin(time),0,0,
		//	std::sin(time),std::cos(time),0,0,
		//	0,0,1,0,
		//	0,0,0,1
		//};
		//glUniformMatrix4fv(id, 1, true, maxtrix);

		//两个Buffer
		//vmath::vec4 poss[3]{
		//	vmath::vec4(std::sin(time) * 0.5, std::cos(time) * 0.5, 0.5, 1),
		//	vmath::vec4(std::cos(time) * 0.5, std::cos(time) * 0.6, 0.5, 1),
		//	vmath::vec4(std::cos(time) * 0.5, std::sin(time) * 0.7, 0.5, 1),
		//};

		//vmath::vec4 colors[3]{
		//	vmath::vec4(std::sin(time) * 0.5 + 0.5, std::cos(time) * 0.5 + 0.5, 0, 1),
		//	vmath::vec4(0, std::sin(time) * 0.5 + 0.5, std::cos(time) * 0.5 + 0.5, 1),
		//	vmath::vec4(1,1, 1,1)
		//};
		//glNamedBufferSubData(buffer[0], 0, sizeof(poss), poss);

		//void* cbuf = glMapNamedBuffer(buffer[1], GL_WRITE_ONLY);
		//memcpy(cbuf, colors, sizeof(colors));
		//glUnmapNamedBuffer(buffer[1]);
		
		//一个Buffer
		auto color1 = vmath::vec4(1, 1, 1, 1);
		auto color2 = vmath::vec4(0, std::sin(time) * 0.5 + 0.5, std::cos(time) * 0.5 + 0.5, 1);
		auto color3 = vmath::vec4(std::sin(time) * 0.5 + 0.5, std::cos(time) * 0.5 + 0.5, 0, 1);
		auto color4 = vmath::vec4(std::sin(time) * 0.5 + 0.5, 0, std::cos(time) * 0.5 + 0.5, 1);
		float fFpos = 0.34;
		vmath::vec4 pos_colors[]{
			vmath::vec4(std::sin(fFpos) * 0.5, std::cos(fFpos) * 0.5, 0.5, 1),color1,
			vmath::vec4(std::cos(fFpos) * 0.5, std::cos(fFpos) * 0.6, 0.5, 1),color2,
			vmath::vec4(std::cos(fFpos) * 0.5, std::sin(fFpos) * 0.7, 0.5, 1),color3
		};

		vmath::vec4 fourFace[]{
			vmath::vec4(0,0,0,1),color1,
			vmath::vec4(-1,-1,1,1),color2,
			vmath::vec4(1,-1,1,1),color3,
			vmath::vec4(0,-1,-1,1),color4,
		};

		auto mpos = vmath::vec4(0,1, -5 + sin(time), 0);
		auto mscale = vmath::vec4(1, 1, 1, 1);
		auto mrote = vmath::vec4(0, 0, (sin(time)*0.5 + 0.5)*3.142 * 2, 0);
		auto pMatix = vmath::perspective(60, 1, 0.1, 1000);

		char* pcbuf = (char*)glMapNamedBuffer(buffer[0], GL_WRITE_ONLY);
		//memcpy(pcbuf, pos_colors, sizeof(pos_colors));
		GenTriangle(pcbuf, fourFace, 0, 1, 2);
		GenTriangle(pcbuf, fourFace, 0, 2, 3);
		GenTriangle(pcbuf, fourFace, 0, 1, 3);
		GenTriangle(pcbuf, fourFace, 3, 1, 2);
		glUnmapNamedBuffer(buffer[0]);

		char* sbuffer = (char*)glMapNamedBuffer(storageBuffer, GL_WRITE_ONLY);
		sbuffer += 16;
		//memcpy(pcbuf, pos_colors, sizeof(pos_colors));
		GenTriangle(sbuffer, fourFace, 0, 1, 2);
		GenTriangle(sbuffer, fourFace, 0, 2, 3);
		GenTriangle(sbuffer, fourFace, 0, 1, 3);
		GenTriangle(sbuffer, fourFace, 3, 1, 2);
		glUnmapNamedBuffer(storageBuffer);

		char* ubuf = (char*)glMapNamedBuffer(unibuffer, GL_WRITE_ONLY);
		memcpy(ubuf, &mpos, 16); ubuf += 16;
		memcpy(ubuf, &mscale, 16); ubuf += 16;
		memcpy(ubuf, &mrote, 16); ubuf += 16;
		memcpy(ubuf, &pMatix, 64); ubuf += 64;

		glUnmapNamedBuffer(unibuffer);

		//glVertexAttrib4fv(0, offset);
		//glVertexAttrib4fv(1, color);
		glPointSize(6);
		//glDrawArrays(GL_POINTS, 0, 3);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 4);
		//glDrawArrays(GL_PATCHES, 0, 3);
		for (int i = 1; i < 5; i++) {
			char* iubuf = (char*)glMapNamedBuffer(unibuffer, GL_WRITE_ONLY);
			mpos[0] += i * 0.3;
			mpos[2] += i * 0.3;
			memcpy(iubuf, &mpos, 16);
			glUnmapNamedBuffer(unibuffer);
			glDrawArrays(GL_TRIANGLES, 0, 3 * 4);
		}
	}

	void GenTriangle(char*& buffer, vmath::vec4* vertexs, int idx1, int idx2, int idx3) {
		int num = sizeof(vmath::vec4) * 2;
		memcpy(buffer, &(vertexs[idx1 * 2]), num); buffer += num;
		memcpy(buffer, &(vertexs[idx2 * 2]), num); buffer += num;
		memcpy(buffer, &(vertexs[idx3 * 2]), num); buffer += num;
	}
};

