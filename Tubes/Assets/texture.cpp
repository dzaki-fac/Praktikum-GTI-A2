
#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "texture.h"
/* ================================================================
   BMP Loader sederhana (24-bit uncompressed BMP)
   ================================================================ */
GLuint loadBMP(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "[WARN] Tidak bisa buka %s, pohon tanpa tekstur.\n", filename);
        return 0;
    }

    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54 || header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "[WARN] %s bukan BMP valid.\n", filename);
        fclose(f);
        return 0;
    }

    /* Baca metadata dari header BMP */
    int dataOffset  = *(int*)&header[10];
    int width       = *(int*)&header[18];
    int height      = *(int*)&header[22];
    short bpp       = *(short*)&header[28];

    if (bpp != 24) {
        fprintf(stderr, "[WARN] %s harus 24-bit BMP (sekarang %d-bit).\n", filename, bpp);
        fclose(f);
        return 0;
    }

    /* Row BMP selalu di-pad ke kelipatan 4 byte */
    int rowSize   = ((width * 3 + 3) / 4) * 4;
    int imageSize = rowSize * height;

    unsigned char *rawData = (unsigned char*)malloc(imageSize);
    if (!rawData) { fclose(f); return 0; }

    fseek(f, dataOffset, SEEK_SET);
    fread(rawData, 1, imageSize, f);
    fclose(f);

    /* BMP disimpan BGR, kita perlu RGB untuk OpenGL */
    /* Sekaligus flip vertikal (BMP bottom-up) */
    unsigned char *rgbData = (unsigned char*)malloc(width * height * 3);
    if (!rgbData) { free(rawData); return 0; }

    int row, col;
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            int srcIdx = (height - 1 - row) * rowSize + col * 3; /* flip vertikal */
            int dstIdx = row * width * 3 + col * 3;
            rgbData[dstIdx + 0] = rawData[srcIdx + 2]; /* R */
            rgbData[dstIdx + 1] = rawData[srcIdx + 1]; /* G */
            rgbData[dstIdx + 2] = rawData[srcIdx + 0]; /* B */
        }
    }
    free(rawData);

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                      GL_UNSIGNED_BYTE, rgbData);
    free(rgbData);

    printf("[INFO] Loaded %s (%dx%d) -> texID=%u\n", filename, width, height, texID);
    return texID;
}