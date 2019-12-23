#include "GLApplication.h"
#include "GraphicEngine.h"

class TessellationRender : public GLApplication {
protected:
	Camera mainCamera;
	Material* metrial;

	GLuint vbo;
public:
	virtual void startup(int x, int y, int w, int h, const char* title) override {
		GLApplication::startup(x, y, w, h, title);
		metrial = Material::Create("D:/OpenGL/materials/tessellation_bezier/");
		mainCamera.fovy = 45;
		mainCamera.nearplan = 0.01;
		mainCamera.farplan = 1000;
		mainCamera.aspect = (float)w / h;

		glCreateBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferStorage(GL_ARRAY_BUFFER, 1024, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), 0);
		glEnableVertexAttribArray(0);
	}

	virtual void onRender(double time, double delta) override {
		static float yRote = 0;
		metrial->Use();
		mainCamera.LookAt(vec3(0), vec3(20, yRote+=0.01, 0), 3);
		mat4 mvp = mainCamera.vp();
		glUniformMatrix4fv(0, 1, GL_FALSE, mvp);

		vec4 quad[]{
			vec4(-1,1,-1,1),	vec4(-0.5,-1,-1,1),	vec4(0,0,-1,1),		vec4(0.5,2,-1,1),
			vec4(-1,0,-0.5,1),	vec4(-0.5,0,-0.5,1),vec4(0,0,-0.5,1),	vec4(0.5,0,-0.5,1),
			vec4(-1,0,0,1),		vec4(-0.5,0,0,1),	vec4(0,0,0,1),		vec4(0.5,0,0,1),
			vec4(-1,0,0.5,1),	vec4(-0.5,1,0.5,1),	vec4(0,-1,0.5,1),	vec4(0.5,0,0.5,1),
		};
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glNamedBufferSubData(vbo, 0, sizeof(quad), quad);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_PATCHES, 0,16);
		//glDrawArrays(GL_TRIANGLES, 0,3);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	virtual void onViewport(int w, int h) override {
		mainCamera.aspect = (float)w / (float)h;
	}

	virtual void shutdown() override {
		delete metrial;
	}
};

GLApplication* createTessllaton() {
	return new TessellationRender();
}