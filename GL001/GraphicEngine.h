#pragma once
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <../vmath.h>
//#include <climits>
#include <vector>
#include <map>

#include "Helper.h"

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


inline vmath::mat4 ZxyRote(const vec3& r) {
	return vmath::rotate(r[1], vec3(0, 1, 0)) * vmath::rotate(r[0], vec3(1, 0, 0)) * vmath::rotate(r[2], vec3(0, 0, 1));
}

inline vmath::mat4 YxzRote(const vec3& r) {
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

	vmath::mat4 worldMatrix() const {
		return vmath::translate(position)
			* ZxyRote(rotation)
			* vmath::scale(scale[0], scale[1], scale[2])
			;
	}

	Model(aiMesh** meshs, int meshNum, vec3 pos, vec3 rot, vec3 scale, float unit = 1.0) {
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
				indicesNum += v.mNumIndices;
			}
		}
		indicesNum += meshNum;
		vertices = new Vertex[verticesNum];
		indices = new unsigned int[indicesNum];

		size_t iv = 0;
		size_t idx = 0;
		int voffset = 0;
		for (int i = 0; i < meshNum; i++) {
			aiMesh* m = meshs[i];
			for (int p = 0; p < m->mNumVertices; p++) {
				auto v = m->mVertices[p] * unit;
				auto n = m->mNormals != nullptr ? m->mNormals[p] : aiVector3D(0);
				auto uv = m->mTextureCoords[0] != nullptr ? m->mTextureCoords[0][p] : aiVector3D(0, 0, 0);
				auto tan = m->mTangents != nullptr ? m->mTangents[p] : aiVector3D();
				auto color = m->mColors[0] != nullptr ? m->mColors[0][p] : aiColor4D(1, 1, 1, 1);
				vertices[iv].position = vec4(v.x, v.y, v.z, 1);
				vertices[iv].color = vec4(color.r, color.g, color.b, color.a);
				vertices[iv].uv = vec2(uv.x, uv.y);
				vertices[iv].normal = vec3(n.x, n.y, n.z);
				vertices[iv].tangent = vec3(tan.x, tan.y, tan.z);
				iv++;
			}
			for (int j = 0; j < m->mNumFaces; j++) {
				aiFace v = m->mFaces[j];
				for (int ix = 0; ix < v.mNumIndices; ix++) {
					indices[idx++] = v.mIndices[ix] + voffset;
				}
			}
			indices[idx++] = IDGAP;

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

	void LookAt(vec3 pos, vec3 dir, float distance) {
		vec4 v4 = vec4(0, 0, -1, 0) * ZxyRote(dir).transpose();
		position = vec3(v4[0], v4[1], v4[2]) * distance + pos;
		rotation = dir;
	}

	mat4 viewMatrix() override {
		static mat4 zInvers = mat4::identity();
		zInvers[2][2] = -1;
		return zInvers * YxzRote(rotation * -1) * translate(position * -1);
	}

	mat4 projectionMatrix() override {
		return vmath::perspective(fovy, aspect, nearplan, farplan);
	}

	mat4 vp() {
		return projectionMatrix() * viewMatrix();
	}
};

class Light {

};

class Scene {
public:
	Camera camera;
	vector<Light*> lights;
	vector<Model*> models;

	Model* AddModel(const char* path, float unit) {
		Assimp::Importer ip;
		const aiScene* scene = ip.ReadFile(path, aiProcess_CalcTangentSpace |
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if (scene == nullptr) {
			cout << "load model: " << path << " error" << endl;
			return nullptr;
		}
		Model* model = new Model(scene->mMeshes, scene->mNumMeshes, vec3(0), vec3(0), vec3(1), unit);
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

	int Index;

	//filter:GL_<*FILTER>_MIPMAP_<*SELECTOR> LINEAR or NEAREST  wrap:
	Sampler2D(int idx, const char* path, GLuint minf, GLuint maxf, GLuint wrap, bool bMipmap) {
		Index = idx;
		auto data = stbi_load(path, &width, &height, &channel, 0);
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureSubImage2D(texture, 0, 0, 0, width, height, channel == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
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

class Material {
public:
	vector<Sampler2D*> textures;
	GLuint program;

	Material(map<int, string> txs, map<GLenum, string> shs, bool bSeparate = false) {
		GLuint* ss = new GLuint[shs.size()];
		GLuint* sspointer = ss;
		for (auto& e : shs) {
			*sspointer = CompileShaderFile(e.first, e.second.c_str());
			sspointer++;
		}
		program = LinkShaderProgram(false, shs.size(), ss);

		for (auto& tx : txs) {
			Sampler2D* sm = new Sampler2D(tx.first, tx.second.c_str(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, true);
			textures.push_back(sm);
		}
	}

	static Material* Create(const char* byDir, bool bSeparate = false) {
		static map<string, GLenum> stype{
			{"vertex",GL_VERTEX_SHADER},
			{"tcs",GL_TESS_CONTROL_SHADER},
			{"tes",GL_TESS_EVALUATION_SHADER},
			{"geometry",GL_GEOMETRY_SHADER},
			{"fragment",GL_FRAGMENT_SHADER}
		};

		static map<string, int> txidices{
			{"000",0},{"001",1},{"002",2},{"003",3},{"004",4},{"005",5},{"006",6},{"007",7},{"008",8},{"009",9},{"010",10},
		};

		vector<string> files;
		map<int, string> txs;
		map<GLenum, string> shs;
		Helper::SearchDir(byDir, files);
		for (const auto& path : files) {
			string name = Helper::GetNameFromPath(path, false);
			if (stype.find(name) != stype.end()) {
				shs.insert({ stype[name],path });
			}
			else if (txidices.find(name) != txidices.end()) {
				txs.insert({ txidices[name],path });
			}
		}
		return new Material(txs, shs, bSeparate);
	}

	virtual void Use() {
		glUseProgram(program);
		for (size_t i = 0; i < textures.size(); i++)
		{
			textures[i]->Bind(i);
		}
	}

	virtual ~Material() {
		for (size_t i = 0; i < textures.size(); i++)
		{
			delete textures[i];
		}
	}
};

