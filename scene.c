// Author: Vince Coghlan (vince.coghlan@colorado.edu)
// Date: 11/3/13
// Description: This is a 3d scene of some happy little trees

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "lib/Load.c"
#include "lib/Shapes.c"
// OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int fov=55;       // Field of view (for perspective)
double asp=2;     // Aspect ratio
double dim=5.0;   // Size of world
double lx = 0, ly = 0;    // Perspective angle
double camx = -15, camy = 1, camz = 0; // Camera Location
double vx = 0, vy = 0, vz = 0; // viewing direction

int width = 600;
int height = 600;

// Light values
int distance  =   20;  // Light distance
int local     =   0;  // Local Viewer Model
int ambient   =  30;  // Ambient intensity (%)
int diffuse   =  60;  // Diffuse intensity (%)
int specular  =  20;  // Specular intensity (%)
int zh        =  75;  // Light azimuth
float ylight  =   10;  // Elevation of light
double count  =   75;
int pause     =   1;
GLfloat tc1 = 1;
GLfloat tc0 = 0;

// key pres stuff
int apress = 0;
int dpress = 0;
int wpress = 0;
int spress = 0;
double timerad = 0;
double timerws = 0;

// fog stuff
GLuint filter;                      // Which Filter To Use
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
GLuint fogfilter= 1;                    // Which Fog To Use
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};

// Texture stuff
double rep = 1;

// Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

// sets the projection.  Written by professor
static void Project()
{
  // Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  // Undo previous transformations
  glLoadIdentity();
  gluPerspective(fov,asp,dim/4,4*dim);
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
}

// gets the height at that point in the terrain
double getheight(double x, double z) {
  double a = x/0.4+64;
  double b = z/0.4+64;

  return 0.01*map[(int)a][(int)b] - 1;
}

void drawCube(double x, double y, double s, double th)
{
   glPushMatrix();
   glTranslatef(x,getheight(y,x),y);
   glScalef(s,s,s);
   glRotated(th,0,1,0);
   glColor3f(0.27,0.27,0.27);
   glCallList(cube);
   glPopMatrix();
}

// draws the ground in strips, the idea came from 
// http://www.lighthouse3d.com/opengl/terrain/ but I do
// it in a different way
void drawGround(unsigned char map[128][128])
{
  int i, j;
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};

  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

  glColor3f(1, 1, 1);
  for (i = 0; i < 127; i++) {
    glBegin(GL_TRIANGLE_STRIP);
    for (j = 0; j < 127; j++) {
      glNormal3f(0, 1, 0);
      glVertex3f(0.4*(j - 64), 0.01*map[i+1][j] - 1, 0.4*(i+1 - 64));
      glVertex3f(0.4*(j - 64), 0.01*map[i][j] - 1, 0.4*(i - 64));
    }
    glEnd();
  }

}

void tree(double x, double y, double z,
          double dx, double dy, double dz,
          double th, int br) {
  int i;

  glPushMatrix();

  glTranslated(x,y,z);
  glRotated(th,0,0,1);
  glScaled(dx,dy,dz);

  glColor3f(.55, .27, .07);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  cylinder(0, .1, 0, .12, .25, .12, 0);
  glDisable(GL_TEXTURE_2D);
  for (i = 0; i < br; i++) {
    glColor3f(0, 1, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    cone(0, .25*i, 0, .6, .6, .6, 0);
    glDisable(GL_TEXTURE_2D);
  }
  glPopMatrix();
}

// creates a skybox and textures it, inspired by some code I
// found online and can no longer find. the texture came from
// http://www.hazelwhorley.com/textures.html
void dooohmmmme(float x, float y, float z, float len) {
  int i;
  x = x - len / 2;
  y = y - len / 2;
  z = z - len / 2;

  for(i = 3; i<8; i++) {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glColor3f(1, 1, 1);
    switch (i) {
      // back
      case 3:
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + len);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + len, z + len);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x + len, y + len, z + len); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x + len, y, z + len);
        glEnd();
        break;
      // front
      case 4:
        glBegin(GL_QUADS);		
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x + len, y, z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x + len, y + len, z); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x,	y + len, z);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x,	y, z);
        glEnd();
        break;
      // left
      case 5:
        glBegin(GL_QUADS);		
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x,	y + len,	z);	
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x,	y + len,	z + len);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x,	y, z + len);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x,	y, z);
        glEnd();
        break;
      // right
      case 6:
        glBegin(GL_QUADS);	
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x + len, y, z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x + len, y, z + len);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x + len, y + len,	z + len); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x + len, y + len,	z);
        glEnd();
        break;
      // up
      case 7:
        glBegin(GL_QUADS);		
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x + len, y + len, z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x + len, y + len, z + len); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x,	y + len,	z + len);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x,	y + len,	z);
        glEnd();
        break;
      default:
        exit(0);
        break;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
  }
}

