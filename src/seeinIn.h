#ifndef SEEININ_H
#define SEEININ_H

#include <string>
#include <vector_types.h>
#include "mnist_io.h"

int run(int argc, char * * argv,
        const std::string networkFilename, const std::string weightFilename,
        float * testImages, const int imageWidth, const int imageHeight,
        const int nTestImages, unsigned char * testLabels,
        const int nClasses, const uchar3 * classColors, const std::string * classNames);

#endif // SEEININ_H
