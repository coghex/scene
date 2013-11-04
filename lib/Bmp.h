#ifndef BMP_H_INCLUDED
#define BMP_H_INCLUDED

unsigned char map[128][128];
struct Texture {
    unsigned long ulDimensionX;
    unsigned long ulDimensionY;
    char *pcData;
};
typedef struct Texture Texture;
unsigned int texture[8];
int loop;

void loadterrain(void);
int LoadBMP(char *szFilename, Texture *pTexture);
int LoadGLTextures(void);


#endif
