#include "mnist_io.h"

#include <iostream>
#include <fstream>

void loadMNISTLabels(const std::string filename, std::vector<unsigned char> & labels) {

    std::ifstream stream;
    stream.open(filename,std::ios_base::in | std::ios_base::binary);

    if (!stream.good()) {
        std::cerr << "couldn't find " << filename << std::endl; return;
    }

    int32_t magicNumber, numLabels;
    stream.read((char *)&magicNumber,sizeof(int32_t));
    magicNumber = be32toh(magicNumber);

    if (magicNumber != mnistMagicLabelNumber) {
        std::cerr << "magic number did not match" << std::endl; return;
    }

    stream.read((char *)&numLabels,sizeof(int32_t));

    numLabels = be32toh(numLabels);

    labels.resize(numLabels);
    stream.read((char *)labels.data(),numLabels*sizeof(unsigned char));
    stream.close();
}

float * loadMNISTImages(const std::string filename, int & nImages, int & imgWidth, int & imgHeight) {

    std::ifstream stream;
    stream.open(filename,std::ios_base::in | std::ios_base::binary);

    if (!stream.good()) {
        std::cerr << "couldn't find " << filename << std::endl; return 0;
    }

    int32_t magicNumber;
    stream.read((char *)&magicNumber,sizeof(int32_t));
    magicNumber = be32toh(magicNumber);

    if (magicNumber != mnistMagicImageNumber) {
        std::cerr << "magic number did not match" << std::endl; return 0;
    }

    stream.read((char *)&nImages,sizeof(int32_t));
    nImages = be32toh(nImages);

    stream.read((char *)&imgHeight,sizeof(int32_t));
    imgHeight = be32toh(imgHeight);

    stream.read((char *)&imgWidth,sizeof(int32_t));
    imgWidth = be32toh(imgWidth);

    float * floatData = new float[nImages*imgHeight*imgWidth];
    unsigned char * ucharData = new unsigned char[nImages*imgHeight*imgWidth];
    stream.read((char *)ucharData,nImages*imgHeight*imgWidth*sizeof(unsigned char));
    stream.close();

    for (int i=0; i<nImages*imgHeight*imgWidth; ++i) {
        floatData[i] = ucharData[i]/255.f;
    }

    delete [] ucharData;
    return floatData;
}
