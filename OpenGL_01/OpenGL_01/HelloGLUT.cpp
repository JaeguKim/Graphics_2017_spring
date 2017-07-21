//2014105024 ±Ë¿Á±∏ HW1

#include <stdio.h>
#include <GL/glew.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#pragma comment(lib, "glew32.lib")

void myinit(void) {
	const char* strVersion = (const char*)(glGetString(GL_VERSION));
	printf("version = %s\n", strVersion);
	fflush(stdout);
}

void mydisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutCreateWindow("simple");
	glutDisplayFunc(mydisplay);
	glewInit();
	myinit();
	glutMainLoop();
	return 0;
}

