#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

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

void display(void)
{
	double s = getCurrentSec();

	const GLfloat color[] = {
		static_cast<float>(sin(s)) * 0.5f + 0.5f,
		static_cast<float>(cos(s)) * 0.5f + 0.5f,
		1.0f, 1.0f
	};
	glClearBufferfv(GL_COLOR, 0, color);
	glutSwapBuffers();
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

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("OpenGL");

	GLenum glResult = glewInit();
	if (glResult != GLEW_OK) {
		return glResult;
	}

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}