#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glut.h>

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 200;

#define DEG2RAD(a) (a * 0.0174532925)
#define GLUT_KEY_ESCAPE 27


class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 20.0f, float eyeY = 5.0f, float eyeZ = 20.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;


Vector3f Eye(20, 5, 20);
Vector3f At(0, 0, 0);
Vector3f Up(0, 1, 0);

int cameraZoom = 0;
int countdownTime = 300;  //5 minutes
float playerX = -33 ;
float playerY = 1.5 ;
float playerZ = -35 ;
float playerRotation = 0.0f;
float oldPlayerRotation = 0.0f;
bool firstPerson = false;
bool thirdPerson = false;
bool toggleView = true;
bool levelTwo = false;

struct GrassWall {
	int x, z, angle;
};

struct ArenaWall {
	int x, z, angle;
};


GrassWall grassWallCoordinates[43] = {
		{5, 35, 90},
{-5, 35, 90},
{-5, 25, 90},
{0, 20, 0},
{5, 15, 90},
{10, 30, 0},
{15, 25, 90},
{15, 25, 90},
{20, 20, 0},
{10, 10, 0},
{20, 10, 0},
{25, 5, 90},
{20, 0, 0},
{10, 0, 0},
{0, 0, 0},
{-5, 5, 90},
{15, -5, 90},
{35, -10, 0},
{30, -15, 90},
{25, -20, 0},
{5, -35, 90},
{5, -25, 90},
{5, -15, 90},
{0, -10, 0},
{-10, -10, 0},
{-15, -15, 90},
{-10, -20, 0},
{-15, 35, 90},
{-30, 25, 90},
{-25, 20, 0},
{-35, 20, 0},
{-20, 15, 90},
{-20, 5, 90},
{-25, 0, 0},
{-30, 5, 90},
{-35, -10, 0},
{-30, -25, 90},
{-30, -35, 90},
{30, 35, 90},
{-25, -20, 0},
{-20, -25, 90},
{-15, -30, 0},
{-10, -35, 90}
};

ArenaWall arenaWallCoordinates[43] = {
	 {5, -35, 90},
	 {-5, -35, 90},
	 {-5, -25, 90},
	 {0, -20, 0},
	 {5, -15, 90},
	 {10, -30, 0},
	 {15, -25, 90},
	 {15, -25, 90},
	 {20, -20, 0},
	 {10, -10, 0},
	 {20, -10, 0},
	 {25, -5, 90},
	 {20, 0, 0},
	 {10, 0, 0},
	 {0, 0, 0},
	 {-5, -5, 90},
	 {15, 5, 90},
	 {35, 10, 0},
	 {30, 15, 90},
	 {25, 20, 0},
	 {5, 35, 90},
	 {5, 25, 90},
	 {5, 15, 90},
	 {0, 10, 0},
	 {-10, 10, 0},
	 {-15, 15, 90},
	 {-10, 20, 0},
	 {-15, -35, 90},
	 {-30, -25, 90},
	 {-25, -20, 0},
	 {-35, -20, 0},
	 {-20, -15, 90},
	 {-20, -5, 90},
	 {-25, 0, 0},
	 {-30, -5, 90},
	 {-35, 10, 0},
	 {-30, 25, 90},
	 {-30, 35, 90},
	 {30, -35, 90},
	 {-25, 20, 0},
	 {-20, 25, 90},
	 {-15, 30, 0},
	 {-10, 35, 90}
};

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_table;
Model_3DS model_human;
Model_3DS model_concreteWall;
Model_3DS model_grassWall;

