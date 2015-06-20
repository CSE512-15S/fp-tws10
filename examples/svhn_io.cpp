#include "svhn_io.h"

#include <iostream>
#include <fstream>
#include <string.h>

#include <mat.h>
#include <matrix.h>

static const int imageWidth = 32;
static const int imageHeight = 32;
static const int imageChannels = 3;

float * loadSVHNLabelsAndImages(const std::string filename, std::vector<unsigned char> & labels) {

    MATFile * file = matOpen(filename.c_str(), "r");

    assert(file != NULL);

    mxArray * arrayPtr;

    // read images
    arrayPtr = matGetVariable(file,"X");

    assert(mxGetNumberOfDimensions(arrayPtr) == 4);
    assert(mxGetDimensions(arrayPtr)[0] == 32);
    assert(mxGetDimensions(arrayPtr)[1] == 32);
    assert(mxGetDimensions(arrayPtr)[2] == 3);

    const int nImages = 10000; //mxGetDimensions(arrayPtr)[3];

    assert(mxIsUint8(arrayPtr));

    const int nElements = nImages*imageWidth*imageHeight*imageChannels;
    float * imageData = new float[nElements];
    unsigned char * mxImageData = (unsigned char *)mxGetData(arrayPtr);
    for (int i=0; i<nImages; ++i) {
        for (int c=0; c<imageChannels; ++c) {
            for (int h=0; h<imageHeight; ++h) {
                for (int w=0; w<imageWidth; ++w) {
                    const int caffeIndex = w + imageWidth*(h + imageHeight*(c + imageChannels*i));
                    const int mxIndex = h + imageHeight*(w + imageWidth*(c + imageChannels*i));
                    if ((mxIndex % 10000) == 0) { std::cout << (int)mxImageData[mxIndex] << std::endl; }
                    imageData[caffeIndex] = mxImageData[mxIndex] * (1.f/255.f);
                }
            }
        }
    }

    // read labels
    arrayPtr = matGetVariable(file,"y");

    assert(mxGetNumberOfDimensions(arrayPtr) == 2);
//    assert(mxGetDimensions(arrayPtr)[0] == nImages);
    assert(mxGetDimensions(arrayPtr)[1] == 1);

    labels.resize(nImages);

    assert(mxIsDouble(arrayPtr));

    double * mxLabelData = (double *)mxGetData(arrayPtr);
    for (int i=0; i<nImages; ++i) {
        labels[i] = mxLabelData[i];
    }

    matClose(file);

    return imageData;
}
