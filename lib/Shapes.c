#include "Shapes.h"
// OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>


// A function to draw a cone.
static void cone(double x, double y, double z,
                     double dx, double dy, double dz,
                     double th) {
  //GLfloat l,m,angle;
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};
  double nfaces = 32;
  double i;
  double len = 0.8;
  double rad = 0.8;

  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,1,0,0);
  glScaled(dx,dy,dz);

  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.0f, 0.0f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(0, 1, 0);
  for (i = 0; i <= 1.1; i += 1.0/nfaces) {
    float radians = i*M_PI*2.0f;
    float cr = cos(radians);
    float sr = sin(radians);
    glNormal3f(cr, 0.6667f, sr);
    glTexCoord2f(1.0, i);
    glVertex3f(rad*cr, 1-len, rad*sr);
  }
  glEnd();
  glPopMatrix();
}

static void cube(double x, double y, double z,
                     double dx, double dy, double dz,
                     double th) {
  double size = 1;

  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);

  x -= size/2;
  y -= size/2;
  z -= size/2;

  glBegin(GL_QUADS);
  glNormal3f(0, 0, -1);
  glVertex3f(x, y, z + size);
  glVertex3f(x, y + size, z + size);
  glVertex3f(x + size, y + size, z + size); 
  glVertex3f(x + size, y, z + size);
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glVertex3f(x + size, y, z);
  glVertex3f(x + size, y + size, z);
  glVertex3f(x, y + size, z); 
  glVertex3f(x, y, z);
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(1, 0, 0);
  glVertex3f(x, y + size, z);
  glVertex3f(x, y + size, z + size);
  glVertex3f(x, y, z + size); 
  glVertex3f(x, y, z);
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(-1, 0, 0);
  glVertex3f(x + size, y, z);
  glVertex3f(x + size, y, z + size);
  glVertex3f(x + size, y + size, z + size); 
  glVertex3f(x + size, y + size, z);
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(0, -1, 0);
  glVertex3f(x + size, y + size, z);
  glVertex3f(x + size, y + size, z + size);
  glVertex3f(x, y + size, z + size); 
  glVertex3f(x, y + size, z);
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(x + size, y, z);
  glVertex3f(x + size, y, z + size);
  glVertex3f(x, y, z); 
  glVertex3f(x, y, z+size);
  glEnd();



  glPopMatrix();

}

static void square(double x, double y, double z,
                       double dx, double dy, double dz, 
                       double th) {
  int size = 1;
  glPushMatrix();
  glNormal3f(0, 1, 0);
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(x + size, y + size, z);
  glTexCoord2f(1, 0); glVertex3f(x + size, y + size, z + size);
  glTexCoord2f(1, 1); glVertex3f(x, y + size, z + size); 
  glTexCoord2f(0, 1); glVertex3f(x, y + size, z);
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
  glRotated(th,1,0,0);
  glScaled(dx,dy,dz);

  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};

  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

  int epsilon = 1.0f;
  float angle;
  int nfaces = 20;
  int len = 4;

  glBegin(GL_TRIANGLE_STRIP);
  for(angle = 0.0f; angle <= 1.0f+epsilon; angle += 1.0f/nfaces)
  {
    float radians = angle*M_PI*2.0f;
    float cr = cos(radians);
    float sr = sin(radians);
    glNormal3f(cr, 0.0f, sr);
    glTexCoord2f(1.0f, angle);
    glVertex3f(cr, len*0.5f, sr);
    glTexCoord2f(0.0f, angle);
    glVertex3f(cr, len*-0.5f, sr);
  }
  glEnd();
  glPopMatrix();
}

