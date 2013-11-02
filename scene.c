
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
GLfloat tc1 = 1;
GLfloat tc0 = 0;


// Texture stuff
unsigned int texture[8];
double rep = 1;
GLuint  loop;

struct Texture {
    unsigned long ulDimensionX;
    unsigned long ulDimensionY;
    char *pcData;
};
typedef struct Texture Texture;

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

// This function has been taken from http://nehe.gamedev.net/ and
// modified to work on my computer, I hope it works on your's too!
int LoadBMP(char *szFilename, Texture *pTexture)
{
//  Texture *pTexture;
  FILE *filePointer;
  unsigned long ulSize;               // size of the image in bytes.
  unsigned long iCount;               // standard counter.
  unsigned short int usiPlanes;       // number of planes in image (must be 1)
  unsigned short int usiBpp;          // number of bits per pixel (must be 24)
  char cTempColor;                    // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
  if ((filePointer = fopen(szFilename, "rb"))==NULL)
  {
    printf("File Not Found : %s\n",szFilename);
    exit(0);
  }

  // seek through the bmp header, up to the width/height:
  fseek(filePointer, 18, SEEK_CUR);

  char buf[4];
  // read the width
  if ((iCount = fread(buf, 4, 1, filePointer)) != 1) {
  printf("Error reading width from %s.n", szFilename);
  return 0;
  }
  pTexture->ulDimensionX =
       ((unsigned long)buf[0]) |
      (((unsigned long)buf[1])<<8) |
      (((unsigned long)buf[2])<<16)|
      (((unsigned long)buf[3])<<24);

    // read the height
  if ((iCount = fread(buf, 4, 1, filePointer)) != 1) {
  printf("Error reading width from %s.n", szFilename);
  return 0;
  }
  pTexture->ulDimensionY =
       ((unsigned long)buf[0]) |
      (((unsigned long)buf[1])<<8) |
      (((unsigned long)buf[2])<<16)|
      (((unsigned long)buf[3])<<24);

  // calculate the size (assuming 24 bits or 3 bytes per pixel).
  ulSize = pTexture->ulDimensionX * pTexture->ulDimensionY * 3;

    // read the planes
  if ((fread(&usiPlanes, 2, 1, filePointer)) != 1)
  {
    printf("Error reading planes from %s.\n", szFilename);
    exit(0);
  }

  if (usiPlanes != 1)
  {
    printf("Planes from %s is not 1: %u\n", szFilename, usiPlanes);
    exit(0);
  }

  // read the bpp
  if ((iCount = fread(&usiBpp, 2, 1, filePointer)) != 1)
  {
    printf("Error reading bpp from %s.\n", szFilename);
    exit(0);
  }
  if (usiBpp != 24)
  {
    printf("Bpp from %s is not 24: %u\n", szFilename, usiBpp);
    exit(0);
  }

  // seek past the rest of the bitmap header.
  fseek(filePointer, 92, SEEK_CUR);

  // read the data.
  pTexture->pcData = (char *) malloc(ulSize);
  if (pTexture->pcData == NULL)
  {
    printf("Error allocating memory for color-corrected image data");
    exit(0);
  }

  if ((iCount = fread(pTexture->pcData, ulSize, 1, filePointer)) != 1)
  {
    printf("Error reading image data from %s.\n", szFilename);
    exit(0);
  }

  for (iCount=0;iCount<ulSize;iCount+=3) // reverse all of the colors. (bgr -> rgb)
  {
    cTempColor = pTexture->pcData[iCount];
    pTexture->pcData[iCount] = pTexture->pcData[iCount+2];
    pTexture->pcData[iCount+2] = cTempColor;
  }
  // we're done.
//  return pTexture;
  return 1;
}

// Load Bitmaps And Convert To Textures
int LoadGLTextures()
{
  // Load Texture
  Texture *TextureImage;
  // allocate space for texture
  TextureImage = (Texture *) malloc(sizeof(Texture));

  glGenTextures(8, &texture[0]);

  for(loop=0;loop<8;loop++)
  {
    switch(loop)
    {
      case 0:
        LoadBMP("tex/snow.bmp", TextureImage);
        break;

      case 1:
        LoadBMP("tex/bark.bmp", TextureImage);
        break;

      case 2:
        LoadBMP("tex/tree.bmp", TextureImage);
        break;

      case 3:
        LoadBMP("sb/bk.bmp", TextureImage);
        break;

      case 4:
        LoadBMP("sb/ft.bmp", TextureImage);
        break;

      case 5:
        LoadBMP("sb/lf.bmp", TextureImage);
        break;

      case 6:
        LoadBMP("sb/rt.bmp", TextureImage);
        break;

      case 7:
        LoadBMP("sb/up.bmp", TextureImage);
        break;

  }
    glBindTexture(GL_TEXTURE_2D, texture[loop]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->ulDimensionX, TextureImage->ulDimensionY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->pcData);
  }

  if (TextureImage)                      // If Texture Exists
  {
    if (TextureImage->pcData)            // If Texture Image Exists
    {
      free(TextureImage->pcData);        // Free The Texture Image Memory
    }
    free(TextureImage);                  // Free The Image Structure
  }

  return 1;                                   // Return The Status
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

// Display Routine
void display()
{
  shinyvec[0] = 2;
  shinyvec[1] = 256;
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
  dooohmmmme(camx, camy, camz, 20);
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
  if (!LoadGLTextures())                         // Jump To Texture Loading Routine
  {
    exit(0);                                // If Texture Didn't Load Return FALSE
  }
  // Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
