#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

extern void startup();
extern void render(double currentTime);
extern void shutdown();

LARGE_INTEGER freq;

DWORD initializeGlut()
{
	DWORD result = ERROR_SUCCESS;
	int argc = 0;
	wchar_t **argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argvw == nullptr) {
		return GetLastError();
	}
	char **argv = reinterpret_cast<char**>(std::calloc(argc, sizeof(char*)));
	if (argv == nullptr) {
		return E_OUTOFMEMORY;
	}
	for (int i = 0; i < argc; i++) {
		int len = WideCharToMultiByte(CP_ACP, 0, argvw[i], -1, nullptr, 0, nullptr, nullptr);
		if (len == 0) {
			result = GetLastError();
			goto end;
		}
		argv[i] = reinterpret_cast<char*>(std::calloc(len, sizeof(char)));
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
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutSetOption(GLUT_INIT_WINDOW_WIDTH, 800);
	glutSetOption(GLUT_INIT_WINDOW_HEIGHT, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
end:
	if (argv != nullptr) {
		for (int i = 0; i < argc; i++) {
			if (argv[i] != nullptr) {
				std::free(argv[i]);
			}
		}
		std::free(argv);
	}
	return result;
}

GLenum initializeGlew()
{
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();

	// GLEW calls glGetString(GL_EXTENSIONS) internaly.
	// This causes GL_INVALID_ENUM with OpenGL 3.2 (or later) contexts.
	while (glGetError() == GL_INVALID_ENUM)
		;

	return result;
}

void display(void)
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	double currentTime = static_cast<double>(t.QuadPart) / static_cast<double>(freq.QuadPart);
	render(currentTime);
	glutSwapBuffers();

	static int frame;
	static double lastTime = currentTime;

	frame++;
	double delta = currentTime - lastTime;
	if (delta >= 1.0) {
		std::wostringstream oss;
		oss << std::setprecision(4) << (static_cast<double>(frame) / delta) << L" fps" << std::endl;
		OutputDebugStringW(oss.str().c_str());
		lastTime = currentTime;
		frame = 0;
	}
}

int APIENTRY WinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR     lpCmdLine,
	_In_     int       nCmdShow)
{
	QueryPerformanceFrequency(&freq);

	int result = initializeGlut();
	if (result != ERROR_SUCCESS) {
		return result;
	}

	glutCreateWindow("OpenGL");

	GLenum glResult = initializeGlew();
	if (glResult != GLEW_OK) {
		return glResult;
	}

	glutCloseFunc(shutdown);
	glutDisplayFunc(display);
	glutIdleFunc(glutPostRedisplay);

	startup();
	glutMainLoop();

	return 0;
}