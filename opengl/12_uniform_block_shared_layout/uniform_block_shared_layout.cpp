#include <math.h>
#include <sstream>
#include <ostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint rendering_program;
GLuint vertex_array_object;
GLuint uniform_buffer;

glm::mat4 model_view_matrix;
glm::mat4 normal_matrix;
glm::mat4 projection_matrix;

GLint model_view_matrix_location;
GLint normal_matrix_location;
GLint projection_matrix_location;

void checkGlError(const char *file, int line)
{
	bool failed = false;
	for (GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
		std::ostringstream oss;
		oss << "glError: " << reinterpret_cast<const char*>(gluErrorString(error)) << " caught at " << file << ":" << line << std::endl;
		OutputDebugStringA(oss.str().c_str());
		failed = true;
	}
	if (failed) {
		exit(1);
	}
}

void checkCompileError(const char *file, int line, GLuint shader)
{
	GLint compile_result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);
	if (compile_result != GL_TRUE) {
		std::ostringstream oss;
		oss << "compile error caught at " << file << "(" << line << ")" << std::endl;
		OutputDebugStringA(oss.str().c_str());
		GLint size;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
		std::vector<char> v(size);
		int length;
		glGetShaderInfoLog(shader, static_cast<GLsizei>(v.size()), &length, v.data());
		OutputDebugStringA(v.data());
		exit(1);
	}
}

