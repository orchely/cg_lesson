#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// sb6
#include <object.h>
#include <shader.h>

GLuint clear_program;
GLuint append_program;
GLuint resolve_program;

struct texture {
	GLuint color;
	GLuint normals;
} textures;

struct uniform_block {
	glm::mat4 mv_matrix;
	glm::mat4 view_matrix;
	glm::mat4 proj_matrix;
};

GLuint uniforms_buffer;

struct uniform {
	GLint mvp;
} uniforms;

sb6::object object;

GLuint list_item_buffer;
GLuint head_pointer_image;
GLuint atomic_counter_buffer;
GLuint dummy_vao;

void load_shaders()
{
	GLuint shaders[2];

	shaders[0] = sb6::shader::load("clear.vs.glsl", GL_VERTEX_SHADER, true);
	shaders[1] = sb6::shader::load("clear.fs.glsl", GL_FRAGMENT_SHADER, true);
	clear_program = sb6::program::link_from_shaders(shaders, 2, true);
	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	shaders[0] = sb6::shader::load("append.vs.glsl", GL_VERTEX_SHADER, true);
	shaders[1] = sb6::shader::load("append.fs.glsl", GL_FRAGMENT_SHADER, true);
	append_program = sb6::program::link_from_shaders(shaders, 2, true);
	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	uniforms.mvp = glGetUniformLocation(append_program, "mvp");

	shaders[0] = sb6::shader::load("resolve.vs.glsl", GL_VERTEX_SHADER, true);
	shaders[1] = sb6::shader::load("resolve.fs.glsl", GL_FRAGMENT_SHADER, true);
	resolve_program = sb6::program::link_from_shaders(shaders, 2, true);
	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);
}

void startup()
{
	load_shaders();

	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniform_block), nullptr, GL_DYNAMIC_DRAW);

	object.load("../media/objects/dragon.sbm");

	glGenBuffers(1, &list_item_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, list_item_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024 * 1024 * 16, nullptr, GL_DYNAMIC_COPY);

	glGenBuffers(1, &atomic_counter_buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, 4, nullptr, GL_DYNAMIC_COPY);

	glGenTextures(1, &head_pointer_image);
	glBindTexture(GL_TEXTURE_2D, head_pointer_image);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 1024, 1024);

	glGenVertexArrays(1, &dummy_vao);
	glBindVertexArray(dummy_vao);
}

float angle_to_rad(float angle)
{
	return angle / 180.0f * static_cast<float>(M_PI);
}

void render(double currentTime)
{
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	glUseProgram(clear_program);
	glBindVertexArray(dummy_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	float f = static_cast<float>(currentTime);
	glm::mat4 model_matrix = glm::scale(glm::mat4(), glm::vec3(7.0f));
	glm::vec3 view_position = glm::vec3(
		cosf(f * 0.35f) * 120.0f,
		cosf(f * 0.40f) * 30.0f,
		sinf(f * 0.35f) * 120.0f);
	glm::mat4 view_matrix = glm::lookAt(
		view_position,
		glm::vec3(0.0f, 30.0f, 0.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj_matrix = glm::perspective(
		angle_to_rad(50.0f),
		static_cast<float>(width) / static_cast<float>(height),
		0.1f,
		1000.0f);
	glm::mat4 mvp_matrix = proj_matrix * view_matrix * model_matrix;

	glUseProgram(append_program);
	glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

	static const unsigned int zero = 0;
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomic_counter_buffer);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(zero), &zero);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, list_item_buffer);

	glBindImageTexture(0, head_pointer_image, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	object.render();

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	glUseProgram(resolve_program);

	glBindVertexArray(dummy_vao);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void on_resize(int width, int height)
{
}

void shutdown()
{
	glDeleteVertexArrays(1, &dummy_vao);
	glDeleteTextures(1, &head_pointer_image);
	glDeleteBuffers(1, &atomic_counter_buffer);
	glDeleteBuffers(1, &list_item_buffer);
	glDeleteBuffers(1, &uniforms_buffer);
	glDeleteProgram(resolve_program);
	glDeleteProgram(append_program);
	glDeleteProgram(clear_program);
}