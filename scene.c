// Author: Vince Coghlan (vince.coghlan@colorado.edu)
// Date: 11/3/13
// Description: This is a 3d scene of some happy little trees

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
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
double lx = 0, ly = -0.5;    // Perspective angle
double camx = -4, camy = 1, camz = 1; // Camera Location
double vx = 0, vy = 0, vz = 0; // viewing direction

int width = 600;
int height = 600;
int mapdiff = 0;
int timer = 0;

double randomlistx[128];
double randomlisty[128];
double randomlistrockx[128];
double randomlistrocky[128];
double randomlistangle[128];
double randomlistsize[128];
int randomtreesize[128];

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

int area = 1;

// key pres stuff
int apress = 0;
int dpress = 0;
int wpress = 0;
int spress = 0;
double timerad = 0;
double timerws = 0;
int shift = 0;

// fog stuff
GLuint filter;                      // Which Filter To Use
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
GLuint fogfilter= 1;                    // Which Fog To Use
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};

// Texture stuff
double rep = 1;
double scale = 0.02;

// rock list
GLuint rock;

// collision detection
double treemap[128][3];
int treecount = 0;

// Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

// Print was not written by me but provided by the professor
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
  char  buf[LEN];
  char*  ch=buf;
  va_list args;
  //  Turn the parameters into a character string
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);
  //  Display the characters one at a time at the current raster position
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

// We need to set a new 2D enviornment if we want the text on top
void begintext(int h, int w){
  glMatrixMode(GL_PROJECTION);
	// so we can restore the perspective view
	glPushMatrix();
	// reset the matrix
 	glLoadIdentity();
	glOrtho( 0, h, 0, w, 0, 1 );
  // go back to modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// make text on top
  glDisable(GL_DEPTH_TEST);
}

