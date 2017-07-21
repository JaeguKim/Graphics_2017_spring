#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#pragma comment(lib, "glew32.lib")
#define USE_MATH_DEFINES
#include <math.h>

#define WINDSIZEX 500
#define WINDSIZEY 500

static char* vsSource = "#version 120 \n\
attribute vec4 aPosition; \n\
attribute vec4 aColor; \n\
varying vec4 vColor; \n\
uniform mat4 uModel; \n\
uniform mat4 uView; \n\
uniform mat4 uProj; \n\
void main(void) { \n\
  gl_Position =  uProj * uView * uModel * aPosition; \n\
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

//Å¥ºê ÁÂÇ¥
GLfloat vertices[] = {
	-0.5, -0.5, -0.5, 1.0, // 0
	-0.5, -0.5, +0.5, 1.0, // 1
	-0.5, +0.5, -0.5, 1.0, // 2
	-0.5, +0.5, +0.5, 1.0, // 3
	+0.5, -0.5, -0.5, 1.0, // 4
	+0.5, -0.5, +0.5, 1.0, // 5
	+0.5, +0.5, -0.5, 1.0, // 6
	+0.5, +0.5, +0.5, 1.0, // 7
};

GLfloat colors[] = {
	0.5, 0.5, 0.5, 1.0, // black
	0.5, 0.5, 1.0, 1.0, // blue
	0.5, 1.0, 0.5, 1.0, // green
	0.5, 1.0, 1.0, 1.0, // cyan
	1.0, 0.5, 0.5, 1.0, // red
	1.0, 0.5, 1.0, 1.0, // magenta
	1.0, 1.0, 0.5, 1.0, // yellow
	1.0, 1.0, 1.0, 1.0, // white
};

GLushort indices[] = { // 36 points, 12 triangles
	0, 4, 6,
	6, 2, 0,
	4, 5, 7,
	7, 6, 4,
	1, 3, 7,
	7, 5, 1,
	0, 2, 3,
	3, 1, 0,
	2, 6, 7,
	7, 3, 2,
	0, 1, 5,
	5, 4, 0,
};

GLuint vbo[1];

char buf[1024];

GLfloat motion_start[3]; // start vector, (x, y, z)
GLfloat motion_end[3]; // end vector, (x, y, z)

GLfloat eye[3], at[3], up[3];

GLfloat moveOffset = 0;
GLfloat dir = 1;
float theta = 0;

