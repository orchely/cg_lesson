#include <math.h>
#include <sstream>
#include <ostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

GLuint rendering_program;
GLuint vertex_array_object;
GLuint uniform_buffer = UINT_MAX;
GLint offset = -1;
GLint single_size = -1;

void glError(const char *file, int line)
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

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core																		\n"
		"																						\n"
		"uniform mat4 model_view_matrix;														\n"
		"uniform mat4 normal_matrix;															\n"
		"uniform mat4 projection_matrix;														\n"
		"																						\n"
		"in vec3 vertex;																		\n"
		"in vec3 normal;																		\n"
		"																						\n"
		"out VS_OUT																				\n"
		"{																						\n"
		"	float NdotL;																		\n"
		"	vec4 ReflectVec;																	\n"
		"	vec4 ViewVec;																		\n"
		"} vs_out;																				\n"
		"																						\n"
		"void main(void)																		\n"
		"{																						\n"
		"	vec4 vertex4		= vec4(vertex, 1.0f);											\n"
		//"	vec4 normal4		= vec4(normal, 1.0f);											\n"
		//"	vec4 LightPosition	= vec4(0.0f, 10.0f, 4.0f, 1.0f);								\n"
		//"	vec4 ecPos			= model_view_matrix * vertex4;									\n"
		//"	vec4 tnorm			= normalize(normal_matrix * normal4);							\n"
		//"	vec4 lightVec		= normalize(LightPosition - ecPos);								\n"
		//"	vs_out.ReflectVec	= normalize(reflect(-lightVec, tnorm));							\n"
		//"	vs_out.ViewVec		= normalize(-ecPos);											\n"
		//"	vs_out.NdotL		= (dot(lightVec, tnorm) + 1.0f) * 0.5f;							\n"
		"	gl_Position			= projection_matrix * model_view_matrix * vertex4;				\n"
		"}																						\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core																			\n"
		"																							\n"
		"layout(std140) uniform ColorBlock															\n"
		"{																							\n"
		"	float DiffuseCool;																		\n"
		"	float DiffuseWarm;																		\n"
		"	vec4  SurfaceColor;																		\n"
		"	vec4  WarmColor;																		\n"
		"	vec4  CoolColor;																		\n"
		"} colors;																					\n"
		"																							\n"
		"in VS_OUT																					\n"
		"{																							\n"
		"	float NdotL;																			\n"
		"	vec4 ReflectVec;																		\n"
		"	vec4 ViewVec;																			\n"
		"} fs_in;																					\n"
		"																							\n"
		"out vec4 color;																			\n"
		"																							\n"
		"void main(void)																			\n"
		"{																							\n"
		//"	vec4 kcool    = min(colors.CoolColor + colors.DiffuseCool * colors.SurfaceColor, 1.0f);	\n"
		//"	vec4 kwarm    = min(colors.WarmColor + colors.DiffuseWarm * colors.SurfaceColor, 1.0f); \n"
		//"	vec4 kfinal   = mix(kcool, kwarm, fs_in.NdotL);											\n"
		//"																							\n"
		//"	vec4 nreflect = normalize(fs_in.ReflectVec);											\n"
		//"	vec4 nview    = normalize(fs_in.ViewVec);												\n"
		//"																							\n"
		//"	float spec    = max(dot(nreflect, nview), 0.0f);										\n"
		//"	spec          = pow(spec, 32.0f);														\n"
		//"																							\n"
		"	color		 = vec4(1.0, 0.0, 0.0, 1.0);//min(kfinal + spec, 1.0f);										\n"
		"}																							\n"
	};

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);
	glError(__FILE__, __LINE__);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	glError(__FILE__, __LINE__);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glError(__FILE__, __LINE__);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glError(__FILE__, __LINE__);

	return program;
}

