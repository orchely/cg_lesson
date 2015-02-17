#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <sstream>
#include <ostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

GLint offset = -1;
GLint single_size = -1;
GLuint rendering_program = UINT_MAX;
GLuint vertex_array_object = UINT_MAX;
GLuint uniform_buffer = UINT_MAX;

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
		"// Vertex shader for Gooch shading								\n"
		"// Author: Randi Rost											\n"
		"// Copyright (c) 2002-2006 3Dlabs Inc. Ltd.					\n"
		"// See 3Dlabs-License.txt for license information				\n"
		"																\n"
		"vec3 LightPosition = vec3(0.0, 10.0, 4.0); 					\n"
		"																\n"
		"varying float NdotL;											\n"
		"varying vec3  ReflectVec;										\n"
		"varying vec3  ViewVec;											\n"
		"																\n"
		"void main(void)												\n"
		"{																\n"
		"	vec3 ecPos      = vec3 (gl_ModelViewMatrix * gl_Vertex);	\n"
		"	vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);	\n"
		"	vec3 lightVec   = normalize(LightPosition - ecPos);			\n"
		"	ReflectVec      = normalize(reflect(-lightVec, tnorm));		\n"
		"	ViewVec         = normalize(-ecPos);						\n"
		"	NdotL           = (dot(lightVec, tnorm) + 1.0) * 0.5;		\n"
		"	gl_Position     = ftransform();								\n"
		"}																\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#extension GL_ARB_uniform_buffer_object : enable						\n"
		"																		\n"
		"layout(std140) uniform colors0											\n"
		"{																		\n"
		"	float DiffuseCool;													\n"
		"	float DiffuseWarm;													\n"
		"	vec3  SurfaceColor;													\n"
		"	vec3  WarmColor;													\n"
		"	vec3  CoolColor;													\n"
		"};																		\n"
		"																		\n"
		"varying float NdotL;													\n"
		"varying vec3  ReflectVec;												\n"
		"varying vec3  ViewVec;													\n"
		"																		\n"
		"void main (void)														\n"
		"{																		\n"
		"	vec3 kcool    = min(CoolColor + DiffuseCool * SurfaceColor, 1.0);	\n"
		"	vec3 kwarm    = min(WarmColor + DiffuseWarm * SurfaceColor, 1.0); 	\n"
		"	vec3 kfinal   = mix(kcool, kwarm, NdotL);							\n"
		"																		\n"
		"	vec3 nreflect = normalize(ReflectVec);								\n"
		"	vec3 nview    = normalize(ViewVec);									\n"
		"																		\n"
		"	float spec    = max(dot(nreflect, nview), 0.0);						\n"
		"	spec          = pow(spec, 32.0);									\n"
		"																		\n"
		"	gl_FragColor = vec4 (min(kfinal + spec, 1.0), 1.0);					\n"
		"}																		\n"
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

void startup(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 800.0f / 600.0f, 0.1, 100.0);

	rendering_program = compile_shaders();

	// Update the uniforms using ARB_uniform_buffer_object
	glGenBuffers(1, &uniform_buffer);

	// There's only one uniform block here, the 'colors0' uniform block. 
	// It contains the color info for the gooch shader.
	GLuint uniform_block_index = glGetUniformBlockIndex(rendering_program, "colors0");

	// We need to get the uniform block's size in order to back it with the
	// appropriate buffer
	GLsizei uniform_block_size;
	glGetActiveUniformBlockiv(rendering_program, uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniform_block_size);

	glError(__FILE__, __LINE__);

	// Create UBO.
	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
	glBufferData(GL_UNIFORM_BUFFER, uniform_block_size, nullptr, GL_DYNAMIC_DRAW);

	// Now we attach the buffer to UBO binding point 0...
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);
	// And associate the uniform block to this binding point.
	glUniformBlockBinding(rendering_program, uniform_block_index, 0);
	glError(__FILE__, __LINE__);

	GLuint index;
	GLchar* names[] = {
		"SurfaceColor",
		"WarmColor",
		"CoolColor",
		"DiffuseWarm",
		"DiffuseCool"
	};

	// To update a single uniform in a uniform block, we need to get its
	// offset into the buffer.
	glGetUniformIndices(rendering_program, 1, &names[2], &index);
	glGetActiveUniformsiv(rendering_program, 1, &index, GL_UNIFORM_OFFSET, &offset);
	glGetActiveUniformsiv(rendering_program, 1, &index, GL_UNIFORM_SIZE, &single_size);
	glError(__FILE__, __LINE__);

	glViewport(0, 0, 800, 600);
}

void render(double currentTime)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(rendering_program);

	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -4);
	glColor3f(1.0f, 1.0f, 1.0);
	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);

	GLfloat colors[] = {
		0.45f, 0.45f, 1.0f,  1.0f,
		0.45f, 0.45f, 1.0f,  1.0f,
		0.75f, 0.75f, 0.75f, 1.0f,
		0.0f,  0.0f,  1.0f,  1.0f,
		0.0f,  1.0f,  0.0f,  1.0f
	};

	// We can use BufferData to upload our data to the shader,
	// since we know it's in the std140 layout
	glBufferData(GL_UNIFORM_BUFFER, 80, colors, GL_DYNAMIC_DRAW);
	//With a non-standard layout, we'd use BufferSubData for each uniform.
	glBufferSubData(GL_UNIFORM_BUFFER, offset, single_size, &colors[8]);
	//the teapot winds backwards
	glFrontFace(GL_CW);
	glutSolidTeapot(1.33);
	glFrontFace(GL_CCW);
}

void shutdown(void)
{
}