//2014105024 ±Ë¿Á±∏ HW2

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#pragma comment(lib, "glew32.lib")

static char* vsSource = "#version 120 \n\
attribute vec4 aPosition; \n\
attribute vec4 aColor; \n\
varying vec4 vColor; \n\
void main(void) { \n\
  gl_Position = aPosition; \n\
  vColor = aColor; \n\
}";

static char* fsSource = "#version 120 \n\
varying vec4 vColor; \n\
void main(void) { \n\
  gl_FragColor = vColor; \n\
}";

GLuint vs = 0;
GLuint fs = 0;
GLuint prog = 0;

char buf[1024];

void myinit(void) {
	GLint status;
	// vs: vertex shader
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsSource, NULL);
	glCompileShader(vs); // compile to get .OBJ
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	printf("vs compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(vs, sizeof(buf), NULL, buf);
	printf("vs log = [%s]\n", buf);
	// fs: fragment shader
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSource, NULL);
	glCompileShader(fs); // compile to get .OBJ
	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	printf("fs compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(fs, sizeof(buf), NULL, buf);
	printf("fs log = [%s]\n", buf);
	// prog: program
	prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog); // link to get .EXE
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	printf("program link status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetProgramInfoLog(prog, sizeof(buf), NULL, buf);
	printf("link log = [%s]\n", buf);
	glValidateProgram(prog);
	glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
	printf("program validate status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetProgramInfoLog(prog, sizeof(buf), NULL, buf);
	printf("validate log = [%s]\n", buf);
	glUseProgram(prog); // execute it !
}

void mykeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESCAPE
		exit(0);
		break;
	}
}

GLfloat first[] = {
	-0.8F, 0.7F, 0.0F, 1.0F,
	-0.8F, -0.9F, 0.0F, 1.0F,
	0.8F, -0.9F, 0.0F, 1.0F,
};

GLfloat second[] = {
	-0.8F, 0.9F, 0.0F, 1.0F,
	0.8F, 0.9F, 0.0F, 1.0F,
	0.8F, -0.7F, 0.0F, 1.0F,
};

GLfloat colors[] = {
	1.0F, 1.0F, 0.0F, 1.0F, // red
	1.0F, 1.0F, 0.0F, 1.0F, // green
	1.0F, 1.0F, 0.0F, 1.0F, // blue
};

void mydisplay(void) {
	GLuint locPosition;
	GLuint locColor;
	// clear 
	glClearColor(0.0F, 0.0F, 1.0F, 1.0F); // blue
	glClear(GL_COLOR_BUFFER_BIT);
	// provide the vertex attributes
	locPosition = glGetAttribLocation(prog, "aPosition");
	glEnableVertexAttribArray(locPosition);
	glVertexAttribPointer(locPosition, 4, GL_FLOAT, GL_FALSE, 0, first);
	// provide the color attributes
	locColor = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(locColor);
	glVertexAttribPointer(locColor, 4, GL_FLOAT, GL_FALSE, 0, colors);
	// draw the first triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// draw the second triangle
	glVertexAttribPointer(locPosition, 4, GL_FLOAT, GL_FALSE, 0, second);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// flush all
	glFlush();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("simple");
	glutDisplayFunc(mydisplay);
	glutKeyboardFunc(mykeyboard);
	glewInit();
	myinit();
	glutMainLoop();
	return 0;
}
