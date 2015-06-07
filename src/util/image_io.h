#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <vector_types.h>

unsigned char * readPNG(const char * filename, int & width, int & height, int & channels);

void writePNG(const char * filename, const uchar3 * imgData, const int width, const int height);

#endif // IMAGE_IO_H
