#include "seeinIn.h"

#include <vector>
#include <vector_functions.h>
#include <assert.h>
#include "svhn_io.h"

static const std::string svhnTestFile = "/home/tws10/Data/svhn/test_32x32.mat";

static const std::string weightFile = "/home/tws10/Development/caffe/examples/svhn/svhn_lenet_iter_10000.caffemodel";
static const std::string netFile =    "/home/tws10/Development/caffe/examples/svhn/svhn_lenet.prototxt";

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

    const int imageWidth = 32;
    const int imageHeight = 32;

    std::vector<unsigned char> testLabels;
    float * testImages = loadSVHNLabelsAndImages(svhnTestFile,testLabels);

    const int nTestImages = testLabels.size();

    return run(argc,argv,
               netFile,weightFile,
               testImages,imageWidth,imageHeight,
               nTestImages, testLabels.data(),
               nClasses,digitColors,digitNames);

}
