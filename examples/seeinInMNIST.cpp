#include "seeinIn.h"

#include <vector_functions.h>
#include <assert.h>

static const std::string mnistTrainImageFile = "../data/mnist/train-images-idx3-ubyte";
static const std::string mnistTrainLabelFile = "../data/mnist/train-labels-idx1-ubyte";
static const std::string mnistTestImageFile  = "../data/mnist/t10k-images-idx3-ubyte";
static const std::string mnistTestLabelFile  = "../data/mnist/t10k-labels-idx1-ubyte";

//static const std::string weightFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese_recomb_iter_50000.caffemodel";
//static const std::string netFile = "/home/tws10/Development/caffe/examples/siamese/mnist_siamese_recomb.prototxt";

static const std::string weightFile = "/home/tws10/Development/caffe/examples/mnist/lenet_iter_10000.caffemodel";
static const std::string netFile =    "/home/tws10/Development/caffe/examples/mnist/lenet.prototxt";

static const int nClasses = 10;
static const uchar3 digitColors[nClasses] = {
    make_uchar3(166,206,227 ),
    make_uchar3(31,120,180  ),
    make_uchar3(178,223,138 ),
    make_uchar3(51,160,44   ),
    make_uchar3(251,154,153 ),
    make_uchar3(227,26,28   ),
    make_uchar3(253,191,111 ),
    make_uchar3(255,127,0   ),
    make_uchar3(202,178,214 ),
    make_uchar3(106,61,154  )
};

static const std::string digitNames[nClasses] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
};

int main(int argc, char * * argv) {

    int nTestImages, imageWidth, imageHeight;
    float * testImages = loadMNISTImages(mnistTestImageFile,nTestImages, imageWidth, imageHeight);
    const int imageChannels = 1;

    std::vector<unsigned char> testLabels;
    loadMNISTLabels(mnistTestLabelFile,testLabels);
    assert(nTestImages == testLabels.size());

    return run(argc,argv,
               netFile,weightFile,
               testImages,imageChannels,imageWidth,imageHeight,
               nTestImages, testLabels.data(),
               nClasses,digitColors,digitNames);

}
