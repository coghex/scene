#ifndef LOAD_H_INCLUDED
#define LOAD_H_INCLUDED
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


unsigned char map[128][128];
struct Texture {
    unsigned long ulDimensionX;
    unsigned long ulDimensionY;
    char *pcData;
};
typedef struct Texture Texture;
unsigned int texture[8];
int loop;

double cubemap[10000][3];
double cubemapn[10000][3];
int normals[10000][10000];
double avg[10000][3];

void loadterrain(void);
int LoadBMP(char *szFilename, Texture *pTexture);
int LoadGLTextures(void);
GLuint loadCube(char *fname, GLuint cube);

#endif
