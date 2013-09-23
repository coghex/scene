/*
 *  Object Examples
 *  'm' to switch modes (objects)
 *  'a' to toggle axes
 *  '0' snaps angles to 0,0
 *  arrows to rotate the world
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int axes=1;       //  Display axes
int mode=0;       //  What to display
double zoom=1;
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
  char    buf[LEN];
  char*   ch=buf;
  va_list args;
  //  Turn the parameters into a character string
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);
  //  Display the characters one at a time at the current raster position
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();
  //  Perspective transformation
  gluPerspective(fov,asp,dim/4,4*dim);
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);
  //  Cube
  glBegin(GL_QUADS);
  //  Front
  glColor3f(1,0,0);
  glVertex3f(-1,-1, 1);
  glVertex3f(+1,-1, 1);
  glVertex3f(+1,+1, 1);
  glVertex3f(-1,+1, 1);
  //  Back
  glColor3f(0,0,1);
  glVertex3f(+1,-1,-1);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,+1,-1);
  glVertex3f(+1,+1,-1);
  //  Right
  glColor3f(1,1,0);
  glVertex3f(+1,-1,+1);
  glVertex3f(+1,-1,-1);
  glVertex3f(+1,+1,-1);
  glVertex3f(+1,+1,+1);
  //  Left
  glColor3f(0,1,0);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,-1,+1);
  glVertex3f(-1,+1,+1);
  glVertex3f(-1,+1,-1);
  //  Top
  glColor3f(0,1,1);
  glVertex3f(-1,+1,+1);
  glVertex3f(+1,+1,+1);
  glVertex3f(+1,+1,-1);
  glVertex3f(-1,+1,-1);
  //  Bottom
  glColor3f(1,0,1);
  glVertex3f(-1,-1,-1);
  glVertex3f(+1,-1,-1);
  glVertex3f(+1,-1,+1);
  glVertex3f(-1,-1,+1);
  //  End
  glEnd();
  //  Undo transformations
  glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
  const double len=1.5;  //  Length of axes
  //  Erase the window and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  //  Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  //  Undo previous transformations
  glLoadIdentity();
  //  Set view angle
  double Ex = -2*dim*Sin(th)*Cos(ph);
  double Ey = +2*dim        *Sin(ph);
  double Ez = +2*dim*Cos(th)*Cos(ph);
  gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
  //  Decide what to draw
  //  Draw cubes
  cube(1,1,1 , 0.3,0.3,0.3 , 0);
  cube(0,0,0 , 0.2,0.2,0.2 , 0);
  //  White
  glColor3f(1,1,1);
  //  Five pixels from the lower left corner of the window
  glWindowPos2i(5,5);
  //  Print the text string
  Print("Angle=%d,%d  Dim=%.1f FOV=%d",th,ph,dim,fov);
  //  Render the scene
  glFlush();
  //  Make the rendered scene visible
  glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
  //  Right arrow key - increase angle by 5 degrees
  if (key == GLUT_KEY_RIGHT)
    th += 5;
  //  Left arrow key - decrease angle by 5 degrees
  else if (key == GLUT_KEY_LEFT)
    th -= 5;
  //  Up arrow key - increase elevation by 5 degrees
  else if (key == GLUT_KEY_UP)
    ph += 5;
  //  Down arrow key - decrease elevation by 5 degrees
  else if (key == GLUT_KEY_DOWN)
    ph -= 5;
  //  Keep angles to +/-360 degrees
  th %= 360;
  ph %= 360;
  Project();
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
  //  Exit on ESC
  if (ch == 27)
    exit(0);
  //  Reset view angle
  else if (ch == '0')
    th = ph = 0;
  //  Toggle axes
  else if (ch == 'a' || ch == 'A')
    axes = 1-axes;
  else if (ch == '+')
    dim += 0.2;
  else if (ch == '-' && dim>1)
    dim -= 0.2;
  else if (ch == 'w' && ch<179)
    fov++;
  else if (ch == 's' && ch>1)
    fov--;
  Project();
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
  //  Ratio of the width to the height of the window
  asp = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Set projection
  Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
  glutInit(&argc,argv);
  //  Request double buffered, true color window with Z buffering at 600x600
  glutInitWindowSize(600,600);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  //  Create the window
  glutCreateWindow("Happy Little Trees");
  //  Tell GLUT to call "display" when the scene should be drawn
  glutDisplayFunc(display);
  //  Tell GLUT to call "reshape" when the window is resized
  glutReshapeFunc(reshape);
  //  Tell GLUT to call "special" when an arrow key is pressed
  glutSpecialFunc(special);
  //  Tell GLUT to call "key" when a key is pressed
  glutKeyboardFunc(key);
  //  Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