GLfloat matTrans[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

GLfloat matTrans2[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

GLfloat matTemp[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

GLfloat matTemp2[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

GLfloat matTemp3[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

GLfloat matRotate[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

GLfloat matModel[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};
GLfloat matView[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};
GLfloat matProj[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};
GLfloat matA[16];
GLfloat matB[16];

void matCopy(GLfloat a[16], GLfloat b[16]) { // a = b;
	a[0] = b[0]; a[4] = b[4]; a[8] = b[8]; a[12] = b[12];
	a[1] = b[1]; a[5] = b[5]; a[9] = b[9]; a[13] = b[13];
	a[2] = b[2]; a[6] = b[6]; a[10] = b[10]; a[14] = b[14];
	a[3] = b[3]; a[7] = b[7]; a[11] = b[11]; a[15] = b[15];
}

void matIden(GLfloat a[16]) { // a = I
	a[0] = 1.0; a[4] = 0.0; a[8] = 0.0; a[12] = 0.0;
	a[1] = 0.0; a[5] = 1.0; a[9] = 0.0; a[13] = 0.0;
	a[2] = 0.0; a[6] = 0.0; a[10] = 1.0; a[14] = 0.0;
	a[3] = 0.0; a[7] = 0.0; a[11] = 0.0; a[15] = 1.0;
}

void matMult(GLfloat c[16], GLfloat a[16], GLfloat b[16]) { // C = A * B
	c[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
	c[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
	c[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
	c[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];
	//
	c[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
	c[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
	c[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
	c[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];
	//
	c[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
	c[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
	c[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
	c[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];
	//
	c[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
	c[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
	c[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
	c[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
}

void vecMult(GLfloat v[4], GLfloat m[16], GLfloat u[4]) { // v = M * u
	v[0] = m[0] * u[0] + m[4] * u[1] + m[8] * u[2] + m[12] * u[3];
	v[1] = m[1] * u[0] + m[5] * u[1] + m[9] * u[2] + m[13] * u[3];
	v[2] = m[2] * u[0] + m[6] * u[1] + m[10] * u[2] + m[14] * u[3];
	v[3] = m[3] * u[0] + m[7] * u[1] + m[11] * u[2] + m[15] * u[3];
}

void myinit(void) {
	GLint status;
	GLuint loc;
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
					// VBO setting
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 8 * 4 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * 4 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 8 * 4 * sizeof(GLfloat), 8 * 4 * sizeof(GLfloat), colors);

	// provide the vertex attributes
	loc = glGetAttribLocation(prog, "aPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	// provide the color attributes
	loc = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(8 * 4 * sizeof(GLfloat)));
	// depth buffer enabled
	glEnable(GL_DEPTH_TEST);

}

void calc_view_mat(void) {
	GLfloat p[3], n[3], v[3], u[3];
	GLfloat l;
	GLfloat vec[4], uec[4];
	int i;

	eye[0] = 1 +2.0 *cos(theta); eye[1] = 1; eye[2] = 1 + 2.0 *sin(theta);
	at[0] = 0; at[1] = 0; at[2] = 0;
	up[0] = 0; up[1] = 1; up[2] = 0;

	// p = eye
	p[0] = eye[0];
	p[1] = eye[1];
	p[2] = eye[2];
	// n = (at - eye)
	n[0] = at[0] - eye[0];
	n[1] = at[1] - eye[1];
	n[2] = at[2] - eye[2];
	l = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	n[0] /= l; n[1] /= l; n[2] /= l;
	// u = up * n
	u[0] = up[1] * n[2] - up[2] * n[1];
	u[1] = up[2] * n[0] - up[0] * n[2];
	u[2] = up[0] * n[1] - up[1] * n[0];
	l = sqrtf(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	u[0] /= l; u[1] /= l; u[2] /= l;
	// v = n * u;
	v[0] = n[1] * u[2] - n[2] * u[1];
	v[1] = n[2] * u[0] - n[0] * u[2];
	v[2] = n[0] * u[1] - n[1] * u[0];
	l = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= l; v[1] /= l; v[2] /= l;
	//
	// view matrix
	matA[0] = u[0]; matA[4] = u[1]; matA[8] = u[2]; matA[12] = 0;
	matA[1] = v[0]; matA[5] = v[1]; matA[9] = v[2]; matA[13] = 0;
	matA[2] = n[0]; matA[6] = n[1]; matA[10] = n[2]; matA[14] = 0;
	matA[3] = 0.0; matA[7] = 0.0; matA[11] = 0.0; matA[15] = 1.0;
	matB[0] = 1; matB[4] = 0; matB[8] = 0; matB[12] = -p[0];
	matB[1] = 0; matB[5] = 1; matB[9] = 0; matB[13] = -p[1];
	matB[2] = 0; matB[6] = 0; matB[10] = 1; matB[14] = -p[2];
	matB[3] = 0.0; matB[7] = 0.0; matB[11] = 0.0; matB[15] = 1.0;

	matMult(matView, matA, matB);
}



void mykeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESCAPE
		exit(0);
		break;
	//make pendulum movement of cube
	case 'z':
		moveOffset += dir * 0.05;
		matTrans[13] = moveOffset;
		if (matTrans[13] <= -1 || matTrans[13] >= 1)
		{
			dir *= -1;
		}
		printf("z-val : %f\n", matTrans[13]);
		matMult(matModel, matTrans, matTemp);
		glutPostRedisplay();
		break;
	//rotate camera
	case 'a':	
		theta += 0.01f;
		calc_view_mat();
		if (theta > 6.2832f)
			theta = 0.0f;
		printf("theta : %f\n", theta);
		glutPostRedisplay();
		break;
	}
}



void myidle(void) {
	moveOffset += dir * 0.0025;
	matTrans[13] = moveOffset;
	if (matTrans[13] <= -1 || matTrans[13] >= 1)
	{
		dir *= -1;
	}
	//printf("z-val : %f\n", matTrans[13]);
	matMult(matModel, matTrans, matTemp);

	theta += 0.0007f;
	calc_view_mat();
	if (theta > 6.2832f)
		theta = 0.0f;
	//printf("theta : %f\n", theta);

	glutPostRedisplay();;
}

void myreshape(int width, int height) {
	int shift;
	if (width >= height) {
		shift = (width - height) / 2;
		glViewport(shift, 0, height, height);
	}
	else {
		shift = (height - width) / 2;
		glViewport(0, shift, width, width);
	}
}


void mydisplay(void) {
	GLuint loc;
	// clear 
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // gray
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// provide the matrices
	loc = glGetUniformLocation(prog, "uModel");
	glUniformMatrix4fv(loc, 1, GL_FALSE, matModel);
	loc = glGetUniformLocation(prog, "uView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, matView);
	loc = glGetUniformLocation(prog, "uProj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, matProj);
	// draw a triangle
	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_SHORT, indices);

	// flush all
	glFlush();
	glutSwapBuffers();
}

void setViewMat(void) {
	
	GLfloat p[3], n[3], v[3], u[3];
	GLfloat l;
	GLfloat vec[4], uec[4];
	int i;

	eye[0] = 3; eye[1] = 1; eye[2] = 1;
	at[0] = 0; at[1] = 0; at[2] = 0;
	up[0] = 0; up[1] = 1; up[2] = 0;
	
	// p = eye
	p[0] = eye[0];
	p[1] = eye[1];
	p[2] = eye[2];
	// n = (at - eye)
	n[0] = at[0] - eye[0];
	n[1] = at[1] - eye[1];
	n[2] = at[2] - eye[2];
	l = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	n[0] /= l; n[1] /= l; n[2] /= l;
	// u = up * n
	u[0] = up[1] * n[2] - up[2] * n[1];
	u[1] = up[2] * n[0] - up[0] * n[2];
	u[2] = up[0] * n[1] - up[1] * n[0];
	l = sqrtf(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	u[0] /= l; u[1] /= l; u[2] /= l;
	// v = n * u;
	v[0] = n[1] * u[2] - n[2] * u[1];
	v[1] = n[2] * u[0] - n[0] * u[2];
	v[2] = n[0] * u[1] - n[1] * u[0];
	l = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= l; v[1] /= l; v[2] /= l;
	//
	// view matrix
	matA[0] = u[0]; matA[4] = u[1]; matA[8] = u[2]; matA[12] = 0;
	matA[1] = v[0]; matA[5] = v[1]; matA[9] = v[2]; matA[13] = 0;
	matA[2] = n[0]; matA[6] = n[1]; matA[10] = n[2]; matA[14] = 0;
	matA[3] = 0.0; matA[7] = 0.0; matA[11] = 0.0; matA[15] = 1.0;
	matB[0] = 1; matB[4] = 0; matB[8] = 0; matB[12] = -p[0];
	matB[1] = 0; matB[5] = 1; matB[9] = 0; matB[13] = -p[1];
	matB[2] = 0; matB[6] = 0; matB[10] = 1; matB[14] = -p[2];
	matB[3] = 0.0; matB[7] = 0.0; matB[11] = 0.0; matB[15] = 1.0;
	matMult(matView, matA, matB);

	printf("matA = \n");
	printf("%f %f %f %f\n", matA[0], matA[4], matA[8], matA[12]);
	printf("%f %f %f %f\n", matA[1], matA[5], matA[9], matA[13]);
	printf("%f %f %f %f\n", matA[2], matA[6], matA[10], matA[14]);
	printf("%f %f %f %f\n", matA[3], matA[7], matA[11], matA[15]);
	// print the result
	printf("matB = \n");
	printf("%f %f %f %f\n", matB[0], matB[4], matB[8], matB[12]);
	printf("%f %f %f %f\n", matB[1], matB[5], matB[9], matB[13]);
	printf("%f %f %f %f\n", matB[2], matB[6], matB[10], matB[14]);
	printf("%f %f %f %f\n", matB[3], matB[7], matB[11], matB[15]);
	printf("view = \n");
	printf("%f %f %f %f\n", matView[0], matView[4], matView[8], matView[12]);
	printf("%f %f %f %f\n", matView[1], matView[5], matView[9], matView[13]);
	printf("%f %f %f %f\n", matView[2], matView[6], matView[10], matView[14]);
	printf("%f %f %f %f\n", matView[3], matView[7], matView[11], matView[15]);
	for (i = 0; i < 8; i++) {
		uec[0] = vertices[4 * i + 0];
		uec[1] = vertices[4 * i + 1];
		uec[2] = vertices[4 * i + 2];
		uec[3] = vertices[4 * i + 3];
		vecMult(vec, matView, uec);
		printf("v[%d]: (%f,%f,%f) --> (%f,%f,%f,%f)\n", i, uec[0], uec[1], uec[2],
			vec[0], vec[1], vec[2], vec[3]);
	}
	fflush(stdout);
}

void setProjMat(void) {
	GLfloat xmin, xmax;
	GLfloat ymin, ymax;
	GLfloat zmin, zmax;
	GLfloat a, b, c, d, e, f;
	GLfloat vec[4], uec[4], wec[4];
	int i;

	xmin = -1; xmax = 1;

	ymin = -1; ymax = 1;

	zmin = 1; zmax = 4;
	
	// perspective projection matrix
	/*
	a = (xmax + xmin) / (xmax - xmin);
	b = (ymax + ymin) / (ymax - ymin);
	c = (zmax + zmin) / (zmax - zmin);
	d = -2 * zmax * zmin / (zmax - zmin);
	e = 2 * zmin / (xmax - xmin);
	f = 2 * zmin / (ymax - ymin);
	
	matProj[0] = e; matProj[4] = 0; matProj[8] = a; matProj[12] = 0;
	matProj[1] = 0; matProj[5] = f; matProj[9] = b; matProj[13] = 0;
	matProj[2] = 0; matProj[6] = 0; matProj[10] = c; matProj[14] = d;
	matProj[3] = 0; matProj[7] = 0; matProj[11] = 1; matProj[15] = 0;
	*/

	// Orthographic Projection matrix
	matProj[0] = 2.0F / (xmax - xmin); matProj[4] = 0.0; matProj[8] = 0.0;
	matProj[12] = -(xmax + xmin) / (xmax - xmin);
	matProj[1] = 0.0; matProj[5] = 2.0F / (ymax - ymin); matProj[9] = 0.0;
	matProj[13] = -(ymax + ymin) / (ymax - ymin);
	matProj[2] = 0.0; matProj[6] = 0.0; matProj[10] = 2.0F / (zmax - zmin);
	matProj[14] = -(zmax + zmin) / (zmax - zmin);
	matProj[3] = 0.0; matProj[7] = 0.0; matProj[11] = 0.0; matProj[15] = 1.0;
	// print the result
	printf("proj = \n");
	printf("%f %f %f %f\n", matProj[0], matProj[4], matProj[8], matProj[12]);
	printf("%f %f %f %f\n", matProj[1], matProj[5], matProj[9], matProj[13]);
	printf("%f %f %f %f\n", matProj[2], matProj[6], matProj[10], matProj[14]);
	printf("%f %f %f %f\n", matProj[3], matProj[7], matProj[11], matProj[15]);
	for (i = 0; i < 8; i++) {
		uec[0] = vertices[4 * i + 0];
		uec[1] = vertices[4 * i + 1];
		uec[2] = vertices[4 * i + 2];
		uec[3] = vertices[4 * i + 3];
		vecMult(wec, matView, uec);
		vecMult(vec, matProj, wec);
		printf("v[%d]: (%f,%f,%f) --> (%f,%f,%f,%f)\n", i, uec[0], uec[1], uec[2],
			vec[0], vec[1], vec[2], vec[3]);
		printf("   --> (%f,%f,%f)\n", vec[0] / vec[3], vec[1] / vec[3], vec[2] / vec[3]);
	}
	fflush(stdout);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDSIZEX, WINDSIZEY);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("simple");
	glutDisplayFunc(mydisplay);
	glutKeyboardFunc(mykeyboard);
	glutReshapeFunc(myreshape);
	glutIdleFunc(myidle);
	glewInit();
	myinit();
	setViewMat();
	setProjMat();
	glutMainLoop();
	return 0;
}
