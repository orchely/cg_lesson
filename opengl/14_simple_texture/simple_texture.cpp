// original code: https://github.com/openglsuperbible/sb6code/blob/master/src/simpletexture/simpletexture.cpp
/*
* Copyright � 2012-2013 Graham Sellers
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

#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint program;
GLuint texture;
GLuint vao;

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core													\n"
		"																	\n"
		"void main(void)													\n"
		"{																	\n"
		"	const vec4 vertices[] = vec4[](vec4( 1.00, -1.00, 0.5, 1.0),	\n"
		"								   vec4(-1.00, -1.00, 0.5, 1.0),	\n"
		"								   vec4( 1.00,  1.00, 0.5, 1.0),	\n"
		"								   vec4( 1.00,  1.00, 0.5, 1.0),	\n"
		"								   vec4(-1.00, -1.00, 0.5, 1.0),	\n"
		"								   vec4(-1.00,  1.00, 0.5, 1.0));	\n"
		"	gl_Position = vertices[gl_VertexID];							\n"
		"}																	\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core							\n"
		"											\n"
		"uniform sampler2D s;						\n"
		"											\n"
		"out vec4 color;							\n"
		"											\n"
		"void main(void)							\n"
		"{											\n"
		"	int x = int(gl_FragCoord.x);			\n"
		"	int y = int(gl_FragCoord.y);			\n"
		"	ivec2 pos = ivec2(x % 256, y % 256);	\n"
		"	color = texelFetch(s, pos, 0);			\n"
		"}											\n"
	};

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}

void generate_texture(float * data, int width, int height)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			data[(y * width + x) * 4 + 0] = static_cast<float>(((x & y) & 0xFF) / 255.0f);
			data[(y * width + x) * 4 + 1] = static_cast<float>(((x | y) & 0xFF) / 255.0f);
			data[(y * width + x) * 4 + 2] = static_cast<float>(((x ^ y) & 0xFF) / 255.0f);
			data[(y * width + x) * 4 + 3] = 1.0f;
		}
	}
}

void startup()
{
	program = compile_shaders();

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexStorage2D(
		GL_TEXTURE_2D,	// 2D texture
		8,				// 8 mipmap levels
		GL_RGBA32F,		// 32-bit floating-point RGBA data
		256, 256);		// 256 x 256 texture

	float *data = new float[256 * 256 * 4];
	generate_texture(data, 256, 256);

	glTexSubImage2D(
		GL_TEXTURE_2D,	// 2D texture
		0,				// level 0
		0, 0,			// offset 0, 0
		256, 256,		// 256 x 256 texels, replace entire image
		GL_RGBA,		// four channel data
		GL_FLOAT,		// floating point data
		data);			// pointer to data

	delete [] data;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void render(double currentTime)
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	glUseProgram(program);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void on_resize(int width, int height)
{
}

void shutdown()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &texture);
	glDeleteProgram(program);
}