void endtext(){
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	// pop the old matrix
 	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
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

double treediff(int i, int j) {
  return 0.75*(treemap[i][2] + treemap[j][2]);
}

int randnum(int limit) {
/* return a random number between 0 and limit inclusive.
 */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

void resetarea(void) {
  camx = -4;
  camz = 1;
  timer = 0;
  switch (area) {
    case (1):
      mapdiff = loadterrain("data/terrain/ski.tga");
      break;
    case (2):
      mapdiff = loadterrain("data/terrain/ski2.tga");
      break;
    default:
      mapdiff = loadterrain("data/terrain/ski.tga");
      break;
  }
}

void initrandomnumbers(void) {
  int i;
  srand(time(NULL));
  for (i = 0; i < 128; i++)
  {
    randomlistx[i] = randnum(10);
    randomlisty[i] = randnum(50)/10.0;
    randomlistrockx[i] = 3 - randnum(6);
    randomlistrocky[i] = randnum(50) + 2;
    randomlistangle[i] = randnum(360);
    randomlistsize[i] = randnum(50)/50.0 + 0.1;
    randomtreesize[i] = randnum(4) + 2;
  }
}

int collision(void) {
  int i = 0;
  int j = 0;

  //0 = nocolllision
  //1 = collisionleft
  //2 = collisionright
  //3 = collisionfront
  //4 = collisionback
  //5 = collisionfrontleft
  //6 = collisionfrontright
  //7 = collisionbackleft
  //8 = collisionbackright
  //9 = i dunno

  for (j=0;j<128;j++) {
    if ((i==j)||(treemap[0][0] == treemap[j][0] && treemap[0][1] == treemap[j][1])){}
    else if (((treemap[0][0] - treemap[j][0]) < treediff(0, j)) &&
        ((treemap[0][0] - treemap[j][0]) > -treediff(0, j)) &&
        ((treemap[0][1] - treemap[j][1]) < treediff(0, j)) &&
        ((treemap[0][1] - treemap[j][1]) > -treediff(0, j))) {
      if (wpress && apress && !dpress && !spress) {
        camx -= vx*0.05;
        camz -= vz*0.05;
        lx += 0.01;
        timerws = 0;
        timerad = 0;
        return 5;
      }
      else if (wpress && dpress && !apress && !spress) {
        camx -= vx*0.05;
        camz -= vz*0.05;
        lx -= 0.01;
        timerws = 0;
        timerad = 0;
        return 6;
      }
      else if (spress && apress && !wpress && !dpress) {
        camx += vx*0.05;
        camz += vz*0.05;
        lx += 0.01;
        timerws = 0;
        timerad = 0;
        return 7;
      }
      else if (spress && dpress && !wpress && !apress) {
        camx += vx*0.05;
        camz += vz*0.05;
        lx -= 0.01;
        timerws = 0;
        timerad = 0;
        return 8;
      }
      else if (apress && !wpress && !spress && !dpress) {
        lx += 0.01;
        timerad = 0;
        return 1;
      }
      else if (dpress && !wpress && !spress && !apress) {
        lx -= 0.01;
        timerad = 0;
        return 2;
      }
      else if (wpress && !dpress && !spress && !apress) {
        camx -= vx*0.05;
        camz -= vz*0.05;
        timerws = 0;
        return 3;
      }
      else if (spress && !wpress && !apress && !dpress) {
        camx += vx*0.05;
        camz += vz*0.05;
        timerws = 0;
        return 4;
      }
      else {
        return 9;
      }
    }
  }

  return 0;
}

// gets the height at that point in the terrain
double getheight(double x, double z) {
  double a = x/0.4+64;
  double b = z/0.4+64;

  if (b > 124) {
    return scale*map[(int)a][(int)b - 124] - 1 - scale*(mapdiff);
  }
  else {
    return scale*map[(int)a][(int)b] - 1;
  }
}

void drawRock(double x, double y, double s, double th)
{
  glPushMatrix();
  glTranslatef(x,getheight(y,x),y);
  glScalef(s,s,s);
  glRotated(th,0,1,0);
  glColor3f(0.27,0.27,0.27);
  glCallList(rock);
  glPopMatrix();
  treemap[treecount][0] = x;
  treemap[treecount][1] = y;
  treemap[treecount][2] = s;
  treecount++;
}

// draws the ground in strips, the idea came from 
// http://www.lighthouse3d.com/opengl/terrain/ but I do
// it in a different way
void drawGround(unsigned char map[128][128])
{
  int i, j, k;
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

  // if you dont split this then you can see the strips due to how it
  // maps the texture, this looks the most alright
  glColor3f(1, 1, 1);
  for (i = 0; i < 127; i++) {
    glBegin(GL_TRIANGLE_STRIP);
    for (j = 0; j < 63; j++) {
      if (i%2) {
        glNormal3f(0, 1, 0);
        glTexCoord2f(1, 0); glVertex3f(0.4*(2*j - 64), scale*map[i+1][2*j] - 1, 0.4*(i+1 - 64));
        glTexCoord2f(1, 1); glVertex3f(0.4*(2*j - 64), scale*map[i][2*j] - 1, 0.4*(i - 64));
        glTexCoord2f(0, 0); glVertex3f(0.4*(2*j - 63), scale*map[i+1][2*j+1] - 1, 0.4*(i+1 - 64));
        glTexCoord2f(0, 1); glVertex3f(0.4*(2*j - 63), scale*map[i][2*j+1] - 1, 0.4*(i - 64));
      }
      else {
        glNormal3f(0, 1, 0);
        glTexCoord2f(1, 1); glVertex3f(0.4*(2*j - 64), scale*map[i+1][2*j] - 1, 0.4*(i+1 - 64));
        glTexCoord2f(0, 1); glVertex3f(0.4*(2*j - 64), scale*map[i][2*j] - 1, 0.4*(i - 64));
        glTexCoord2f(0, 0); glVertex3f(0.4*(2*j - 63), scale*map[i+1][2*j+1] - 1, 0.4*(i+1 - 64));
        glTexCoord2f(1, 0); glVertex3f(0.4*(2*j - 63), scale*map[i][2*j+1] - 1, 0.4*(i - 64));
      }
    }
    glEnd();
  }

  glColor3f(1, 1, 1);
  for (i = 0; i < 127; i++) {
    glBegin(GL_TRIANGLE_STRIP);
    for (j = 0; j < 63; j++) {
      k = j + 62;
      if (i%2) {
        glNormal3f(0, 1, 0);
        glTexCoord2f(1, 0); glVertex3f(0.4*(2*k - 64), scale*map[i+1][2*j] - 1 - scale*(mapdiff), 0.4*(i+1 - 64));
        glTexCoord2f(1, 1); glVertex3f(0.4*(2*k - 64), scale*map[i][2*j] - 1 - scale*(mapdiff), 0.4*(i - 64));
        glTexCoord2f(0, 0); glVertex3f(0.4*(2*k - 63), scale*map[i+1][2*j+1] - 1 - scale*(mapdiff), 0.4*(i+1 - 64));
        glTexCoord2f(0, 1); glVertex3f(0.4*(2*k - 63), scale*map[i][2*j+1] - 1 - scale*(mapdiff), 0.4*(i - 64));
      }
      else {
        glNormal3f(0, 1, 0);
        glTexCoord2f(1, 1); glVertex3f(0.4*(2*k - 64), scale*map[i+1][2*j] - 1 - scale*(mapdiff), 0.4*(i+1 - 64));
        glTexCoord2f(0, 1); glVertex3f(0.4*(2*k - 64), scale*map[i][2*j] - 1 - scale*(mapdiff), 0.4*(i - 64));
        glTexCoord2f(0, 0); glVertex3f(0.4*(2*k - 63), scale*map[i+1][2*j+1] - 1 - scale*(mapdiff), 0.4*(i+1 - 64));
        glTexCoord2f(1, 0); glVertex3f(0.4*(2*k - 63), scale*map[i][2*j+1] - 1 - scale*(mapdiff), 0.4*(i - 64));
      }
    }
    glEnd();
  }


  glDisable(GL_TEXTURE_2D);
}

void tree(double x, double y, double z,
          double dx, double dy, double dz,
          double th, int br) {
  int i;

  glPushMatrix();

  glTranslated(x,y,z);
  glRotated(th,0,0,1);
  glScaled(dx,dy,dz);

  treemap[treecount][0] = x;
  treemap[treecount][1] = z;
  treemap[treecount][2] = 0.4*(dx*dy*dz);

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
  treecount++;
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

void drawgoal(int x) {
  int i;
  glPushMatrix();
  glTranslatef(x,getheight(0,x),0);

  glColor3f(1, 1, 1);

  for (i=0;i<12;i++) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[i+8]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f(0, 6, i-5);
    glTexCoord2f(1, 0); glVertex3f(0, 5, i-5);
    glTexCoord2f(0, 0); glVertex3f(0, 5, i-6);
    glTexCoord2f(0, 1); glVertex3f(0, 6, i-6);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }

  glBegin(GL_QUADS);
  glVertex3f(0, -2, -6);
  glVertex3f(0, -2, -5.8);
  glVertex3f(0, 5, -5.8);
  glVertex3f(0, 5, -6);
  glEnd();

  glBegin(GL_QUADS);
  glVertex3f(0, -2, 6);
  glVertex3f(0, -2, 5.8);
  glVertex3f(0, 5, 5.8);
  glVertex3f(0, 5, 6);
  glEnd();

  glPopMatrix();
}

void win(void) {
  dpress = 0;
  wpress = 0;
  apress = 0;
  spress = 0;
  int sec = ((timer/1000)%60);
  int min = sec/60;

  glWindowPos2i(width/2-60,height/2);
  begintext(width, height);
  Print("Finished!");
  endtext();
  glWindowPos2i(width/2-75,height/2 - 20);
  begintext(width, height);
  if (sec < 10) {
    Print("Time of %d:0%d", min, sec);
  }
  else {
    Print("Time of %d:%d", min, sec);
  }
  endtext();
  glWindowPos2i(width/2-120,height/2 - 40);
  begintext(width, height);
  Print("choose your map by number");
  endtext();
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
  int i, e;
  double a, b, c, d;
  int sec = ((timer/1000)%60);
  int min = sec/60;

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

  // positions the camera based on the player
  camy = getheight(camz+vz*4, camx+vx*4) + 3;

  // This is the professor's code
  gluLookAt(camx,camy,camz,camx+vx,camy+vy,camz+vz,0,1,0);
  glShadeModel(GL_SMOOTH);
  // OpenGL should normalize normal vectors
  glEnable(GL_NORMALIZE);

  lettherebelight();
  fog();
  dooohmmmme(camx, camy, camz, 10);
  tree(camx+vx*4, getheight(camz+vz*4, camx+vx*4)+0.3, camz+vz*4, 1, 1, 1, 0, 2);
  drawgoal(60);

  for (i = 0; i < 50; i++)
  {
    a = i + randomlistx[i];
    b = i + randomlistx[128 - i];
    c = 4 + randomlisty[i];
    d = -4 - randomlisty[128 - i];
    e = randomtreesize[i];
    tree(a, getheight(c, a)+0.3, c, 1, 1, 1, 0, e);
    tree(b, getheight(d, b)+0.3, d, 1, 1, 1, 0, e);
  }

  for (i = 0; i < 10; i++) {
    a = randomlistrocky[i];
    b = randomlistrockx[i];
    c = randomlistsize[i];
    d = randomlistangle[i];
    drawRock(a, b, c, d);
  }

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  drawGround(map);
  glDisable(GL_TEXTURE_2D);

  if (camx > 58 && camz > -6 && camz < 6) {
    win();
  }
  else {
    glWindowPos2i(5,5);
    begintext(width, height);
    if (sec < 10) {
      Print("%d:0%d", min, sec);
    }
    else {
      Print("%d:%d", min, sec);
    }
    endtext();

  }

  treecount = 0;

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
    case('W'):
      wpress = 1;
      shift = 1;
      break;
    case('S'):
      spress = 1;
      shift = 1;
      break;
    case('A'):
      apress = 1;
      break;
    case('D'):
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
    case('1'):
      area = 1;
      resetarea();
      break;
    case('2'):
      area = 2;
      resetarea();
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
    case('W'):
      wpress = 0;
      shift = 0;
      break;
    case('S'):
      spress = 0;
      shift = 0;
      break;
    case('A'):
      apress = 0;
      break;
    case('D'):
      dpress = 0;
      break;

  }
}

// Reshape function written by the professor
void reshape(int w,int h)
{
  width = w;
  height = h;
  // Ratio of the width to the height of the window
  asp = (h>0) ? (double)w/h : 1;
  // Set the viewport to the entire window
  glViewport(0,0, w,h);
  // Set projection
  Project();
}

void update()
{

  int col;

  if (dpress || apress) {
    timerad += 0.1;
  }
  else {
    timerad -= 0.1;
  }

  if (timerad < 0) {
    timerad = 0;
  }
  if (timerad > 2) {
    timerad = 2;
  }

  col = collision();
  if (dpress && apress) {
    timerad = 0;
  }
  else if (dpress && col != 2) {
    lx += 0.01*timerad;
  }
  else if (apress && col != 1) {
    lx -= 0.01*timerad;
  }

  if (wpress || spress) {
    timerws += 0.1;
  }
  else {
    timerws -= 0.1;
  }

  if (timerws < 0) {
    timerws = 0;
  }
  if (timerws > 2 && !shift) {
    timerws = 2;
  }
  if (timerws > 5) {
    timerws = 5;
  }

  col = collision();
  if (wpress && spress) {
    timerws = 0;
  }
  else if (wpress && col != 3) {
    camx += vx*0.05*timerws;
    camz += vz*0.05*timerws;
  }
  else if (spress && col != 4) {
    camx -= vx*0.05*timerws;
    camz -= vz*0.05*timerws;
  }

  vx = cos(lx);
  vy = ly;
  vz = sin(lx);

  timer += 25;

  if (camx > 58 && camz > -6 && camz < 6) {
    timer -= 25;
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
  mapdiff = loadterrain("data/terrain/ski.tga");
  //I found the rock here: http://robo3d.com/index.php?pr=100903-tip2
  rock = loadCube("data/objects/rock.obj", rock);
  if (rock == 0) {
    exit(0);
  }
  initrandomnumbers();

  // Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
