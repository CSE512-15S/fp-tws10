#ifndef CAFFE_HELPERS_H
#define CAFFE_HELPERS_H

#include <caffe/caffe.hpp>
#include <map>
#include <string>
#include <vector>
#include <vector_types.h>

int getBlobNumber(caffe::Net<float> & net, caffe::Blob<float> * blob);

void getBlobsToVisualize(caffe::Net<float> & net, std::vector<std::string> & blobsToVisualize);

void getBlobStridesAndReceptiveFields(caffe::Net<float> & net, const std::vector<std::string> & blobsToVisualize,
                                      std::map<std::string,int> & strides, std::map<std::string,int2> & receptiveFields);

#endif // CAFFE_HELPERS_H
