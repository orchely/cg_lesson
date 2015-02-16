#include <GL/glew.h>
#include <GL/freeglut.h>
#include <object.h>

sb6::object obj;

void startup()
{
	obj.load("..\\media\\objects\\bunny_1k.sbm");
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void render(double currentTime)
{
	const GLfloat color[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);
	obj.render();
}

void shutdown()
{
	obj.free();
}