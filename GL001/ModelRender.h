#pragma once
#include"GLApplication.h"
#include"GraphicEngine.h"
class ModelRender : public GLApplication
{
protected:
	Scene* scene;
	GLuint vao;
	GLuint vbo;
	GLuint veo;

	Material* matrial;
	Model* model;
public:

	void startup(int x, int y, int w, int h, const char* title) override {
		GLApplication::startup(x, y, w, h, title);
		scene = new Scene();
		scene->camera.position = vec3(0, 0, -10);
		scene->camera.fovy = 45;
		scene->camera.nearplan = 0.1;
		scene->camera.farplan = 1000;
		scene->camera.aspect = (float)w / (float)h;

		model = scene->AddModel("D:/OpenGL/models/lowpolydeer/deer.obj",0.001f);
		model->position = vec3(0, 0, 0);
		//for (int i = 0; i < model->indicesNum; i++) {
		//	if (model->indices[i] == IDGAP) {
		//		cout << "#" << endl; continue;
		//	}
		//	cout << model->indices[i] << ",";
		//}

		matrial = Material::Create("D:/OpenGL/materials/IronMan/");

		glCreateVertexArrays(1, &vao);
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, 1024 * 1024 * 32, nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);

		glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));



		int offset = 0;
		glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, false, 0);
		glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, false, 16);
		glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, false, 32);
		glVertexArrayAttribFormat(vao, 3, 3, GL_FLOAT, false, 44);
		glVertexArrayAttribFormat(vao, 4, 2, GL_FLOAT, false, 56);

		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribBinding(vao, 1, 0);
		glVertexArrayAttribBinding(vao, 2, 0);
		glVertexArrayAttribBinding(vao, 3, 0);
		glVertexArrayAttribBinding(vao, 4, 0);

		glEnableVertexArrayAttrib(vao, 0);
		glEnableVertexArrayAttrib(vao, 1);
		glEnableVertexArrayAttrib(vao, 2);
		glEnableVertexArrayAttrib(vao, 3);
		glEnableVertexArrayAttrib(vao, 4);

		glCreateBuffers(1, &veo);
		glNamedBufferStorage(veo, 1024 *1024* 8, nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veo);

		glVertexArrayElementBuffer(vao, veo);
	}

	virtual void shutdown() override {
		GLApplication::shutdown();
		delete matrial;
		delete scene;
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &veo);
		glDeleteVertexArrays(1, &vao);
	}

	virtual void onRender(double time, double delta) override {
		glBindVertexArray(vao);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		matrial->Use();
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(IDGAP);
		//model->rotation[1] += 0.1;
		static float rY = 0;
		scene->camera.LookAt(model->position, vec3(10,rY+=0.1,0), 5);

		Vertex* pointer = (Vertex*)glMapNamedBuffer(vbo, GL_WRITE_ONLY);
		Vertex tmpV;
		tmpV.color = vec4(1,1,1,1);
		tmpV.position = vec4(0,0,0,1);
		float min = -20;
		float max = 20;
		float step = 0.5;
		int num = 0;
		for (float i =min; i < max; i+=step) {
			tmpV.position[0] = i;
			tmpV.position[2] = min;
			memcpy(&pointer[num++], &tmpV, sizeof(Vertex));
			tmpV.position[2] = max;
			memcpy(&pointer[num++], &tmpV, sizeof(Vertex));

			tmpV.position[2] = i;
			tmpV.position[0] = min;
			memcpy(&pointer[num++], &tmpV, sizeof(Vertex));
			tmpV.position[0] = max;
			memcpy(&pointer[num++], &tmpV, sizeof(Vertex));
		}
		glUnmapNamedBuffer(vbo);

		mat4 mvp = scene->camera.projectionMatrix() * scene->camera.viewMatrix();
		glUniformMatrix4fv(0, 1, GL_FALSE, mvp);
		glDrawArrays(GL_LINES, 0, num -1);

		for (const auto* m : scene->models) {
			glNamedBufferSubData(vbo, 0, sizeof(Vertex) * m->verticesNum, m->vertices);
			glNamedBufferSubData(veo, 0, m->indicesNum * 4, m->indices);
			mvp = scene->camera.projectionMatrix() * scene->camera.viewMatrix() * m->worldMatrix();

			glUniformMatrix4fv(0, 1, GL_FALSE, mvp);


			glDrawElements(GL_TRIANGLES, m->indicesNum, GL_UNSIGNED_INT, 0);
		}
		//glBindVertexArray(vao);
		//auto vSize =sizeof(Vertex);
		//Vertex v1, v2, v3;
		//v1.position = vec4(0, 0.5, 0.5, 1);
		//v1.color = vec4(1, 1, 1, 1);

		//v2.position = vec4(-0.5, -0.5, 0.5, 1);
		//v2.color = vec4(0, 1, 1, 1);

		//v3.position = vec4(0.5, -0.5, 0.5, 1);
		//v3.color = vec4(1, 0, 1, 1);
		//Vertex vs[3]{
		//	v1,v2,v3
		//};
		//glNamedBufferSubData(vbo,0, vSize * 3,vs);
		//glPointSize(10);
		//glDrawArrays(GL_TRIANGLES, 0, model->verticesNum);

		//unsigned int* tmpIdx = new unsigned int[model->verticesNum];
		//for (int i = 0; i < model->verticesNum; i++) {
		//	tmpIdx[i] = i;
		//}
		//glNamedBufferSubData(veo, 0, sizeof(unsigned int) * model->verticesNum, tmpIdx);
		//glDrawElements(GL_TRIANGLES, model->verticesNum, GL_UNSIGNED_INT, 0);
	}

	virtual void onViewport(int w,int h) override {
		GLApplication::onViewport(w, h);
		scene->camera.aspect = (float)w / (float)h;
	}
};