// lighting
void lettherebelight(void)
{
  shinyvec[0] = 2;
  shinyvec[1] = 2;
  float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
  float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
  float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
  float Position[]  = {distance*Cos(count),ylight,distance*Sin(count),1.0};

  // Enable lighting
  glEnable(GL_LIGHTING);
  // Location of viewer for specular calculations
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
  // glColor sets ambient and diffuse color materials
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  // Enable light 0
  glEnable(GL_LIGHT0);
  // Set ambient, diffuse, specular components and position of light 0
  glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
  glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
  glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
  glLightfv(GL_LIGHT0,GL_POSITION,Position);

}

// basic fog, some code from nehe.gamedev.net
void fog(void) {
  glClearColor(0.5f,0.5f,0.5f,1.0f);          // We'll Clear To The Color Of The Fog ( Modified )

  glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
  glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
  glFogf(GL_FOG_DENSITY, 0.07f);              // How Dense Will The Fog Be
  glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
  glFogf(GL_FOG_START, 10.0f);             // Fog Start Depth
  glFogf(GL_FOG_END, 50.0f);               // Fog End Depth
  glEnable(GL_FOG);                   // Enables GL_FOG
}

// Display Routine
void display()
{
  // Erase the window and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  // Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

  // Undo previous transformations
  glLoadIdentity();
  vx = cos(lx);
  vy = ly;
  vz = sin(lx);

  // positions the camera based on the terrain
  camy = getheight(camz, camx) + 1.5;

  // This is the professor's code
  gluLookAt(camx,camy,camz,camx+vx,camy+vy,camz+vz,0,1,0);
  glShadeModel(GL_SMOOTH);
  // OpenGL should normalize normal vectors
  glEnable(GL_NORMALIZE);

  lettherebelight();
  fog();
  dooohmmmme(camx, camy, camz, 10);
  tree(camx+vx*M_PI, getheight(camz+vz*M_PI, camx+vx*M_PI)+0.3, camz+vz*M_PI, 1, 1, 1, 0, 2);
  drawCube(-2, 5, 0.5, 45);
  drawCube(0, -2, 1, 0);
  drawCube(-7, 3, 0.2, 100);

  // shows in wireframe
  //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  tree(-4, getheight(2, -4)+0.3, 2, 1, 1, 1, 0, 2);
  tree(-1.1, getheight(1, -1.1)+0.3, 1, 1, 1, 1, 0, 2);
  tree(-3, getheight(-4, -3)+0.3, -4, 1, 1, 1, 0, 2);
  tree(1, getheight(0.2, 1)+0.3, 0.2, 1, 1, 1, 0, 2);
  tree(3, getheight(-4, 3)+0.3, -4, 1, 1, 1, 0, 4);
  tree(5, getheight(3, 5)+0.3, 3, 1, 1, 1, 0, 2);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  drawGround(map);
  glDisable(GL_TEXTURE_2D);

  // Render the scene
  glFlush();
  // Make the rendered scene visible
  glutSwapBuffers();
}

