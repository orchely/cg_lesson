// original code: https://github.com/openglsuperbible/sb6code/blob/master/src/alienrain/alienrain.cpp
/*
* Copyright © 2012-2013 Graham Sellers
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

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sb6ktx.h>
#include <glm/vec4.hpp>

GLuint rendering_program;
GLuint vertex_array_object;
GLuint tex_alien_array;
GLuint rain_buffer;
GLfloat droplet_x_offset[256];
GLfloat droplet_rot_speed[256];
GLfloat droplet_fall_speed[256];

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core															\n"
		"																			\n"
		"layout (location = 0) in int alien_index;									\n"
		"																			\n"
		"out VS_OUT																	\n"
		"{																			\n"
		"	flat int alien;															\n"
		"	vec2 tc;																\n"
		"} vs_out;																	\n"
		"																			\n"
		"struct droplet_t															\n"
		"{																			\n"
		"	float x_offset;															\n"
		"	float y_offset;															\n"
		"	float orientation;														\n"
		"	float unused;															\n"
		"};																			\n"
		"																			\n"
		"layout (std140) uniform droplets											\n"
		"{																			\n"
		"	droplet_t droplet[256];													\n"
		"};																			\n"
		"																			\n"
		"void main(void)															\n"
		"{																			\n"
		"	const vec2[4] position = vec2[4](vec2(-0.5f, -0.5f),					\n"
		"									 vec2( 0.5f, -0.5f),					\n"
		"									 vec2(-0.5f,  0.5f),					\n"
		"									 vec2( 0.5f,  0.5f));					\n"
		"   // calculate texture coordinate	(move the vertex into (0,0)-(1,1))		\n"
		"	vs_out.tc = position[gl_VertexID].xy + vec2(0.5f);						\n"
		"   // build rotation matrix                                                \n"
		"	float co = cos(droplet[alien_index].orientation);						\n"
		"	float so = sin(droplet[alien_index].orientation);						\n"
		"	mat2 rot = mat2(vec2( co, so),											\n"
		"					vec2(-so, co));											\n"
		"   // rotate the vertex and shrink											\n"
		"	vec2 pos = 0.25f * rot * position[gl_VertexID];							\n"
		"   // move the vetex to each alien's postion from origin					\n"
		"	gl_Position = vec4(pos.x + droplet[alien_index].x_offset,				\n"
		"					   pos.y + droplet[alien_index].y_offset,				\n"
		"					   0.5f, 1.0f);											\n"
		"	// choose texture														\n"
		"	vs_out.alien = alien_index % 64;										\n"
		"}																			\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core															\n"
		"																			\n"
		"layout (location = 0) out vec4 color;										\n"
		"																			\n"
		"in VS_OUT																	\n"
		"{																			\n"
		"	flat int alien;															\n"
		"	vec2 tc;																\n"
		"} fs_in;																	\n"
		"																			\n"
		"uniform sampler2DArray tex_aliens;											\n"
		"																			\n"
		"void main(void)															\n"
		"{																			\n"
		"	color = texture(tex_aliens, vec3(fs_in.tc, float(fs_in.alien)));		\n"
		"}																			\n"
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

float random_float()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void startup(void)
{
	rendering_program = compile_shaders();

	tex_alien_array = sb6::ktx::file::load("../media/textures/aliens.ktx");
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex_alien_array);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenBuffers(1, &rain_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, rain_buffer);
	glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

	for (int i = 0; i < 256; i++) {
		droplet_x_offset[i] = random_float() * 2.0f - 1.0f;
		droplet_rot_speed[i] = (random_float() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
		droplet_fall_speed[i] = random_float() + 0.2f;
	}

	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void render(double currentTime)
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	const static GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, black);
	
	glUseProgram(rendering_program);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, rain_buffer);
	glm::vec4 *droplet = reinterpret_cast<glm::vec4*>(glMapBufferRange(GL_UNIFORM_BUFFER, 0, 256 * sizeof(glm::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	float t = static_cast<float>(currentTime);
	for (int i = 0; i < 256; i++) {
		droplet[i][0] = droplet_x_offset[i];
		droplet[i][1] = 2.0f - fmodf((t + static_cast<float>(i)) * droplet_fall_speed[i], 4.31f);
		droplet[i][2] = t * droplet_rot_speed[i];
		droplet[i][3] = 0.0f;
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	for (int alien_index = 0; alien_index < 256; alien_index++) {
		glVertexAttribI1i(0, alien_index);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void on_resize(int width, int height)
{
}

void shutdown(void)
{
	glDeleteBuffers(1, &rain_buffer);
	glDeleteTextures(1, &tex_alien_array);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);
}