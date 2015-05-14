#ifndef MNIST_IO_H
#define MNIST_IO_H

#include <string>
#include <vector>

static const std::string mnistTrainImageFile = "../data/mnist/train-images-idx3-ubyte";
static const std::string mnistTrainLabelFile = "../data/mnist/train-labels-idx1-ubyte";
static const std::string mnistTestImageFile  = "../data/mnist/t10k-images-idx3-ubyte";
static const std::string mnistTestLabelFile  = "../data/mnist/t10k-labels-idx1-ubyte";
static const int mnistMagicLabelNumber = 2049;
static const int mnistMagicImageNumber = 2051;

void loadMNISTLabels(const std::string filename, std::vector<unsigned char> & labels);
float * loadMNISTImages(const std::string filename, int & nImages, int & imgWidth, int & imgHeight);

#endif // MNIST_IO_H
