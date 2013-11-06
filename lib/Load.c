#include "Load.h"
#include <stdio.h>
#include <stdlib.h>
// OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


// uses the same apporach as NeHe but for an RGB tga file
void loadterrain(void) {
  FILE *filePointer;
  unsigned long iCount;
  unsigned char rgbmap[128][128*3];
  int i, j;
  // make sure the file is there.
  // I found this file at http://www.lighthouse3d.com/opengl/terrain/
  if ((filePointer = fopen("data/terrain/ter.tga", "rb"))==NULL)
  {
    printf("File Not Found : ter.tga");
    exit(0);
  }
  fseek(filePointer, 12, SEEK_CUR);

  char buf[4];
  // read the width and height
  if ((iCount = fread(buf, 4, 1, filePointer)) != 1) {
    printf("Error reading width from terr.tga");
    exit(0);
  }

  fseek(filePointer, 2, SEEK_CUR);

  for(i = 0; i<128; i++) {
    if ((iCount = fread(rgbmap[i], 128*3, 1, filePointer)) != 1) {
      printf("Error reading width from terr.tga");
      exit(0);
    }
  }
  for(i = 0; i<128; i++) {
    for(j = 0; j<128; j++) {
      map[i][j] =  rgbmap[i][3*j];
    }
  }
}

// This function has been taken from http://nehe.gamedev.net/ and
// modified to work on my computer, I hope it works on your's too!
// This is because my method af editing bmp image (GIMP on macbook)
// generates bmp images with unusual headers.  This should work with
// the images that come with the source.

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
  return 1;
}

// Load Bitmaps And Convert To Textures, code also inspired by nehe gamedev
int LoadGLTextures(void)
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
      // image from http://en.pudn.com/downloads72/sourcecode/windows/opengl/detail262399_en.html
      case 0:
        LoadBMP("data/tex/snow.bmp", TextureImage);
        break;

      // image is from http://www.turbosquid.com/FullPreview/Index.cfm/ID/417496
      case 1:
        LoadBMP("data/tex/bark.bmp", TextureImage);
        break;

      // image is from http://www.123rf.com/photo_4835333_vibrant-green-tree-leaves-texture.html
      case 2:
        LoadBMP("data/tex/tree.bmp", TextureImage);
        break;

      case 3:
        LoadBMP("data/sb/bk.bmp", TextureImage);
        break;

      case 4:
        LoadBMP("data/sb/ft.bmp", TextureImage);
        break;

      case 5:
        LoadBMP("data/sb/lf.bmp", TextureImage);
        break;

      case 6:
        LoadBMP("data/sb/rt.bmp", TextureImage);
        break;

      case 7:
        LoadBMP("data/sb/up.bmp", TextureImage);
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

GLuint loadCube(char *fname, GLuint cube)
{
  FILE *fp;
  int read;
  GLfloat x, y, z;
  int a, b, c, d, e, f;
  char ch;
  int i = 0;
  int j, k;
  cube=glGenLists(1);
  fp=fopen(fname,"r");
  if (!fp)
  {
    printf("can't open file %s\n", fname);
    exit(1);
  }
  glPointSize(2.0);
  glNewList(cube, GL_COMPILE);
  {
    glPushMatrix();
    while(!(feof(fp)))
    {
      read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
      if(read==4&&ch=='v')
      {
        cubemap[i][0] = x;
        cubemap[i][1] = y;
        cubemap[i][2] = z;
        i++;
      }
    }
    fclose(fp);

    i=0;
    fp=fopen(fname,"r");
    while(!(feof(fp)))
    {
      read=fscanf(fp,"%cn %f %f %f",&ch,&x,&y,&z);
      if(read==4&&ch=='v')
      {
        cubemapn[i][0] = x;
        cubemapn[i][1] = y;
        cubemapn[i][2] = z;
        i++;
      }
    }
    fclose(fp);

    fp=fopen(fname,"r");
    while(!(feof(fp)))
    {
      read = fscanf(fp, "%c %d//%d %d//%d %d//%d", &ch, &a, &b, &c, &d, &e, &f);
      if(read==7&&ch=='f')
      {
        i=0;
        j=0;
        k=0;
        while (normals[a-1][i]) {
          i++;
        }
        while (normals[c-1][j]) {
          j++;
        }
        while (normals[e-1][k]) {
          k++;
        }

        normals[a-1][i] = b;
        normals[c-1][j] = d;
        normals[e-1][k] = f;
      }
    }
    fclose(fp);

    for (i=0;normals[i][0];i++) {
      for (j=0;normals[i][j];j++) {
        avg[i][0] += cubemapn[normals[i][j]][0];
        avg[i][1] += cubemapn[normals[i][j]][1];
        avg[i][2] += cubemapn[normals[i][j]][2];
      }
      avg[i][0] /= j;
      avg[i][1] /= j;
      avg[i][2] /= j;
    }

    fp=fopen(fname,"r");
    while(!(feof(fp))) {
      read = fscanf(fp, "%c %d//%d %d//%d %d//%d", &ch, &a, &b, &c, &d, &e, &f);
        if (read==7&&ch=='f') {
          glBegin(GL_TRIANGLE_STRIP);
          // I dont know why there are negative
          glNormal3f(-avg[a-1][0], -avg[a-1][1], -avg[a-1][2]);
          glVertex3f(cubemap[a-1][0], cubemap[a-1][1], cubemap[a-1][2]);
          glNormal3f(-avg[c-1][0], -avg[c-1][1], -avg[c-1][2]);
          glVertex3f(cubemap[c-1][0], cubemap[c-1][1], cubemap[c-1][2]);
          glNormal3f(-avg[e-1][0], -avg[e-1][1], -avg[e-1][2]);
          glVertex3f(cubemap[e-1][0], cubemap[e-1][1], cubemap[e-1][2]);
          glEnd();
        }
      }

  }
  glPopMatrix();
  glEndList();
  fclose(fp);
  return cube;
}

