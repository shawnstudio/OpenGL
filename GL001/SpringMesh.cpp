#include "GLApplication.h"
#include "GraphicEngine.h"

class SpringMeshRender : public GLApplication {
protected:
	Material* physicMaterial;
	Material* renderMaterial;
	Camera mainCamera;

	GLuint vao[2];
	GLuint vbo[6];

	GLuint tbo[2];

	const int Idx_Position = 0;
	const int Idx_Velocity = 2;
	const int Idx_Conneted = 4;
	const int Idx_Element = 5;

	const float meshW = 5;
	const float meshH = 5;
	const float mass = 0.1f;
	const int NUM_X = 60;
	const int NUM_Z = 60;
	const int NUM_TOTAL = NUM_X * NUM_Z;
	const int LINE_NUM = (NUM_X - 1) * (NUM_Z - 1) * 4;


public:
	virtual void startup(int x, int y, int w, int h, const char* title) override {
		GLApplication::startup(x, y, w, h, title);
		physicMaterial = Material::Create("D:/OpenGL/materials/spring_physic/");
		renderMaterial = Material::Create("D:/OpenGL/materials/spring_render/");

		const char* names[] = { "out_position","out_velocity" };
		glTransformFeedbackVaryings(physicMaterial->program, 2, names, GL_SEPARATE_ATTRIBS);
		glLinkProgram(physicMaterial->program);

		mainCamera.fovy = 45;
		mainCamera.nearplan = 0.01;
		mainCamera.farplan = 1000;
		mainCamera.aspect = (float)w / h;

		glCreateVertexArrays(2, vao);
		glCreateBuffers(6, vbo);

		vec4* initial_poss = new vec4[NUM_TOTAL];
		vec3* initial_velocities = new vec3[NUM_TOTAL];
		ivec4* connects = new ivec4[NUM_TOTAL];
		unsigned int* idxs = new unsigned int[LINE_NUM];

		for (int z = 0; z < NUM_Z; z++) {
			float posZ = z * meshH / NUM_Z - meshH /2;
			for (int x = 0; x < NUM_X; x++) {
				int n = z * NUM_X + x;
				float posX = x * meshW / NUM_X - meshW /2;
				initial_poss[n] = vec4(posX, 0, posZ, mass);
				initial_velocities[n] = vec3(0);
				connects[n] = ivec4(z + 1 == NUM_Z ? -1 : (z + 1) * NUM_X + x,	//Top
					x == 0 ? -1 : x - 1 + z * NUM_X,							//Left
					z == 0 ? -1 : (z - 1) * NUM_X + x,							//Bottom
					x + 1 == NUM_X ? -1 : z * NUM_X + x + 1						//Right
				);
				if (z < NUM_Z - 1 && x < NUM_X - 1) {
					int idx = (z * (NUM_X - 1) + x)*4;
					idxs[idx] = n;
					idxs[idx + 1] = n + 1;
					idxs[idx + 2] = n;
					idxs[idx + 3] = n + NUM_X;
				}
			}
		}
		
		for (int i = 0; i < 2; i++) {
			glBindVertexArray(vao[i]);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[Idx_Position + i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * NUM_TOTAL, initial_poss,  GL_DYNAMIC_COPY);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,sizeof(vec4),0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[Idx_Velocity + i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * NUM_TOTAL, initial_velocities, GL_DYNAMIC_COPY);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[Idx_Conneted]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ivec4) * NUM_TOTAL, connects, GL_STATIC_DRAW);
			glVertexAttribIPointer(2, 4, GL_INT, sizeof(ivec4), 0);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[Idx_Element]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * LINE_NUM, idxs, GL_STATIC_DRAW);
		}

		//glCreateTextures(GL_TEXTURE_BUFFER, 2, tbo);
		glGenTextures(2, tbo);
		glBindTexture(GL_TEXTURE_BUFFER, tbo[0]);
		//glTextureBuffer(tbo[0], GL_RGBA32F, vbo[Idx_Position]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbo[Idx_Position]);

		glBindTexture(GL_TEXTURE_BUFFER, tbo[1]);
		//glTextureBuffer(tbo[1], GL_RGBA32F, vbo[Idx_Position+1]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbo[Idx_Position]);

		delete[] initial_poss;
		delete[] initial_velocities;
		delete[] connects;
		delete[] idxs;
	}

	virtual void onRender(double time,double delta) override {
		static int iteration_index = 0;
		const static int iteration_perframe = 1;
		physicMaterial->Use();

		static float gravity[]{ 0, -9.8, 0 };
		static float damping = 8;
		static float spring = 100;
		static float spring_rest = meshW / NUM_X;

		for (int i = 0; i < iteration_perframe; i++) {
			glBindVertexArray(vao[iteration_index & 1]);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTexture(GL_TEXTURE_BUFFER, tbo[iteration_index & 1]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo[iteration_index & 1]);
			iteration_index++;
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[Idx_Position + (iteration_index & 1)]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vbo[Idx_Velocity + (iteration_index & 1)]);

			glUniform3fv(0, 1, gravity);
			glUniform1f(1, damping);
			glUniform1f(2, spring);
			glUniform1f(3, delta / iteration_perframe);
			glUniform1f(4, spring_rest);

			glBeginTransformFeedback(GL_POINTS);
			glDrawArrays(GL_POINTS, 0, NUM_TOTAL);
			glEndTransformFeedback();
		}
		glDisable(GL_RASTERIZER_DISCARD);
		glBindVertexArray(vao[iteration_index & 1]);
		renderMaterial->Use();
		static float yRote = 0;
		mainCamera.LookAt(vec3(0), vec3(20, yRote +=0.1, 0), 5);
		mat4 mvp = mainCamera.vp();
		glUniformMatrix4fv(0, 1, GL_FALSE, mvp);
		glPointSize(2);
		glDrawArrays(GL_POINTS, 0, NUM_TOTAL);

		glDrawElements(GL_LINES, LINE_NUM, GL_UNSIGNED_INT, 0);

		//glDrawArrays(GL_LINES, 0, NUM_TOTAL);
	}

	virtual void onViewport(int w, int h) override {
		mainCamera.aspect = (float)w / (float)h;
	}

	virtual void shutdown() override {
		delete physicMaterial;
		delete renderMaterial;
	}
};

GLApplication* createSpring() {
	return new SpringMeshRender();
}