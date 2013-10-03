// File: scene.c
// Author: Vince Coghlan (vince.coghlan@colorado.edu)
// Date: 10/2/13
// Description: This is a 3d scene of some happy little trees

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
// OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=1;       // Display axes
int mode=1;       // What to display
int fov=55;       // Field of view (for perspective)
double asp=2;     // Aspect ratio
double dim=5.0;   // Size of world
double lx = 0, ly = 0;    // Perspective angle
double camx = -15, camy = 1, camz = 0; // Camera Location
double vx = 0, vy = 0, vz = 0; // viewing direction

// Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

// This was written by the professor
// Convenience routine to output raster text
// Use VARARGS to make this more flexible

#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
  char    buf[LEN];
  char*   ch=buf;
  va_list args;
  // Turn the parameters into a character string
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);
  // Display the characters one at a time at the current raster position
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

// sets the projection.  Written by professor
static void Project()
{
  // Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  // Undo previous transformations
  glLoadIdentity();
  if (mode)
    gluPerspective(fov,asp,dim/4,4*dim);
  // Orthogonal projection
  else
    glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();}

// A function to draw a cone.  Altered from code from
// an anonymous blog post
static void cone(double x, double y, double z,
                     double dx, double dy, double dz,
                     double th) {
  GLfloat l,m,angle;
  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,1,0,0);
  glScaled(dx,dy,dz);

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0.0f, 0.0f, 3.0f);

  for(angle = 0.0f; angle < (2.0f*M_PI); angle += (M_PI/8.0f))
    {
    // x and y of next vertex
    l = 2.0f*sin(angle);
    m = 2.0f*cos(angle);
    // next vertex for the triangle fan
    glVertex2f(l, m);
    }
  glEnd();

  // Now for the bottom. (another cone of height 0 off the bottom)
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(0.0f, 0.0f);
  for(angle = 0.0f; angle < (2.0f*M_PI); angle += (M_PI/8.0f))
    {
    l = 2.0f*sin(angle);
    m = 2.0f*cos(angle);
    glVertex2f(m, l);
    }
  glEnd();

  glPopMatrix();
}

// A function to draw a cylinder
static void cylinder(double x, double y, double z,
                     double dx, double dy, double dz,
                     double th) {
  // Save transformation
  glPushMatrix();
  // Offset
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);

  float radius = 1;
  float halfLength = 2;
  int slices = 200;
  int i;

  // code was modified from an anonymous blog post
  for(i=0; i<slices; i++) {
    float theta = ((float)i)*2.0*M_PI;
    float nextTheta = ((float)i+1)*2.0*M_PI;
    glBegin(GL_TRIANGLE_STRIP);
    // vertex at middle of end
    glVertex3f(0.0, halfLength, 0.0);
    // vertices at edges of circle
    glVertex3f(radius*Cos(theta), halfLength, radius*Sin(theta));
    glVertex3f (radius*Cos(nextTheta), halfLength, radius*Sin(nextTheta));
    // the same vertices at the bottom of the cylinder
    glVertex3f (radius*Cos(nextTheta), -halfLength, radius*Sin(nextTheta));
    glVertex3f(radius*Cos(theta), -halfLength, radius*Sin(theta));
    glVertex3f(0.0, -halfLength, 0.0);
    glEnd();
  }
  glPopMatrix();
}

// This was also taken from an anonymous blog post
void drawGround(void)
{
  GLfloat fExtent = 20.0f;
  GLfloat fStep = 1.0f;
  GLfloat y = -0.4f; 
  GLfloat iStrip, iRun;

  glColor3f(0.8f, .8f, 1.0f);
  for(iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
	{
	  glBegin(GL_TRIANGLE_STRIP);
	  glNormal3f(0.0f, 1.0f, 0.0f); // All teh normals face up

	  for(iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
		{
		  glVertex3f(iStrip, y, iRun);
		  glVertex3f(iStrip + fStep, y, iRun);
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
  cylinder(0, .1, 0, .12, .25, .12, 0);

  for (i = 0; i < br; i++) {
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(0, 0, 0);
    cone(0, .25*i, 0, .2, .2, .2, 270);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glColor3f(0, .3, 0);
    cone(0, .25*i, 0, .2, .2, .2, 270);  }
  glPopMatrix();
}
// Display Routine
void display()
{
  // Erase the window and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  // Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);

  // Undo previous transformations
  glLoadIdentity();
  vx = cos(lx);
  vy = ly;
  vz = sin(lx);

  if (mode)
  {
    // Set view angle
    gluLookAt(camx,camy,camz,camx+vx,camy+vy,camz+vz,0,1,0);
  }
  else
  {
    glRotated(ly*3+4, 1, 0, 0);
    glRotated(lx*3, 0, 1, 0);
    glTranslated(-camz, -camy, -camx);
  }

  // Decide what to draw
  // shows in wireframe
  //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  tree(-4, 0, 2, 1, 1, 1, 0, 2);
  tree(-1.1, 0, 1, 1, 1, 1, 0, 2);
  tree(-3, 0, -4, 1, 1, 1, 0, 2);
  tree(1, 0, .2, 1, 1, 1, 0, 2);
  tree(3, 0, -4, 1, 1, 1, 0, 4);
  tree(5, 0, 3, 1, 1, 1, 0, 2);
  tree(0, 0, 0, 0.2, 0.2, 0.2, 45, 2);
  drawGround();

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
  // Exit on ESC
  switch (ch) {
    case(27):
      exit(0);
      break;
    // Reset view angle
    case('0'):
      camx = camz = 0;
      lx = ly = 0;
      fov = 55;
      break;
    case('['):
      dim += 0.2;
      break;
    case(']'):
      if (dim>1)
        dim -= 0.2;
      break;
    case('+'):
      if (ch<179)
        fov++;
      break;
    case('-'):
      if(ch>1)
        fov--;
      break;
    case('m'):
      mode = !mode;
      if (mode == 0){
        camx = camz = 0;
        lx = ly = 0;
        fov = 55;
      } else {
        camx = -15;
        camy = 1;
        camz = 0;
      }
      break;
    case('w'):
      camx += vx*0.3;
      camz += vz*0.3;
      break;
    case('s'):
      camx -= vx*0.3;
      camz -= vz*0.3;
      break;
    case('a'):
      lx -= 0.1;
      break;
    case('d'):
      lx += 0.1;
      break;
  }
  Project();
  // Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
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

// GLut initialiation and main loop
int main(int argc,char* argv[])
{
  // Initialize GLUT and process user parameters
  glutInit(&argc,argv);
  // Request double buffered, true color window with Z buffering at 600x600
  glutInitWindowSize(600,600);
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
  // Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}