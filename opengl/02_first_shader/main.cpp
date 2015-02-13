#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

void startup(void);
void render(void);
void shutdown(void);
GLuint renderring_program;
GLuint vertex_array_object;

DWORD initializeGlut()
{
	DWORD result = ERROR_SUCCESS;
	int argc = 0;
	wchar_t **argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argvw == nullptr) {
		return GetLastError();
	}
	char **argv = reinterpret_cast<char**>(calloc(argc, sizeof(char*)));
	if (argv == nullptr) {
		return E_OUTOFMEMORY;
	}
	for (int i = 0; i < argc; i++) {
		int len = WideCharToMultiByte(CP_ACP, 0, argvw[i], -1, nullptr, 0, nullptr, nullptr);
		if (len == 0) {
			result = GetLastError();
			goto end;
		}
		argv[i] = reinterpret_cast<char*>(calloc(len, sizeof(char)));
		if (argv[i] == nullptr) {
			result = E_OUTOFMEMORY;
			goto end;
		}
		len = WideCharToMultiByte(CP_ACP, 0, argvw[i], -1, argv[i], len, nullptr, nullptr);
		if (len == 0) {
			result = GetLastError();
			goto end;
		}
	}
	int c = argc;
	glutInit(&c, argv);
end:
	if (argv != nullptr) {
		for (int i = 0; i < argc; i++) {
			if (argv[i] != nullptr) {
				free(argv[i]);
			}
		}
		free(argv);
	}
	return result;
}

double getCurrentSec()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return static_cast<double>(t.QuadPart) / static_cast<double>(f.QuadPart);
}

void idle(void)
{
	glutPostRedisplay();
}

int APIENTRY wWinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPWSTR    lpCmdLine,
	_In_     int       nCmdShow)
{
	int result = initializeGlut();
	if (result != ERROR_SUCCESS) {
		return result;
	}

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("OpenGL");

	GLenum glResult = glewInit();
	if (glResult != GLEW_OK) {
		return glResult;
	}

	startup();

	glutCloseFunc(shutdown);

	glutDisplayFunc(render);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core								\n"
		"												\n"		
		"void main(void)								\n"
		"{												\n"
		"	gl_Position = vec4(0.0, 0.0, 0.5, 1.0);		\n"
		"}												\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core							\n"
		"											\n"
		"out vec4 color;							\n"
		"											\n"
		"void main(void)							\n"
		"{											\n"
		"	color = vec4(0.0, 0.8, 1.0, 1.0);		\n"
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

void startup(void)
{
	renderring_program = compile_shaders();
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
}

void render(void)
{
	double s = getCurrentSec();

	const GLfloat color[] = {
		static_cast<float>(sin(s)) * 0.5f + 0.5f,
		static_cast<float>(cos(s)) * 0.5f + 0.5f,
		1.0f, 1.0f
	};
	glClearBufferfv(GL_COLOR, 0, color);

	glUseProgram(renderring_program);

	glPointSize(40.0f);
	glDrawArrays(GL_POINTS, 0, 1);

	glutSwapBuffers();
}

void shutdown(void)
{
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(renderring_program);
}