// Special Key Function
void special(int key,int x,int y)
{
  switch (key) {
    case(GLUT_KEY_UP):
      ly += 0.1;
      break;
    case(GLUT_KEY_DOWN):
      ly -= 0.1;
      break;
  }
  Project();
  // Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

// Normal Key Function
void key(unsigned char ch,int x,int y)
{
  switch (ch) {
    // Exit on ESC
    case(27):
      exit(0);
      break;
    // spacebar
    case(32):
      pause = !pause;
      break;
    case('0'):
      camx = camz = 0;
      lx = ly = 0;
      fov = 55;
      break;
    case('['):
      tc0 += 0.005;
      break;
    case(']'):
      tc0 -= 0.005;
      break;
    case('+'):
      tc1 += 0.005;
      break;
    case('-'):
      tc1 -= 0.005;
      break;
    case('w'):
      wpress = 1;
      break;
    case('s'):
      spress = 1;
      break;
    case('a'):
      apress = 1;
      break;
    case('d'):
      dpress = 1;
      break;
    case('q'):
      if (pause) {
        count += 5;
      }
      break;
    case('e'):
      if (pause) {
        count -= 5;
      }
      break;
  }
  Project();
  // Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

void keyup(unsigned char ch,int x,int y) {
  switch (ch) {
    case('d'):
      dpress = 0;
      break;
    case('a'):
      apress = 0;
      break;
    case('w'):
      wpress = 0;
      break;
    case('s'):
      spress = 0;
      break;
  }
}

// Reshape function written by the professor
void reshape(int width,int height)
{
  // Ratio of the width to the height of the window
  asp = (height>0) ? (double)width/height : 1;
  // Set the viewport to the entire window
  glViewport(0,0, width,height);
  // Set projection
  Project();
}

void update()
{
  if (dpress || apress) {
    timerad += 0.1;
  }
  else {
    timerad -= 0.1;
  }
  if (timerad < 0) {
    timerad = 0;
  }
  if (timerad > 1.5) {
    timerad = 1.5;
  }

  if (dpress && apress) {
  }
  else if (dpress) {
    lx += 0.01*timerad;
  }
  else if (apress) {
    lx -= 0.01*timerad;
  }

  if (wpress || spress) {
    timerws += 0.1;
  }
  else {
    timerws -= 0.1;
  }
  if (timerad < 0) {
    timerad = 0;
  }
  if (timerad > 1.0) {
    timerad = 1.0;
  }

  if (wpress && spress) {
  }
  else if (wpress) {
    camx += vx*0.05;
    camz += vz*0.05;
  }
  else if (spress) {
    camx -= vx*0.05;
    camz -= vz*0.05;
  }

  glutPostRedisplay();
  glutTimerFunc(25, update, 0);
}

// GLut initialiation and main loop
int main(int argc,char* argv[])
{
  // Initialize GLUT and process user parameters
  glutInit(&argc,argv);
  // Request double buffered, true color window with Z buffering at 600x600
  glutInitWindowSize(width,height);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  // Create the window
  glutCreateWindow("Happy Little Trees (Vince Coghlan)");
  // Tell GLUT to call "display" when the scene should be drawn
  glutDisplayFunc(display);
  // Tell GLUT to call "reshape" when the window is resized
  glutReshapeFunc(reshape);
  // Tell GLUT to call "special" when an arrow key is pressed
  glutSpecialFunc(special);
  // Tell GLUT to call "key" when a key is pressed
  glutKeyboardFunc(key);
  glutKeyboardUpFunc(keyup);
  glutTimerFunc(25, update, 0);
  // Load Textures
  if (!LoadGLTextures())                         // Jump To Texture Loading Routine
  {
    exit(0);                                // If Texture Didn't Load Return FALSE
  }
  loadterrain();
  loadCube("data/objects/cube.obj");

  // Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
