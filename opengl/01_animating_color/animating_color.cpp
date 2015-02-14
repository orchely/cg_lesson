#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

void startup()
{
}

void render(double currentTime)
{
	const GLfloat color[] = {
		static_cast<float>(sin(currentTime)) * 0.5f + 0.5f,
		static_cast<float>(cos(currentTime)) * 0.5f + 0.5f,
		1.0f, 1.0f
	};
	glClearBufferfv(GL_COLOR, 0, color);
}

void shutdown()
{
}