void startup(void)
{
	while (glGetError() != GL_NO_ERROR)
		;

	rendering_program = compile_shaders();
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
	glError(__FILE__, __LINE__);

	GLint vertex_location = glGetAttribLocation(rendering_program, "vertex");
	glutSetVertexAttribCoord3(vertex_location);
	glError(__FILE__, __LINE__);
	GLint normal_location = glGetAttribLocation(rendering_program, "normal");
	glutSetVertexAttribNormal(normal_location);
	glError(__FILE__, __LINE__);

	float rotate_x = 0.0f;
	float rotate_y = 0.0f;
	glm::mat4 projection = glm::perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 view_translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
	glm::mat4 view_rotate_x = glm::rotate(view_translate, rotate_y, glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 view = glm::rotate(view_rotate_x, rotate_x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	glm::mat4 model_view = view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(model_view));

	GLint model_view_matrix_location = glGetUniformLocation(rendering_program, "model_view_matrix");
	glUniformMatrix4fv(model_view_matrix_location, 1, GL_FALSE, glm::value_ptr(model_view));
	glError(__FILE__, __LINE__);
	GLint normal_matrix_location = glGetUniformLocation(rendering_program, "normal_matrix");
	glUniformMatrix4fv(normal_matrix_location, 1, GL_FALSE, glm::value_ptr(normal));
	glError(__FILE__, __LINE__);
	GLint projection_matrix_location = glGetUniformLocation(rendering_program, "projection_matrix");
	glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, glm::value_ptr(projection));
	glError(__FILE__, __LINE__);

	// Update the uniforms using ARB_uniform_buffer_object
	glGenBuffers(1, &uniform_buffer);

	// There's only one uniform block here, the 'colors0' uniform block. 
	// It contains the color info for the gooch shader.
	GLuint uniform_block_index = glGetUniformBlockIndex(rendering_program, "ColorBlock");
	glError(__FILE__, __LINE__);

	// We need to get the uniform block's size in order to back it with the
	// appropriate buffer
	GLsizei uniform_block_size;
	glGetActiveUniformBlockiv(rendering_program, uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniform_block_size);
	glError(__FILE__, __LINE__);

	// Now we attach the buffer to UBO binding point 0...
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);
	// And associate the uniform block to this binding point.
	glUniformBlockBinding(rendering_program, uniform_block_index, 0);
	glError(__FILE__, __LINE__);

	GLuint index;
	GLchar* names[] = {
		"ColorBlock.SurfaceColor",
		"ColorBlock.WarmColor",
		"ColorBlock.CoolColor",
		"ColorBlock.DiffuseWarm",
		"ColorBlock.DiffuseCool"
	};

	// To update a single uniform in a uniform block, we need to get its
	// offset into the buffer.
	glGetUniformIndices(rendering_program, 1, &names[2], &index);
	glGetActiveUniformsiv(rendering_program, 1, &index, GL_UNIFORM_OFFSET, &offset);
	glGetActiveUniformsiv(rendering_program, 1, &index, GL_UNIFORM_SIZE, &single_size);
	glError(__FILE__, __LINE__);


}

// Our rendering function
void render(double currentTime)
{
	const GLfloat bg_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, bg_color);
	//glClearBufferfv(GL_DEPTH, 0, bg_color);
	glError(__FILE__, __LINE__);

	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
	glError(__FILE__, __LINE__);

	GLfloat colors[] = {
		0.45f, 0.45f, 1.0f, 1.0f,
		0.45f, 0.45f, 1.0f, 1.0f,
		0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	// We can use BufferData to upload our data to the shader,
	// since we know it's in the std140 layout
	glBufferData(GL_UNIFORM_BUFFER, 80, colors, GL_DYNAMIC_DRAW);
	//With a non-standard layout, we'd use BufferSubData for each uniform.
	glBufferSubData(GL_UNIFORM_BUFFER, offset, single_size, &colors[8]);
	//the teapot winds backwards
	//glFrontFace(GL_CW);
	glutSolidTeapot(1.33);
	//glFrontFace(GL_CCW);
}

void shutdown(void)
{
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);
}