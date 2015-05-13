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

float * loadMNISTImages(const std::string filename) {

    std::ifstream stream;
    stream.open(filename,std::ios_base::in | std::ios_base::binary);

    if (!stream.good()) {
        std::cerr << "couldn't find " << filename << std::endl; return 0;
    }

    int32_t magicNumber, numImages, numRows, numCols;
    stream.read((char *)&magicNumber,sizeof(int32_t));
    magicNumber = be32toh(magicNumber);

    if (magicNumber != mnistMagicImageNumber) {
        std::cerr << "magic number did not match" << std::endl; return 0;
    }

    stream.read((char *)&numImages,sizeof(int32_t));
    numImages = be32toh(numImages);

    stream.read((char *)&numRows,sizeof(int32_t));
    numRows = be32toh(numRows);

    stream.read((char *)&numCols,sizeof(int32_t));
    numCols = be32toh(numCols);

    float * floatData = new float[numImages*numRows*numCols];
    unsigned char * ucharData = new unsigned char[numImages*numRows*numCols];
    stream.read((char *)ucharData,numImages*numRows*numCols*sizeof(unsigned char));
    stream.close();

    for (int i=0; i<numImages*numRows*numCols; ++i) {
        floatData[i] = ucharData[i]/255.f;
    }

    delete [] ucharData;
    return floatData;
}
