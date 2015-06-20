#ifndef MNIST_IO_H
#define MNIST_IO_H

#include <string>
#include <vector>

void loadMNISTLabels(const std::string filename, std::vector<unsigned char> & labels);

float * loadMNISTImages(const std::string filename, int & nImages, int & imgWidth, int & imgHeight);

#endif // MNIST_IO_H
