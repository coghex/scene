
// Author: Vince Coghlan (vince.coghlan@colorado.edu)
// Date: 10/10/13
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
int fov=55;       // Field of view (for perspective)
double asp=2;     // Aspect ratio
double dim=5.0;   // Size of world
double lx = 0, ly = 0;    // Perspective angle
double camx = -15, camy = 1, camz = 0; // Camera Location
double vx = 0, vy = 0, vz = 0; // viewing direction

// Light values
int distance  =   10;  // Light distance
int local     =   0;  // Local Viewer Model
int ambient   =  50;  // Ambient intensity (%)
int diffuse   =  70;  // Diffuse intensity (%)
int specular  =  10;  // Specular intensity (%)
float shinyvec[3];    // Shininess (value)
int zh        =  75;  // Light azimuth
float ylight  =   4;  // Elevation of light
double count  =   180;
int pause     =   1;

// Texture stuff
unsigned int texture[2];
double rep = 1;

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

/*
 *  Reverse n bytes
 */
static void Reverse(void* x,const int n)
{
   int k;
   char* ch = (char*)x;
   for (k=0;k<n/2;k++)
   {
      char tmp = ch[k];
      ch[k] = ch[n-1-k];
      ch[n-1-k] = tmp;
   }
}

// All this code loads the BMP image, it can be found in the CSCIx229 library
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

unsigned int LoadTexBMP(const char* file)
{
   unsigned int   texture;    // Texture name
   FILE*          f;          // File pointer
   unsigned short magic;      // Image magic
   unsigned int   dx,dy,size; // Image dimensions
   unsigned short nbp,bpp;    // Planes and bits per pixel
   unsigned char* image;      // Image data
   unsigned int   k;          // Counter
   int            max;        // Maximum texture dimensions

   //  Open file
   f = fopen(file,"rb");
   if (!f) Fatal("Cannot open file %s\n",file);
   //  Check image magic
   if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
   if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
   //  Seek to and read header
   if (fseek(f,16,SEEK_CUR) || fread(&dx ,4,1,f)!=1 || fread(&dy ,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     Fatal("Cannot read header from %s\n",file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic==0x424D)
   {
      Reverse(&dx,4);
      Reverse(&dy,4);
      Reverse(&nbp,2);
      Reverse(&bpp,2);
      Reverse(&k,4);
   }
   //  Check image parameters
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
   if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
   if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
   if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
   if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k*=2);
   if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   size = 3*dx*dy;
   image = (unsigned char*) malloc(size);
   if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   if (fseek(f,20,SEEK_CUR) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Sanity check
   ErrCheck("LoadTexBMP");
   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}

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
// A function to draw a cone.  Altered from code from
// an anonymous blog post
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

// This was also taken from an anonymous blog post
void drawGround(void)
{
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};
  float i, j;
  float y = -0.4;
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

  GLfloat length = 40.0f;
  GLfloat width = 40.0f;
  GLfloat step = 20.0f;

  glColor3f(0.8f, .8f, 1.0f);
  for(i = -length; i < length; i += step/length){
    for(j = -width; j < width; j += step/width) {
      glBegin(GL_POLYGON);
      glNormal3f(0.0, 1.0, 0.0);
      glTexCoord2d(0,0);glVertex3f(i,y,j);
      glTexCoord2d(0,1);glVertex3f(i,y,j+step/width+0.2);
      glTexCoord2d(1,1);glVertex3f(i+step/length+0.2,y,j+step/width+0.2);
      glTexCoord2d(1,0);glVertex3f(i+step/length+0.2,y,j);
      glEnd();
    }
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
// Display Routine
void display()
{
  shinyvec[0] = 2;
  shinyvec[1] = 256;
  // Erase the window and the depth buffer
  glClearColor(0.1, 0.5, 0.7, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  // Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

  // Undo previous transformations
  glLoadIdentity();
  vx = cos(lx);
  vy = ly;
  vz = sin(lx);

  // This is the professor's code
  gluLookAt(camx,camy,camz,camx+vx,camy+vy,camz+vz,0,1,0);
  glShadeModel(GL_SMOOTH);
  // Translate intensity to color vectors
  float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
  float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
  float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
  // Light position
  float Position[]  = {distance*Cos(count),ylight,distance*Sin(count),1.0};
  // OpenGL should normalize normal vectors
  glEnable(GL_NORMALIZE);
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
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  drawGround();
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
  if (!pause) {
    count += 1;
    if (count >= 360)
      count = 0;
    glutPostRedisplay();
  }
  glutTimerFunc(25, update, 0);
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
  glutTimerFunc(25, update, 0);
  // Load Textures
  texture[0] = LoadTexBMP("snow.bmp");
  texture[1] = LoadTexBMP("bark.bmp");
  texture[2] = LoadTexBMP("tree.bmp");
  // Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
