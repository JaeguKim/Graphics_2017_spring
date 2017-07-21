#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#pragma comment(lib, "glew32.lib")
#define MAX_BULLET 100
#define COOR_NUM 12
#define WINDOW_SIZE 500

using namespace std;

static char* vsSource = "#version 120 \n\
attribute vec4 vertex; \n\
attribute vec4 aColor; \n\
varying vec4 vColor; \n\
void main(void) { \n\
  gl_Position = vertex; \n\
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

GLfloat plane0[] = { // initial position
	-0.2F, -0.8F, 0.0F, 1.0F,
	+0.2F, -0.8F, 0.0F, 1.0F,
	+0.0F, -0.5F, 0.0F, 1.0F,
};

GLfloat plane1[] = { // current position
	-0.2F, -0.8F, 0.0F, 1.0F,
	+0.2F, -0.8F, 0.0F, 1.0F,
	+0.0F, -0.5F, 0.0F, 1.0F,
};

GLfloat stone0[] = { // dropping obstacle
	-0.5F, +0.9F, 0.0F, 1.0F,
	-0.4F, +0.9F, 0.0F, 1.0F,
	-0.5F, +1.0F, 0.0F, 1.0F,
};

GLfloat stone1[] = { // dropping obstacle
	-0.5F, +0.9F, 0.0F, 1.0F,
	-0.4F, +0.9F, 0.0F, 1.0F,
	-0.5F, +1.0F, 0.0F, 1.0F,
};
GLfloat temp_color[] = { +1.0F, +0.0F, +0.0F, +1.0F }; //red color
GLfloat temp_colors[] = {
	1.0F, 0.0F, 0.0F, 1.0F, 
	1.0F, 0.0F, 0.0F, 1.0F, 
	1.0F, 0.0F, 0.0F, 1.0F, 
};

//variables for Bullet
bool is_bullet[MAX_BULLET] = { false }; //decide whether drawing bullet or not
GLfloat bullet[MAX_BULLET][COOR_NUM]; //bullet coordinate
GLfloat bullet_color[] = {  //sky blue color
	0.0F, +0.74902F, +1.0F, +1.0F,
	0.0F, +0.74902F, +1.0F, +1.0F,
	0.0F, +0.74902F, +1.0F, +1.0F,
}; 

int bullet_cnt = 0;

//variable for movement
const float planeStep = 0.04F;
const float stoneStep = 0.0001F;
const float bulletStep = 0.0001F;

//move stone
void move_stone(void) {
	stone1[1] -= stoneStep; // vertex#0.y
	stone1[5] -= stoneStep; // vertex#1.y
	stone1[9] -= stoneStep; // vertex#2.y
	if (stone1[9] < -1.0F) {
		memcpy(stone1, stone0, sizeof(stone0));
	}
}
//move bullets IF bullet exists 
void move_bullets(void) {
	//cout << is_bullet[0] << endl;
	for (int i = 0; i < bullet_cnt; i++) {
		if (is_bullet[i] == true) //IF bullet exist, move bullets
		{
			bullet[i][1] += bulletStep;
			bullet[i][5] += bulletStep;
			bullet[i][9] += bulletStep;
			if (bullet[i][5] >= WINDOW_SIZE / 2) //IF bullet escapes window, deactivate bullet
				is_bullet[i] = false;
		}
	}
}

void myidle(void) {
	move_stone();
	move_bullets();
	// redisplay
	glutPostRedisplay();
}

//initialize bullet coordinate, start right above the plane1 + Activate bullet
void init_bullet_coor(GLfloat bullet[]) {
	//set bullet's left coor
	bullet[0] = plane1[8] - 0.05;
	bullet[1] = plane1[9];
	bullet[2] = plane1[10];
	bullet[3] = plane1[11];
	//set bullet's right coor
	bullet[4] = plane1[8] + 0.05;
	bullet[5] = plane1[9];
	bullet[6] = plane1[10];
	bullet[7] = plane1[11];
	//set bullet's middle coor
	bullet[8] = (bullet[0] + bullet[4]) / 2;
	bullet[9] = plane1[9] + 0.15;
	bullet[10] = plane1[10];
	bullet[11] = plane1[11];
}

//make bullet , Activate Bullet AND increase Bullet count
void make_bullet(GLfloat bullet[][COOR_NUM]) {
	// IF bullet count is larger than MAX_BULLET, set bullet count 0
	if (bullet_cnt >= MAX_BULLET) {
		//if bullet[0] is available
		if (is_bullet[0] == false)
			bullet_cnt = 0;
		//if bullet[0] is not available, waiting
		else
			return;
	}
	init_bullet_coor(bullet[bullet_cnt]);
	is_bullet[bullet_cnt] = true;
	bullet_cnt++;
}

void mykeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESCAPE
		exit(0);
		break;
	case 'r': // reset
		memcpy(plane1, plane0, sizeof(plane0));
		glutPostRedisplay();
		break;
		// user interface: simple joystick!
	case 'a': // left move
		plane1[0] -= planeStep;
		plane1[4] -= planeStep;
		plane1[8] -= planeStep;
		glutPostRedisplay();
		break;
	case 'd': // right move
		plane1[0] += planeStep;
		plane1[4] += planeStep;
		plane1[8] += planeStep;
		glutPostRedisplay();
		break;
	case ' ':	// shoot
		make_bullet(bullet);
		glutPostRedisplay();
		break;
	}
}

void myinit(void) {
	// vs: vertex shader
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsSource, NULL);
	glCompileShader(vs); // compile to get .OBJ
						 // fs: fragment shader
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSource, NULL);
	glCompileShader(fs); // compile to get .OBJ
						 // prog: program
	prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog); // link to get .EXE
	glUseProgram(prog); // execute it !
}

//draw bullets IF bullet exists
void drawBullets(GLuint loc) {
	for (int i = 0; i < bullet_cnt; i++) {
		if (is_bullet[i] == true)
		{
			glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, bullet[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
}

void mydisplay(void) {
	GLuint loc;
	GLuint color_loc;
	// clear in black color
	glClearColor(0.3F, 0.3F, 0.3F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT);
	// provide the vertex attributes
	loc = glGetAttribLocation(prog, "vertex");
	glEnableVertexAttribArray(loc);
	color_loc = glGetAttribLocation(prog, "aColor");
	glEnableVertexAttribArray(color_loc);
	// draw the plane
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, temp_color );
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, plane1);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// draw the stone
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, temp_color);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, stone1);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// draw the bullets
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, bullet_color);
	drawBullets(loc);
	// flush all
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("game");
	glutDisplayFunc(mydisplay);
	glutKeyboardFunc(mykeyboard);
	glutIdleFunc(myidle);
	glewInit();
	myinit();
	glutMainLoop();
	return 0;
}
