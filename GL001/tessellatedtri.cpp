/*
 * Copyright ?2012-2015 Graham Sellers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "GLApplication.h"

class tessellatedtri_app : public GLApplication
{
public:
	void startup(int x, int y, int w, int h, const char* title) override
    {
		GLApplication::startup(x, y, w, h, title);
        static const char * vs_source[] =
        {
            "#version 410 core                                                 \n"
            "                                                                  \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    const vec4 vertices[] = vec4[](vec4( 0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4(-0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4( 0.25,  0.25, 0.5, 1.0)); \n"
            "                                                                  \n"
            "    gl_Position = vertices[gl_VertexID];                          \n"
            "}                                                                 \n"
        };

        static const char * tcs_source[] =
        {
            "#version 410 core                                                                 \n"
            "                                                                                  \n"
            "layout (vertices = 3) out;                                                        \n"
            "                                                                                  \n"
            "void main(void)                                                                   \n"
            "{                                                                                 \n"
            "    if (gl_InvocationID == 0)                                                     \n"
            "    {                                                                             \n"
            "        gl_TessLevelInner[0] = 5.0;                                               \n"
            "        gl_TessLevelOuter[0] = 5.0;                                               \n"
            "        gl_TessLevelOuter[1] = 5.0;                                               \n"
            "        gl_TessLevelOuter[2] = 5.0;                                               \n"
            "    }                                                                             \n"
            "    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;     \n"
            "}                                                                                 \n"
        };

        static const char * tes_source[] =
        {
            "#version 410 core                                                                 \n"
            "                                                                                  \n"
            "layout (triangles, equal_spacing, cw) in;                                         \n"
            "                                                                                  \n"
            "void main(void)                                                                   \n"
            "{                                                                                 \n"
            "    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +                       \n"
            "                  (gl_TessCoord.y * gl_in[1].gl_Position) +                       \n"
            "                  (gl_TessCoord.z * gl_in[2].gl_Position);                        \n"
            "}                                                                                 \n"
        };

        static const char * fs_source[] =
        {
            "#version 410 core                                                 \n"
            "                                                                  \n"
            "out vec4 color;                                                   \n"
            "                                                                  \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    color = vec4(0.0, 0.8, 1.0, 1.0);                             \n"
            "}                                                                 \n"
        };

        program = glCreateProgram();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source, NULL);
        glCompileShader(vs);





        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, NULL);
        glCompileShader(fs);

		GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tes, 1, tes_source, NULL);
		glCompileShader(tes);

		GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tcs, 1, tcs_source, NULL);
		glCompileShader(tcs);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
		glAttachShader(program, tes);
		glAttachShader(program, tcs);

        glLinkProgram(program);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void onRender(double currentTime) override
    {
        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, green);

        glUseProgram(program);
        glDrawArrays(GL_PATCHES, 0, 3);
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
    }

private:
    GLuint          program;
    GLuint          vao;
};


//int main(int argc, char** argv) {
//
//	tessellatedtri_app app;
//	app.startup(0, 0, 600, 400, "firstApp");
//	app.run();
//	app.shutdown();
//	return 0;
//}