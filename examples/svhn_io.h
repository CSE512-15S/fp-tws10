#ifndef SVHN_IO_H
#define SVHN_IO_H

#include <string>
#include <vector>

float * loadSVHNLabelsAndImages(const std::string filename, std::vector<unsigned char> & labels);

#endif // SVHN_IO_H
