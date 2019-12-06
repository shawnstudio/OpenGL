#pragma once
#include"GLApplication.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <../vmath.h>
//#include <climits>
#include <vector>
#include <map>
#include <stb_image.h>

using namespace std;
using namespace vmath;

#define IDGAP 0xffffffff


class Vertex {
public:
	vec4 position;
	vec4 color;
	vec3 normal;
	vec3 tangent;
	vec2 uv;
};


mat4 ZxyRote(const vec3& r) {
	return vmath::rotate(r[1], vec3(0, 1, 0)) * vmath::rotate(r[0], vec3(1, 0, 0)) * vmath::rotate(r[2], vec3(0, 0, 1));
}

mat4 YxzRote(const vec3& r) {
	return  vmath::rotate(r[2], vec3(0, 0, 1)) * vmath::rotate(r[0], vec3(1, 0, 0)) * vmath::rotate(r[1], vec3(0, 1, 0));
}

class Model {
public:
	vec3 scale;
	vec3 position;
	vec3 rotation;
	Vertex* vertices;

	unsigned int verticesNum;
	unsigned int* indices;
	unsigned int indicesNum;

	vmath::mat4 worldMatrix() {
		return vmath::translate(position[0], position[1], position[2]) * ZxyRote(rotation) * vmath::scale(scale[0], scale[1], scale[2]);
	}

	Model(aiMesh** meshs, int meshNum,vec3 pos,vec3 rot, vec3 scale) {
		position = pos;
		rotation = rot;
		this->scale = scale;
		verticesNum = 0;
		indicesNum = 0;

		for (int i = 0; i < meshNum; i++) {
			aiMesh* m = meshs[i];
			verticesNum += m->mNumVertices;
			for (int j = 0; j < m->mNumFaces; j++) {
				aiFace v = m->mFaces[0];
				indicesNum += v.mNumIndices + 1;
			}
		}

		vertices = new Vertex[verticesNum];
		indices = new unsigned int[indicesNum];

		size_t iv = 0;
		size_t idx = 0;
		int voffset = 0;
		for (int i = 0; i < meshNum; i++) {
			aiMesh* m = meshs[i];
			for (int p = 0; p < m->mNumVertices; p++) {
				auto v = m->mVertices[p];
				auto n = m->mNormals[p];
				auto uv = m->mTextureCoords[0][p];
				auto tan = m->mTangents[p];
				auto color = m->mColors[0][p];
				vertices[iv].position = vec4(v.x, v.y, v.z, 1);
				vertices[iv].color = vec4(color.r, color.g, color.b,color.a);
				vertices[iv].uv = vec2(uv.x, uv.y);
				vertices[iv].normal = vec3(n.x, n.y, n.z);
				vertices[iv].tangent = vec3(tan.x, tan.y, tan.z);
				iv++;
			}
			for (int j = 0; j < m->mNumFaces; j++) {
				aiFace v = m->mFaces[0];
				for (int ix = 0; ix < v.mNumIndices; ix++) {
					indices[idx++] = v.mIndices[idx] + voffset;
				}
				indices[idx++] = IDGAP;
			}
			voffset = iv - 1;
		}
	}
};

class ICamera {
public:
	virtual mat4 viewMatrix() = 0;
	virtual mat4 projectionMatrix() = 0;
};


class Camera : public ICamera {
public:
	vec3 position;
	vec3 rotation;
	float fovy;
	float aspect;
	float nearplan;
	float farplan;

	mat4 viewMatrix() override {
		static mat4 zInvers = mat4::identity();
		zInvers[2][2] = -1;
		return zInvers * YxzRote(rotation * -1) * translate(position * -1);
	}

	mat4 projectionMatrix() override {
		return vmath::perspective(fovy, aspect, nearplan, farplan);
	}
};

class Light {

};

class Scene {
public:
	Camera camera;
	vector<Light*> lights;
	vector<Model*> models;

