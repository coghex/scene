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

GLuint cube;
double cubemap[50000][3];
double cubemapn[50000][3];
int normals[50000][50000];
double avg[50000][3];

void loadterrain(void);
int LoadBMP(char *szFilename, Texture *pTexture);
int LoadGLTextures(void);
void loadCube(char *fname);

#endif
