#include "caffe_helpers.h"

#include <iostream>
#include <set>
#include <vector_functions.h>
#include <helper_math.h>

int getBlobNumber(caffe::Net<float> & net, caffe::Blob<float> * blob) {

    for (int i=0; i<net.blobs().size(); ++i) {
        if (net.blobs()[i].get() == blob) {
            return i;
        }
    }
    return -1;

}

void getBlobsToVisualize(caffe::Net<float> & net, std::vector<std::string> & blobsToVisualize) {

    // find input blob
    const int nInputBlobs = net.input_blob_indices().size();
    if (nInputBlobs == 0) { std::cerr << "there are no input blobs - where to start?" << std::endl; return; }
    blobsToVisualize.push_back(net.blob_names()[net.input_blob_indices()[0]]);

    std::set<std::string> layersWhoOutputVisualizableBlobs;
    layersWhoOutputVisualizableBlobs.insert("Convolution");

    std::map<std::string,std::vector<int> > outputBlobsToVisualizeByLayerType;
    outputBlobsToVisualizeByLayerType["Convolution"] = std::vector<int>(1,0);
    outputBlobsToVisualizeByLayerType["InnerProduct"] = std::vector<int>(1,0);
    outputBlobsToVisualizeByLayerType["Pooling"] = std::vector<int>(1,0);

    const int nLayers = net.layers().size();
    for (int i=0; i<nLayers; ++i) {
        boost::shared_ptr<caffe::Layer<float> > layer = net.layers()[i];
        std::string layerType(layer->type());
        if (outputBlobsToVisualizeByLayerType.find(layerType) != outputBlobsToVisualizeByLayerType.end()) {
            caffe::Blob<float> * outputBlob = net.top_vecs()[i][0];
            int blobNum = getBlobNumber(net,outputBlob);
            assert(blobNum >= 0);
            blobsToVisualize.push_back(net.blob_names()[blobNum]);
        }
    }

}

void getBlobStridesAndReceptiveFields(caffe::Net<float> & net, const std::vector<std::string> & blobsToVisualize,
                                      std::map<std::string,int> & strides, std::map<std::string,int2> & receptiveFields) {

    const int nInputBlobs = net.input_blob_indices().size();
    if (nInputBlobs == 0) { std::cerr << "there are no input blobs - where to start?" << std::endl; return; }
    std::string inputBlobName(net.blob_names()[net.input_blob_indices()[0]]);

    strides[inputBlobName] = 1;
    receptiveFields[inputBlobName] = make_int2(1,1);

    boost::shared_ptr<caffe::Blob<float> > inputBlob = net.blob_by_name(inputBlobName);
    int2 inputSize = make_int2(inputBlob->width(),inputBlob->height());

    const int nLayers = net.layers().size();
    for (int i=0; i<nLayers; ++i) {
        boost::shared_ptr<caffe::Layer<float> > layer = net.layers()[i];
        std::string layerType(layer->type());
        bool isConv = layerType == std::string("Convolution");
        bool isPool = layerType == std::string("Pooling");
        if (isConv || isPool) {
            caffe::Blob<float> * inputBlob = net.bottom_vecs()[i][0];
            int inputBlobNum = getBlobNumber(net,inputBlob);
            assert(inputBlobNum >= 0);
            std::string inputBlobName = net.blob_names()[inputBlobNum];

            caffe::Blob<float> * outputBlob = net.top_vecs()[i][0];
            int outputBlobNum = getBlobNumber(net,outputBlob);
            assert(outputBlobNum >= 0);
            std::string outputBlobName = net.blob_names()[outputBlobNum];

            int2 kernelSize, stride;
            if (isConv) {
                caffe::ConvolutionParameter convParam = layer->layer_param().convolution_param();
                kernelSize = convParam.has_kernel_size() ? make_int2(convParam.kernel_size()) : make_int2(convParam.kernel_w(),convParam.kernel_h());
                stride     = convParam.has_stride() ? make_int2(convParam.stride()) : make_int2(convParam.stride_w(),convParam.stride_h());
            } else if (isPool) {
                caffe::PoolingParameter poolParam = layer->layer_param().pooling_param();
                kernelSize = poolParam.has_kernel_size() ? make_int2(poolParam.kernel_size()) : make_int2(poolParam.kernel_w(),poolParam.kernel_h());
                stride = poolParam.has_stride() ? make_int2(poolParam.stride()) : make_int2(poolParam.stride_w(),poolParam.stride_h());
            }
            if (strides.find(inputBlobName) != strides.end()) {
                const int strideIn = strides[inputBlobName];
                const int2 fieldIn = receptiveFields[inputBlobName];
                strides[outputBlobName] = strideIn*stride.x;
                receptiveFields[outputBlobName] = strideIn*(kernelSize - make_int2(1)) + fieldIn;
            }
        } else if (layerType == std::string("InnerProduct")) {
            caffe::Blob<float> * inputBlob = net.bottom_vecs()[i][0];
            int inputBlobNum = getBlobNumber(net,inputBlob);
            assert(inputBlobNum >= 0);
            std::string inputBlobName = net.blob_names()[inputBlobNum];

            caffe::Blob<float> * outputBlob = net.top_vecs()[i][0];
            int outputBlobNum = getBlobNumber(net,outputBlob);
            assert(outputBlobNum >= 0);
            std::string outputBlobName = net.blob_names()[outputBlobNum];

            if (strides.find(inputBlobName) != strides.end()) {
                strides[outputBlobName] = strides[inputBlobName];
                receptiveFields[outputBlobName] = inputSize;
            }
        }
    }

}