	Model* AddModel(const char* path) {
		Assimp::Importer ip;
		const aiScene* scene = ip.ReadFile(path, aiProcess_CalcTangentSpace |
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if (scene == nullptr) {
			cout << "load model: " << path << " error" << endl;
			return nullptr;
		}
		Model* model = new Model(scene->mMeshes, scene->mNumMeshes, vec3(0), vec3(0), vec3(1));
		models.push_back(model);
		return model;
	}

	
};

class Sampler2D {
public:
	GLuint texture;
	GLuint sampler;
	int width;
	int height;
	int channel;

	//filter:GL_<*FILTER>_MIPMAP_<*SELECTOR> LINEAR or NEAREST  wrap:
	Sampler2D(const char* path,GLuint minf,GLuint maxf,GLuint wrap,bool bMipmap) {
		 auto data = stbi_load(path, &width, &height, &channel, 0);
		 glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		 glTextureSubImage2D(texture, 0, 0, 0, width, height, channel==4?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, data);
		 glTextureParameteri(texture, GL_TEXTURE_BASE_LEVEL, 0);
		 glTextureParameteri(texture, GL_TEXTURE_MAX_LEVEL, bMipmap ? 4 : 0);
		 stbi_image_free(data);

		 glCreateSamplers(1, &sampler);
		 glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, minf);
		 glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, maxf);
		 glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrap);
		 glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrap);
	}

	void Bind(GLuint index) {
		glBindTextureUnit(index, texture);
		glBindSampler(index, sampler);
	}

	virtual ~Sampler2D() {
		glDeleteTextures(1, &texture);
		glDeleteSamplers(1, &sampler);
	}
};

class Metrial {
public:
	vector<Sampler2D*> textures;
	GLuint program;

	Metrial(vector<string> txs, map<GLenum, string> shs){
		GLuint* ss = new GLuint[shs.size()];
		GLuint* sspointer = ss;
		for (auto& e : shs) {
			*sspointer = CompileShaderFile(e.first, e.second.c_str());
			sspointer++;
		}
		program = LinkShaderProgram(false, shs.size(), ss);

		for (auto& tx : txs) {
			Sampler2D* sm = new Sampler2D(tx.c_str(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, true);
			textures.push_back(sm);
		}
	}

	virtual void Use() {
		glUseProgram(program);
		for (size_t i = 0; i < textures.size(); i++)
		{
			textures[i]->Bind(i);
		}
	}

	virtual ~Metrial() {
		for (size_t i = 0; i < textures.size(); i++)
		{
			delete textures[i];
		}
	}
};

class ModelRender : public GLApplication
{
protected:
	Scene* scene;
	GLuint vao;
	GLuint vbo;

	Metrial* matrial;
public:

	void startup(int x, int y, int w, int h, const char* title) override {
		GLApplication::startup(x, y, w, h, title);
		scene = new Scene();
		scene->camera.position = vec3(0, 2, -10);
		scene->camera.fovy = 45;
		scene->camera.nearplan = 0.1;
		scene->camera.farplan = 1000;
		scene->camera.aspect = (float)w / (float)h;

		scene->AddModel(R"(C:\Users\Shawn\Downloads\jzb865er6v-IronMan\IronMan\IronMan.obj)");

		matrial = new Metrial({R"(D:\OpenGL\image.jpg)"},
			{
				{GL_VERTEX_SHADER,"D:/OpenGL/shaders/vertex.shader"},
				//{GL_TESS_CONTROL_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TCS.shader"},
				//{GL_TESS_EVALUATION_SHADER,"D:/Workspace/OpenGL/GL001/shaders/TES.shader"},
				{GL_FRAGMENT_SHADER,"D:/OpenGL/shaders/fragment.shader"},
			});

		glCreateVertexArrays(1, &vao);
	}

	virtual void onRender(double time) override {

	}

	virtual void onViewport(int w,int h) override {
		GLApplication::onViewport(w, h);
		scene->camera.aspect = (float)w / (float)h;
	}
};

