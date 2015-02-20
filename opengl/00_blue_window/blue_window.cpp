#include <GL/glew.h>
#include <GL/freeglut.h>

void startup()
{
}

void render(double currentTime)
{
	const GLfloat color[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);
}

void on_resize(int width, int height)
{
}

void shutdown()
{
}