// Textures
GLTexture tex_ground;
GLTexture tex_concrete;
GLTexture tex_grass;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_grass.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-40, 0, -40);
	glTexCoord2f(5, 0);
	glVertex3f(40, 0, -40);
	glTexCoord2f(5, 5);
	glVertex3f(40, 0, 40);
	glTexCoord2f(0, 5);
	glVertex3f(-40, 0, 40);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderConcreteGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_concrete.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-40, 0, -40);
	glTexCoord2f(1, 0);
	glVertex3f(40, 0, -40);
	glTexCoord2f(1, 1);
	glVertex3f(40, 0, 40);
	glTexCoord2f(0, 1);
	glVertex3f(-40, 0, 40);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60, 640 / 480, 0.001, 100);
	gluPerspective(fovy, aspectRatio, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void timer(int val)
{
	if (countdownTime != 0) {
		countdownTime--;
	}
	glutTimerFunc(1000, timer, 0);
	glutPostRedisplay();

}

void drawWall(double thickness) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
	glBindTexture(GL_TEXTURE_2D, tex_concrete.texture[0]);
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(30, thickness, 30);
	glutSolidCube(1);
	glTexCoord2f(0, 0);
	glTexCoord2f(5, 0);
	glTexCoord2f(5, 5);
	glTexCoord2f(0, 5);
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void arenaWall(float x, float z , float angle) {
	glPushMatrix();
	glTranslatef(x, 0, z);
	glRotatef(angle, 0, 1, 0);

	glScalef(2, 0.8, 0.3);
	model_concreteWall.Draw();
	glPopMatrix();
}

void grassWall(float x, float z, float angle) {
	glPushMatrix();
	glTranslatef(x, 0, z);
	glRotatef(angle, 0, 1, 0);

	glScalef(2, 0.8, 0.3);
	model_grassWall.Draw();
	glPopMatrix();
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	setupCamera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	if (levelTwo) {

		printf("X: %f and Z: %f", playerX, playerZ);
		
		RenderConcreteGround();

		
		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_concreteWall.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_concreteWall.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_concreteWall.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_concreteWall.Draw();
		glPopMatrix();

		/*arenaWall(5, -35, 90);
		arenaWall(-5, -35, 90);
		arenaWall(-5, -25, 90);
		arenaWall(0, -20, 0);
		arenaWall(5, -15, 90);
		arenaWall(10, -30, 0);
		arenaWall(15, -25, 90);
		arenaWall(15, -25, 90);
		arenaWall(20, -20, 0);
		arenaWall(10, -10, 0);
		arenaWall(20, -10, 0);

		arenaWall(25, -5, 90);

		arenaWall(20, 0, 0);
		arenaWall(10, 0, 0);
		arenaWall(0, 0, 0);

		arenaWall(-5, -5, 90);

		arenaWall(15, 5, 90);
		arenaWall(35, 10, 0);

		arenaWall(30, 15, 90);
		arenaWall(25, 20, 0);

		arenaWall(5, 35, 90);
		arenaWall(5, 25, 90);
		arenaWall(5, 15, 90);

		arenaWall(0, 10, 0);
		arenaWall(-10, 10, 0);

		arenaWall(-15, 15, 90);
		arenaWall(-10, 20, 0);

		arenaWall(-15, -35, 90);
		arenaWall(-30, -25, 90);
		arenaWall(-25, -20, 0);
		arenaWall(-35, -20, 0);

		arenaWall(-20, -15, 90);
		arenaWall(-20, -5, 90);
		arenaWall(-25, 0, 0);
		arenaWall(-30, -5, 90);

		arenaWall(-35, 10, 0);
		arenaWall(-30, 25, 90);
		arenaWall(-30, 35, 90);
		arenaWall(30, -35, 90);

		arenaWall(-25, 20, 0);
		arenaWall(-20, 25, 90);
		arenaWall(-15, 30, 0);
		arenaWall(-10, 35, 90);*/

		for (int i = 0; i < 43; ++i) {
			arenaWall(arenaWallCoordinates[i].x, arenaWallCoordinates[i].z, arenaWallCoordinates[i].angle);
		}


		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glRotatef(playerRotation, 0.0, 1.0, 0.0);
		model_human.Draw();
		glPopMatrix();
	}
	else {
		printf("X: %f and Z: %f", playerX, playerZ);
		// Draw Ground
		RenderGround();
		
		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_grassWall.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_grassWall.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_grassWall.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 41);
		glScalef(17, 0.8, 0.3);
		model_grassWall.Draw();
		glPopMatrix();

		// Draw house Model
		glPushMatrix();
		glRotatef(90.f, 1, 0, 0);
		glTranslatef(-35, -38, 0);
		model_house.Draw();
		glPopMatrix();

		/*glPushMatrix();
		glTranslatef(10, 0, 0);
		glScalef(0.7, 0.7, 0.7);
		model_table.Draw();
		glPopMatrix();*/

		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glRotatef(playerRotation, 0.0, 1.0, 0.0);
		model_human.Draw();
		glPopMatrix();

		//sky box
		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();

		/*grassWall(5, 35, 90);
		grassWall(-5, 35, 90);
		grassWall(-5, 25, 90);
		grassWall(0, 20, 0);
		grassWall(5, 15, 90);
		grassWall(10, 30, 0);
		grassWall(15, 25, 90);
		grassWall(15, 25, 90);
		grassWall(20, 20, 0);
		grassWall(10, 10, 0);
		grassWall(20, 10, 0);

		grassWall(25, 5, 90);

		grassWall(20, 0, 0);
		grassWall(10, 0, 0);
		grassWall(0, 0, 0);

		grassWall(-5, 5, 90);

		grassWall(15, -5, 90);
		grassWall(35, -10, 0);

		grassWall(30, -15, 90);
		grassWall(25, -20, 0);

		grassWall(5, -35, 90);
		grassWall(5, -25, 90);
		grassWall(5, -15, 90);

		grassWall(0, -10, 0);
		grassWall(-10, -10, 0);

		grassWall(-15, -15, 90);
		grassWall(-10, -20, 0);

		grassWall(-15, 35, 90);
		grassWall(-30, 25, 90);
		grassWall(-25, 20, 0);
		grassWall(-35, 20, 0);

		grassWall(-20, 15, 90);
		grassWall(-20, 5, 90);
		grassWall(-25, 0, 0);
		grassWall(-30, 5, 90);

		grassWall(-35, -10, 0);
		grassWall(-30, -25, 90);
		grassWall(-30, -35, 90);
		grassWall(30, 35, 90);

		grassWall(-25, -20, 0);
		grassWall(-20, -25, 90);
		grassWall(-15, -30, 0);
		grassWall(-10, -35, 90);*/



		for (int i = 0; i < 43; ++i) {
			grassWall(grassWallCoordinates[i].x, grassWallCoordinates[i].z, grassWallCoordinates[i].angle);
		}



	}

	



	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char key, int x, int y) {
	float d = 1;

	switch (key) {
	case 'u':
		camera.moveY(d);
		break;
	case 'j':
		camera.moveY(-d);
		break;
	case 'h':
		camera.moveX(d);
		break;
	case 'k':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case '1':  //NORMAL VIEW
		camera.eye = Vector3f(20.0f, 5.0f, 20.0f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 1.0f, 0.0f);
		firstPerson = false;
		thirdPerson = false;
		break;
	case '2': //TOP VIEW
		camera.eye = Vector3f(0.1f, 80.0f, 0.0f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 1.0f, 0.0f);
		firstPerson = false;
		thirdPerson = false;
		break;
	case '3': //FIRST PERSON VIEW
		if (playerRotation == -90) { 
			camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
			camera.center = Vector3f(playerX - 20, playerY, playerZ);
		}
		else if (playerRotation == 90) {
			camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
			camera.center = Vector3f(playerX + 20, playerY, playerZ);
		}
		else if (playerRotation == 0) {
			camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
			camera.center = Vector3f(playerX, playerY, playerZ + 20);
		}
		else {
			camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
			camera.center = Vector3f(playerX, playerY, playerZ - 20);
		}
		firstPerson = true;
		thirdPerson = false;
		break;

	case '4': //THIRD PERSON VIEW
		camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
		camera.center = Vector3f(playerX, playerY - 3, playerZ + 20);
		thirdPerson = true;
		firstPerson = false;
		break;

		//PLAYER MOVEMENT
	case 'w': //move forward
		if (playerZ + 0.2 < 40) {
			playerZ += 0.2;
			playerRotation = 0;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX , playerY, playerZ + 20);
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
				camera.center = Vector3f(playerX , playerY - 3, playerZ + 20);
			}

		}
		break;
	case 's': //move backward
		if (playerZ - 0.2 > -40) {
			playerZ -= 0.2;
			playerRotation = 180;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX , playerY, playerZ - 20);
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
				camera.center = Vector3f(playerX, playerY - 3, playerZ + 20);
			}

		}
		break;
	case 'a': //move left
		if (playerX + 0.2 < 40) {
			playerX += 0.2;
			playerRotation = 90;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX + 20, playerY, playerZ );
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
				camera.center = Vector3f(playerX , playerY - 3, playerZ + 20);
			}

		}
		break;
	case 'd': //move right
		if (playerX - 0.2 > -40) {
			playerX -= 0.2;
			playerRotation = -90;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX - 20 , playerY, playerZ);
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
				camera.center = Vector3f(playerX, playerY - 3, playerZ + 20);
			}

		}
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	float a = 5.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;

	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	Vector3f oldPosition = camera.center;

	if (firstPerson || thirdPerson)
	{
		if (x > 640) {
			camera.rotateY(0.3);
		}
		else {
			camera.rotateY(-0.3);
		}
	}
	glLoadIdentity();	//Clear Model_View Matrix
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//toggle first person and third person
		if (toggleView) {
			thirdPerson = true;
			firstPerson = false;
			camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
			camera.center = Vector3f(playerX, playerY - 3, playerZ + 20);
			toggleView = false;

		}
		else {
			thirdPerson = false;
			firstPerson = true;
			if (playerRotation == -90) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX - 20, playerY, playerZ);
			}
			else if (playerRotation == 90) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX + 20, playerY, playerZ);
			}
			else if (playerRotation == 0) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX, playerY, playerZ + 20);
			}
			else {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX, playerY, playerZ - 20);
			}

			toggleView = true;
		}
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_table.Load("Models/table/table.3ds");
	model_human.Load("Models/human/human2.3ds");
	model_concreteWall.Load("Models/rock/stone.3ds");
	model_grassWall.Load("Models/grass/grass.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_concrete.Load("Textures/Concrite.bmp");
	tex_grass.Load("Textures/texture-grass.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}



//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(Special);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	//glutReshapeFunc(myReshape);

	glutTimerFunc(0, timer, 0);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	if (levelTwo) {
		playerX = 0.8;
		playerZ = -35.f;
	}

	glutMainLoop();
}