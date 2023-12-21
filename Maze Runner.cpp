#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <mmsystem.h>
#include <SFML/Audio.hpp>
#include <glut.h>

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
GLuint space_tex;
char title[] = "Maze Runner";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 200;

#define DEG2RAD(a) (a * 0.0174532925)
#define GLUT_KEY_ESCAPE 27

float playerX = -33;
float playerY = 1.5;
float playerZ = -35;


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

	Camera(float eyeX = playerX, float eyeY = playerY + 1, float eyeZ = playerZ - 4, float centerX = playerX, float centerY = playerY - 3, float centerZ = playerZ + 20, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
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
int score = 0;
int countUpTime = 0;  

float scoreX = playerX;
float scoreY = 2.0;
float scoreZ = playerZ;
float playerRotation = 0.0f;
float oldPlayerRotation = 0.0f;
bool firstPerson = false;
bool thirdPerson = true;
bool toggleView = true;
bool levelTwo = false;

int chestOneX = 25;
int chestOneZ = 5;
int chestTwoX = -18;
int chestTwoZ = 20;

int crystalOneX = 33;
int crystalOneZ = 14;
int crystalTwoX = 22;
int crystalTwoZ = -15;

float boulderOneX = 12;
int boulderOneZ = -5;
float boulderTwoX = 10;
int boulderTwoZ = 15;

int bladeOneX = 10;
int bladeOneZ = 10;
int bladeTwoX = 25;
int bladeTwoZ = -15;

bool stumble = false;
bool getUp = true;
float stumbleRotation = 0.0f;

float bladeRotation = 0.0f;

bool bolderOneMove = false;
bool bolderTwoMove = false;
float bolderOneRotation = 0.0f;
float bolderTwoRotation = 0.0f;

float portalX = 0.f;
float portalZ = 37.f;

float GateX = -2.f;
float GateZ = 35.f;

bool gameWin = false;
float playerWinRotation = 0.f;

float lightPositionY = 0.f;
float lightIntensityR = 0.7f;
float lightIntensityG = 0.7f;
float lightIntensityB = 0.7f;

float portalLightIntensityR = 0.3f;
float portalLightIntensityG = 0.3f;
float portalLightIntensityB = 0.3f;


//Sound
sf::Sound gameMusicSound;
sf::SoundBuffer gameMusicBuffer;

sf::Sound collectableSound;
sf::SoundBuffer collectableSoundBuffer;

sf::Sound obstacleSound;
sf::SoundBuffer obstacleSoundBuffer;

sf::Sound gameEndSound;
sf::SoundBuffer gameEndSoundBuffer;

sf::Sound gameWinSound;
sf::SoundBuffer gameWinSoundBuffer;

sf::Sound portalSound;
sf::SoundBuffer portalSoundBuffer;

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
Model_3DS model_boulder;
Model_3DS model_crystal;
Model_3DS model_chest;
Model_3DS model_blade;
Model_3DS model_portal;
Model_3DS model_gate;

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
	GLfloat ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
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

void setupLights() {
	/*GLfloat ambient[] = {0.7f, 0.7f, 0.7, 1.0f};
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);*/

	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	//GLfloat lightIntensity[] = { lightIntensityR, lightIntensityG, lightIntensityB, 1.0f };
	GLfloat lightPosition[] = { playerX, lightPositionY, playerZ, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
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

	//InitLightSource();

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
	countUpTime++;
	lightPositionY-=1.f;
	lightIntensityR -= 0.01f;
	lightIntensityG -= 0.01f;
	lightIntensityB -= 0.01f;
	portalLightIntensityR += 0.001f;
	portalLightIntensityG += 0.001f;
	portalLightIntensityB += 0.001f;
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

void print(int x, int y, int z, char* string)
{
	int len, i;
	glRasterPos3f(x, y, z);
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void drawText(const std::string& text, int x, int y, void* font) {
	glRasterPos2i(x, y);

	for (char c : text) {
		glutBitmapCharacter(font, c);
	}
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	setupLights();
	setupCamera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (gameWin) {

		glColor3f(0, 0, 0);
		char* p1s[20];
		sprintf((char*)p1s, "YOU WON, FINAL SCORE: %d ", score);
		print(playerX, playerY, playerZ, (char*)p1s);

		//put third person view
		camera.eye = Vector3f(playerX, playerY , playerZ - 4);
		camera.center = Vector3f(playerX, playerY , playerZ + 20);
		thirdPerson = true;
		firstPerson = false;

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

		for (int i = 0; i < 43; ++i) {
			arenaWall(arenaWallCoordinates[i].x, arenaWallCoordinates[i].z, arenaWallCoordinates[i].angle);
		}

		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glRotatef(playerWinRotation, 0.0, 1.0, 0.0);
		glRotatef(stumbleRotation, 1.0, 0.0, 0.0);
		model_human.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(chestOneX, 1, chestOneZ);
		glScalef(0.01, 0.01, 0.01);
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
		model_chest.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(chestTwoX, 1, chestTwoZ);
		glScalef(0.01, 0.01, 0.01);
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
		model_chest.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(bladeOneX, 1, bladeOneZ);
		glScalef(0.3, 0.3, 0.3);
		glRotatef(90, 0, 1, 0);
		glRotatef(bladeRotation, 0, 1, 0);
		model_blade.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(bladeTwoX, 1, bladeTwoZ);
		glScalef(0.3, 0.3, 0.3);
		glRotatef(bladeRotation, 0, 1, 0);
		model_blade.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(3.5, 0, 36);
		glScalef(0.03, 0.02, 0.03);
		glRotatef(90, 0, 1, 0);
		model_gate.Draw();
		glPopMatrix();
	}

	else if (levelTwo) {

		glColor3f(0, 0, 0);
		char* p1s[20];
		sprintf((char*)p1s, "Score: %d ", score);
		print(scoreX, scoreY, scoreZ, (char*)p1s);

		GLfloat lightIntensity[] = { lightIntensityR, lightIntensityG, lightIntensityB, 1.0f };
		GLfloat lightPosition[] = { 0.f, lightPositionY, 0.f, 1.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

		//printf("X: %f and Z: %f", playerX, playerZ);
		
		RenderConcreteGround();

		//sky 
		glPushMatrix();
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, space_tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
		glPopMatrix();

		
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

		for (int i = 0; i < 43; ++i) {
			arenaWall(arenaWallCoordinates[i].x, arenaWallCoordinates[i].z, arenaWallCoordinates[i].angle);
		}

		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glRotatef(playerRotation, 0.0, 1.0, 0.0);
		glRotatef(stumbleRotation, 1.0, 0.0, 0.0);
		model_human.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(chestOneX, 1, chestOneZ);
		glScalef(0.01, 0.01, 0.01);
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
		model_chest.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(chestTwoX, 1, chestTwoZ);
		glScalef(0.01, 0.01, 0.01);
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
		model_chest.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(bladeOneX, 1, bladeOneZ);
		glScalef(0.3, 0.3, 0.3);
		glRotatef(90, 0, 1, 0);
		glRotatef(bladeRotation, 0, 1, 0);
		model_blade.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(bladeTwoX, 1, bladeTwoZ);
		glScalef(0.3, 0.3, 0.3);
		glRotatef(bladeRotation, 0, 1, 0);
		model_blade.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(3.5, 0, 36);
		glScalef(0.03, 0.02, 0.03);
		glRotatef(90, 0, 1, 0);
		model_gate.Draw();
		glPopMatrix();


	}
	else {
		glColor3f(1, 1, 1);
		char* p1s[20];
		sprintf((char*)p1s, "Score: %d ", score);
		print(playerX, scoreY, playerZ, (char*)p1s);

		GLfloat lightIntensity[] = { lightIntensityR, lightIntensityG, lightIntensityB, 1.0f };
		GLfloat lightPosition[] = { 0.f, lightPositionY, 0.f, 1.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);


		//GLfloat l2Diffuse[] = { 0.0f, 0.0f, 1.0f, 1.0f };
		//GLfloat l2Ambient[] = { 0.0f, 0.0f, 0.1f, 1.0f };
		GLfloat l2Position[] = { portalX, 1, portalZ, 1.0f };
		GLfloat l2Intensity[] = { portalLightIntensityR, portalLightIntensityG, portalLightIntensityB, 1.0f };
		GLfloat l2Direction[] = { portalX, 0.0, portalZ };
		//glLightfv(GL_LIGHT1, GL_DIFFUSE, l2Diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, l2Intensity);
		glLightfv(GL_LIGHT1, GL_POSITION, l2Position);
		//glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
		//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90.0);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l2Direction);

		/*GLfloat lightPosition2[] = {portalX, 5,portalZ, 1.0f};
		GLfloat lightIntensity2[] = { 100, 100, 255, 1.0f };
		glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
		glLightfv(GL_LIGHT1, GL_AMBIENT, lightIntensity2);*/




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

		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glRotatef(playerRotation, 0.0, 1.0, 0.0);
		glRotatef(stumbleRotation, 1.0, 0.0, 0.0);
		model_human.Draw();
		glPopMatrix();

		//sky 
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

		for (int i = 0; i < 43; ++i) {
			grassWall(grassWallCoordinates[i].x, grassWallCoordinates[i].z, grassWallCoordinates[i].angle);
		}


		glPushMatrix();
		glTranslatef(boulderOneX, 0, boulderOneZ);
		glScalef(5,5, 5);
		model_boulder.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(boulderTwoX, 0, boulderTwoZ);
		glScalef(5, 5, 5);
		model_boulder.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(crystalOneX, 1, crystalOneZ);
		glScalef(0.4, 1.5, 1.5);
		model_crystal.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(crystalTwoX, 1, crystalTwoZ);
		glScalef(0.4, 1.5, 1.5);
		model_crystal.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, -6, 37);
		glScalef(0.01, 0.01, 0.01);
		glRotatef(90, 0, 0, 1);
		model_portal.Draw();
		glPopMatrix();

	}





	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================

bool checkGrassCollision(const GrassWall& wall) {
	// Check collision based on the orientation of the wall
	if (wall.angle == 90) {
		// Wall is vertical (angle = 90)
		if (playerX > wall.x - 1.2 && playerX < wall.x + 1.2 && playerZ > wall.z - 5.5 && playerZ < wall.z + 5.5)
			return true;
		else
			return false;

	}
	else {
		// Wall is horizontal (angle = 0)
		if (playerZ > wall.z - 1.2 && playerZ  < wall.z + 1.2 && playerX > wall.x - 5.5 && playerX < wall.x + 5.5)
			return true;
		else
			return false;
	}

}

bool checkArenaCollision(const ArenaWall& wall) {
	// Check collision based on the orientation of the wall
	if (wall.angle == 90) {
		// Wall is vertical (angle = 90)
		if (playerX > wall.x - 1.2 && playerX < wall.x + 1.2 && playerZ > wall.z - 5.5 && playerZ < wall.z + 5.5)
			return true;
		else
			return false;

	}
	else {
		// Wall is horizontal (angle = 0)
		if (playerZ > wall.z - 1.2 && playerZ  < wall.z + 1.2 && playerX > wall.x - 5.5 && playerX < wall.x + 5.5)
			return true;
		else
			return false;
	}

}

bool checkAllWallCollisions(bool levelTwo) {

	if (levelTwo) {
		for (const auto& wall : arenaWallCoordinates) {
			if (checkArenaCollision(wall)) {
				obstacleSound.play();
				return true;
			}
		}
		return false;
	}

	for (const auto& wall : grassWallCoordinates) {
		if (checkGrassCollision(wall)) {
			obstacleSound.play();
			return true;
		}
	}
	return false;
}

void checkCollectableCollision(bool levelTwo) {
	if (levelTwo) { //TREASURE CHEST COORDINATES ARE {25,5} AND {-18,20}
		if (playerX > chestOneX - 1 && playerX < chestOneX + 1 && playerZ > chestOneZ - 0.5 && playerZ < chestOneZ + 0.5) {
			chestOneX = -100;
			chestOneZ = -100;
			score = score + 100;
			collectableSound.play();
		}
		if (playerX > chestTwoX - 1 && playerX < chestTwoX + 1 && playerZ > chestTwoZ - 0.5 && playerZ < chestTwoZ + 0.5){
			chestTwoX = -100;
			chestTwoZ = -100;
			score = score + 100;
			collectableSound.play();

		}
	}
	else { //CRYSTAL ORB COORDINATES ARE {33,14} AND {22,-15}
		if (playerX > crystalOneX - 1 && playerX < crystalOneX + 1 && playerZ > crystalOneZ - 0.5 && playerZ < crystalOneZ + 0.5) {
			crystalOneX = -100;
			crystalOneZ = -100;
			score = score + 100;
			collectableSound.play();
		}
		if (playerX > crystalTwoX - 1 && playerX < crystalTwoX + 1 && playerZ > crystalTwoZ - 0.5 && playerZ < crystalTwoZ + 0.5) {
			crystalTwoX = -100;
			crystalTwoZ = -100;
			score = score + 100;
			collectableSound.play();
		}

	}
}


void checkObstacleCollision(bool levelTwo) {
	if (levelTwo) { //Blade COORDINATES 
		if (playerX > bladeOneX - 2 && playerX < bladeOneX + 2 && playerZ > bladeOneZ - 2 && playerZ < bladeOneZ + 2) {
			stumble = true;
			getUp = false;
			obstacleSound.play();
		}
		if (playerX > bladeTwoX - 2 && playerX < bladeTwoX + 2 && playerZ > bladeTwoZ - 2 && playerZ < bladeTwoZ + 2) {
			stumble = true;
			getUp = false;
			obstacleSound.play();
		}
	}
	else { //Boulder COORDINATES 
		if (playerX > boulderOneX - 2.5 && playerX < boulderOneX + 2.5 && playerZ > boulderOneZ - 2.5 && playerZ < boulderOneZ + 2.5) {
			getUp = false;
			stumble = true;
			obstacleSound.play();
		}
		if (playerX > boulderTwoX - 2.5 && playerX < boulderTwoX + 2.5 && playerZ > boulderTwoZ - 2.5 && playerZ < boulderTwoZ + 2.5) {
			getUp = false;
			stumble = true;
			obstacleSound.play();
		}

	}
}

void makeBoulderMove(bool levelTwo) {
	if (!levelTwo) { //Boulder COORDINATES 
		if (playerX > boulderOneX - 15 && playerX < boulderOneX + 15 && playerZ > boulderOneZ - 15 && playerZ < boulderOneZ + 15) {
			bolderOneMove = true;
		}
		if (playerX > boulderTwoX - 15 && playerX < boulderTwoX + 15 && playerZ > boulderTwoZ - 15 && playerZ < boulderTwoZ + 15) {
			bolderTwoMove = true;
		}

	}
}

void checkPortalCollision(bool ifLevelTwo) {
	if (!ifLevelTwo) {
		if (playerX > portalX - 1 && playerX < portalX + 1 && playerZ > portalZ - 1 && playerZ < portalZ + 1) {
			playerX = 0.8;
			playerZ = -35.f;
			levelTwo = true;
			lightPositionY = 0.f;
			lightIntensityR = 0.7f;
			lightIntensityG = 0.7f;
			lightIntensityB = 0.7f;
			portalSound.play();
			glutPostRedisplay();
		}
	}
}

void checkGateCollision() {
	if (levelTwo) {
		if (playerX > GateX - 2 && playerX < GateX + 2 && playerZ > GateZ - 3 && playerZ < GateZ + 3) {
			gameWin = true;
			gameMusicSound.stop();
			gameWinSound.play();
			glutPostRedisplay();
		}
	}
}


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
			scoreX = playerX - 1;
			scoreZ = playerZ;
		}
		else if (playerRotation == 90) {
			camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
			camera.center = Vector3f(playerX + 20, playerY, playerZ);
			scoreX = playerX + 1;
			scoreZ = playerZ;
		}
		else if (playerRotation == 0) {
			camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
			camera.center = Vector3f(playerX, playerY, playerZ + 20);
			scoreX = playerX ;
			scoreZ = playerZ+ 1;
		}
		else {
			camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
			camera.center = Vector3f(playerX, playerY, playerZ - 20);
			scoreX = playerX;
			scoreZ = playerZ - 1;
		}
		firstPerson = true;
		thirdPerson = false;
		scoreY = 2.f;
		break;

	case '4': //THIRD PERSON VIEW
		camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
		camera.center = Vector3f(playerX, playerY - 3, playerZ + 20);
		thirdPerson = true;
		firstPerson = false;
		scoreY = 2.f;
		break;

		//PLAYER MOVEMENT
	case 'w': //move forward
		if (playerZ + 0.2 < 40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerZ -= 0.5;
				break;
			}
			playerZ += 0.2;
			playerRotation = 0;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX , playerY, playerZ + 20);
				scoreX = playerX;
				scoreZ = playerZ + 1;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
				camera.center = Vector3f(playerX , playerY - 3, playerZ + 20);
			}

		}
		break;
	case 's': //move backward
		if (playerZ - 0.2 > -40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerZ += 0.5;
				break;
			}
			playerZ -= 0.2;
			playerRotation = 180;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX , playerY, playerZ - 20);
				scoreX = playerX;
				scoreZ = playerZ - 1 ;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ + 4);
				camera.center = Vector3f(playerX, playerY - 3, playerZ - 20);
			}

		}
		break;
	case 'a': //move left
		if (playerX + 0.2 < 40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerX -= 0.5;
				break;
			}
			playerX += 0.2;
			playerRotation = 90;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX + 20, playerY, playerZ );
				scoreX = playerX + 1;
				scoreZ = playerZ;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX - 4, playerY + 1, playerZ );
				camera.center = Vector3f(playerX + 20, playerY - 3, playerZ );
			}

		}
		break;
	case 'd': //move right
		if (playerX - 0.2 > -40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerX += 0.5;
				break;
			}
			playerX -= 0.2;
			playerRotation = -90;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX - 20 , playerY, playerZ);
				scoreX = playerX - 1;
				scoreZ = playerZ;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX + 4, playerY + 1, playerZ );
				camera.center = Vector3f(playerX - 20, playerY - 3, playerZ );
			}

		}
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	if (thirdPerson) {
		scoreX = playerX;
		scoreZ = playerZ;
	}

	printf("X: %f and Z: %f", playerX, playerZ);
	checkPortalCollision(levelTwo);
	makeBoulderMove(levelTwo);
	checkObstacleCollision(levelTwo);
	checkCollectableCollision(levelTwo);
	checkGateCollision();
	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	float a = 5.0;

	switch (key) {
	/*case GLUT_KEY_UP:
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
		break;*/


	case GLUT_KEY_UP:
		if (playerZ + 0.2 < 40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerZ -= 0.5;
				break;
			}
			playerZ += 0.2;
			playerRotation = 0;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX, playerY, playerZ + 20);
				scoreX = playerX;
				scoreZ = playerZ + 1;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
				camera.center = Vector3f(playerX, playerY - 3, playerZ + 20);
			}

		}
		break;
	case GLUT_KEY_DOWN:
		if (playerZ - 0.2 > -40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerZ += 0.5;
				break;
			}
			playerZ -= 0.2;
			playerRotation = 180;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX, playerY, playerZ - 20);
				scoreX = playerX;
				scoreZ = playerZ - 1;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX, playerY + 1, playerZ + 4);
				camera.center = Vector3f(playerX, playerY - 3, playerZ - 20);
			}

		}
		break;
	case GLUT_KEY_LEFT:
		if (playerX + 0.2 < 40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerX -= 0.5;
				break;
			}
			playerX += 0.2;
			playerRotation = 90;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX + 20, playerY, playerZ);
				scoreX = playerX + 1;
				scoreZ = playerZ;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX - 4, playerY + 1, playerZ);
				camera.center = Vector3f(playerX + 20, playerY - 3, playerZ);
			}

		}
		break;
	case GLUT_KEY_RIGHT:
		if (playerX - 0.2 > -40 && !stumble) {
			if (checkAllWallCollisions(levelTwo) == true) {
				playerX += 0.5;
				break;
			}
			playerX -= 0.2;
			playerRotation = -90;
			if (firstPerson) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX - 20, playerY, playerZ);
				scoreX = playerX - 1;
				scoreZ = playerZ;
			}

			if (thirdPerson) {
				camera.eye = Vector3f(playerX + 4, playerY + 1, playerZ);
				camera.center = Vector3f(playerX - 20, playerY - 3, playerZ);
			}

		}
		break;

	}
	if (thirdPerson) {
		scoreX = playerX;
		scoreZ = playerZ;
	}
	printf("X: %f and Z: %f", playerX, playerZ);
	checkPortalCollision(levelTwo);
	makeBoulderMove(levelTwo);
	checkObstacleCollision(levelTwo);
	checkCollectableCollision(levelTwo);
	checkGateCollision();
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
			camera.rotateY(0.5);
		}
		else {
			camera.rotateY(-0.5);
		}
	}
	//glLoadIdentity();	//Clear Model_View Matrix

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
			camera.eye = Vector3f(playerX, playerY + 1, playerZ - 4);
			camera.center = Vector3f(playerX, playerY - 3, playerZ + 20);
			thirdPerson = true;
			firstPerson = false;
			scoreY = 2.f;
			scoreX = playerX;
			scoreZ = playerZ;
			toggleView = false;

		}
		else {
			if (playerRotation == -90) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX - 20, playerY, playerZ);
				scoreX = playerX - 1;
				scoreZ = playerZ;
			}
			else if (playerRotation == 90) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX + 20, playerY, playerZ);
				scoreX = playerX + 1;
				scoreZ = playerZ;
			}
			else if (playerRotation == 0) {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX, playerY, playerZ + 20);
				scoreX = playerX;
				scoreZ = playerZ + 1;
			}
			else {
				camera.eye = Vector3f(playerX, playerY + 0.5, playerZ);
				camera.center = Vector3f(playerX, playerY, playerZ - 20);
				scoreX = playerX;
				scoreZ = playerZ - 1;
			}
			firstPerson = true;
			thirdPerson = false;
			scoreY = 2.f;
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
	model_boulder.Load("Models/boulder/rock.3ds");
	model_crystal.Load("Models/crystal/rock.3ds");
	model_chest.Load("Models/chest/chest.3ds");
	model_portal.Load("Models/portal/StarGate.3ds");
	model_blade.Load("Models/blade/shrknlw.3ds");
	model_gate.Load("Models/gate/Gate.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_concrete.Load("Textures/Concrite.bmp");
	tex_grass.Load("Textures/texture-grass.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	loadBMP(&space_tex, "Textures/space_texture3.bmp", true);
}

void Animation() {
	if (stumble) {
		if (!getUp && stumbleRotation > -90.f) {
			stumbleRotation -= 1.f;
			playerY -= 0.01;
		}
		else {
			getUp = true;
			stumbleRotation += 1.f;
			playerY += 0.01;
			if (stumbleRotation < 0.f) {
				stumbleRotation = 0.0f;
				playerY = 1.5f;
				stumble = false;
			}
		}
	}

	if (bolderOneMove) {
		boulderOneX -= 0.1f;
	}

	if (bolderTwoMove) {
		boulderTwoX += 0.1f;
	}
	if (gameWin) {
		if (playerY < 3.f) {
			playerY += 0.005f;
			playerWinRotation += 5.f;
		}

	}
	bladeRotation += 6.f;
	glutPostRedisplay();
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

	myInit();

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(Special);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	glutTimerFunc(0, timer, 0);

	glutIdleFunc(Animation);

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	if (levelTwo) {
		playerX = 0.8;
		playerZ = -35.f;
	}


	//BONUS
	gameMusicBuffer.loadFromFile("game-music.wav");
	gameMusicSound.setBuffer(gameMusicBuffer);
	gameMusicSound.setVolume(100.0f);
	gameMusicSound.setLoop(true);
	gameMusicSound.play();

	collectableSoundBuffer.loadFromFile("collectable.wav");
	collectableSound.setBuffer(collectableSoundBuffer);
	collectableSound.setVolume(100.0f);

	obstacleSoundBuffer.loadFromFile("obstacle.wav");
	obstacleSound.setBuffer(obstacleSoundBuffer);
	obstacleSound.setVolume(100.0f);

	gameWinSoundBuffer.loadFromFile("game-win.wav");
	gameWinSound.setBuffer(gameWinSoundBuffer);
	gameWinSound.setVolume(100.0f);

	portalSoundBuffer.loadFromFile("portal.wav");
	portalSound.setBuffer(portalSoundBuffer);
	portalSound.setVolume(100.0f);

	/*gameEndSoundBuffer.loadFromFile("game-end.wav");
	gameEndSound.setBuffer(gameEndSoundBuffer);
	gameEndSound.setVolume(100.0f);*/

	glutMainLoop();
}