void checkLinkError(const char *file, int line, GLuint program)
{
	GLint link_result;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result != GL_TRUE) {
		std::ostringstream oss;
		oss << "link error caught at " << file << "(" << line << ")" <<std::endl;
		OutputDebugStringA(oss.str().c_str());
		GLint size;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);
		std::vector<char> v(size);
		int length;
		glGetProgramInfoLog(program, static_cast<GLsizei>(v.size()), &length, v.data());
		OutputDebugStringA(v.data());
		exit(1);
	}
}

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core																		\n"
		"																						\n"
		"in vec3 vertex;																		\n"
		"in vec3 normal;																		\n"
		"																						\n"
		"uniform mat4 model_view_matrix;														\n"
		"uniform mat4 normal_matrix;															\n"
		"uniform mat4 projection_matrix;														\n"
		"																						\n"
		"out VS_OUT																				\n"
		"{																						\n"
		"	float NdotL;																		\n"
		"	vec3 ReflectVec;																	\n"
		"	vec3 ViewVec;																		\n"
		"} vs_out;																				\n"
		"																						\n"
		"void main(void)																		\n"
		"{																						\n"
		"	vec3 LightPosition	= vec3(0.0f, 10.0f, 4.0f);										\n"
		"	vec3 ecPos			= vec3(model_view_matrix * vec4(vertex, 1.0f));					\n"
		"	vec3 tnorm			= normalize(vec3(normal_matrix * vec4(normal, 1.0f)));			\n"
		"	vec3 lightVec		= normalize(LightPosition - ecPos);								\n"
		"	vs_out.ReflectVec	= normalize(reflect(-lightVec, tnorm));							\n"
		"	vs_out.ViewVec		= normalize(-ecPos);											\n"
		"	vs_out.NdotL		= (dot(lightVec, tnorm) + 1.0f) * 0.5f;							\n"
		"	gl_Position			= projection_matrix * model_view_matrix * vec4(vertex, 1.0f);	\n"
		"}																						\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core																			\n"
		"																							\n"
		"uniform ColorBlock																			\n"
		"{																							\n"
		"	float DiffuseCool;																		\n"
		"	float DiffuseWarm;																		\n"
		"	vec3  SurfaceColor;																		\n"
		"	vec3  WarmColor;																		\n"
		"	vec3  CoolColor;																		\n"
		"} colors;																					\n"
		"																							\n"
		"in VS_OUT																					\n"
		"{																							\n"
		"	float NdotL;																			\n"
		"	vec3 ReflectVec;																		\n"
		"	vec3 ViewVec;																			\n"
		"} fs_in;																					\n"
		"																							\n"
		"out vec4 color;																			\n"
		"																							\n"
		"void main(void)																			\n"
		"{																							\n"
		"	vec3 kcool    = min(colors.CoolColor + colors.DiffuseCool * colors.SurfaceColor, 1.0f);	\n"
		"	vec3 kwarm    = min(colors.WarmColor + colors.DiffuseWarm * colors.SurfaceColor, 1.0f);	\n"
		"	vec3 kfinal   = mix(kcool, kwarm, fs_in.NdotL);											\n"
		"																							\n"
		"	vec3 nreflect = normalize(fs_in.ReflectVec);											\n"
		"	vec3 nview    = normalize(fs_in.ViewVec);												\n"
		"																							\n"
		"	float spec    = max(dot(nreflect, nview), 0.0f);										\n"
		"	spec          = pow(spec, 32.0f);														\n"
		"																							\n"
		"	color		 = vec4(min(kfinal + spec, 1.0f), 1.0f);									\n"
		"}																							\n"
	};

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);
	checkCompileError(__FILE__, __LINE__, vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	checkCompileError(__FILE__, __LINE__, fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	checkLinkError(__FILE__, __LINE__, program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	checkGlError(__FILE__, __LINE__);

	return program;
}

void build_uniform_block()
{
	GLuint uniform_block_index = glGetUniformBlockIndex(rendering_program, "ColorBlock");
	checkGlError(__FILE__, __LINE__);

	const static GLchar* names[] = {
		"ColorBlock.SurfaceColor",
		"ColorBlock.WarmColor",
		"ColorBlock.CoolColor",
		"ColorBlock.DiffuseWarm",
		"ColorBlock.DiffuseCool"
	};

	GLuint indies[5];
	glGetUniformIndices(rendering_program, 5, names, indies);

	GLint offsets[5];
	glGetActiveUniformsiv(rendering_program, 5, indies, GL_UNIFORM_OFFSET, offsets);

	GLsizei uniform_block_size;
	glGetActiveUniformBlockiv(rendering_program, uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniform_block_size);
	checkGlError(__FILE__, __LINE__);

	uint8_t *colors = new uint8_t[uniform_block_size];

	GLfloat *surface_color = reinterpret_cast<GLfloat*>(colors + offsets[0]);
	surface_color[0] = 0.45f; // r
	surface_color[1] = 0.45f; // g
	surface_color[2] = 1.00f; // b
	GLfloat *warm_color = reinterpret_cast<GLfloat*>(colors + offsets[1]);
	warm_color[0] = 0.75f; // r
	warm_color[1] = 0.75f; // g
	warm_color[2] = 0.75f; // b
	GLfloat *cool_color = reinterpret_cast<GLfloat*>(colors + offsets[2]);
	cool_color[0] = 0.00f; // r
	cool_color[1] = 0.00f; // g
	cool_color[2] = 1.00f; // b
	*reinterpret_cast<GLfloat*>(colors + offsets[3]) = 0.45f; // DiffuseWarm
	*reinterpret_cast<GLfloat*>(colors + offsets[4]) = 0.45f; // DiffuseCool

	glGenBuffers(1, &uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
	glBufferData(GL_UNIFORM_BUFFER, uniform_block_size, colors, GL_DYNAMIC_DRAW);
	checkGlError(__FILE__, __LINE__);

	delete [] colors;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);
	glUniformBlockBinding(rendering_program, uniform_block_index, 0);
	checkGlError(__FILE__, __LINE__);
}

void startup(void)
{
	float rotate_x = 0.0f;
	float rotate_y = 0.0f;
	glm::mat4 view_translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
	glm::mat4 view_rotate_x = glm::rotate(view_translate, rotate_y, glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 view = glm::rotate(view_rotate_x, rotate_x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	model_view_matrix = view * model;
	normal_matrix = glm::transpose(glm::inverse(model_view_matrix));
	projection_matrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

	rendering_program = compile_shaders();
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
	checkGlError(__FILE__, __LINE__);

	GLint vertex_location = glGetAttribLocation(rendering_program, "vertex");
	glutSetVertexAttribCoord3(vertex_location);
	checkGlError(__FILE__, __LINE__);
	GLint normal_location = glGetAttribLocation(rendering_program, "normal");
	glutSetVertexAttribNormal(normal_location);
	checkGlError(__FILE__, __LINE__);

	model_view_matrix_location = glGetUniformLocation(rendering_program, "model_view_matrix");
	normal_matrix_location = glGetUniformLocation(rendering_program, "normal_matrix");
	projection_matrix_location = glGetUniformLocation(rendering_program, "projection_matrix");
	checkGlError(__FILE__, __LINE__);

	build_uniform_block();

	glViewport(0, 0, 800, 600);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

// Our rendering function
void render(double currentTime)
{
	const GLfloat bg_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, bg_color);

	GLfloat one = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &one);
	checkGlError(__FILE__, __LINE__);

	glUseProgram(rendering_program);

	glUniformMatrix4fv(model_view_matrix_location, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
	glUniformMatrix4fv(normal_matrix_location, 1, GL_FALSE, glm::value_ptr(normal_matrix));
	glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	checkGlError(__FILE__, __LINE__);

	//the teapot winds backwards
	glutSolidTeapot(1.33);
}

void on_resize(int width, int height)
{
}

void shutdown(void)
{
	glDeleteProgram(rendering_program);
	glDeleteBuffers(1, &uniform_buffer);
	glDeleteVertexArrays(1, &vertex_array_object);
}