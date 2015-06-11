# fp-tws10
Seein' In: visualizing convolutional neural networks (http://cse512-15s.github.io/fp-tws10/)

# Platform

Seein' In has only been tested to run on Ubuntu 14.04.

# Required Dependencies:
Seein' In requires the following software:

Pangolin - specifically from this fork (https://github.com/tschmidt23/Pangolin) to ensure the GUI is layed out correctly.

caffe - https://github.com/BVLC/caffe

freetype - <code>sudo apt-get install libfreetype6-dev</code>

fontconfig - <code>sudo apt-get install libfontconfig1-dev</code>

# Highly Recommended Dependencies:

CUDA - https://developer.nvidia.com/cuda-downloads

# Install directions

```
mkdir build 
cd build
cmake -DCAFFE_ROOT={path_to_caffe}
make
```

# Running directions

To run the MNIST example (from examples/seeinInMNIST), you will need to acquire the data from http://yann.lecun.com/exdb/mnist/. Specifically, the executable will look for the data in the following files:

../data/mnist/train-images-idx3-ubyte
../data/mnist/train-labels-idx1-ubyte

These are both relative to the directory the executable is run from (by default, the build/ directory). These files can be links if they live elsewhere.

# Development Process commentary

A significant portion of the development time was spent simply grappling with the scale of the data, and as an end result there are a number of tricks happening behind the scenes. When the embedding view is focused on a 500-dimensinal layer, for example, there are 250,000 pairs of scatter plots, each with 10,000 images to be plotted. The first trick is to limit the degree to which a user can zoom out. The threshold is set to a reasonably high level beyond which the user would likely not be able to see anything anyway. Then, a quick computation reveals which scatter plots are on screen at any given time and only those are drawn. However, at the highest zoom level, this could still be hundreds of scatter plots. 

The next problem was that when passing vertex data to OpenGL, the vertex coordinate must be contiguous in memory. That is, the array must contain the x coordinate of the first point, followed by the y coordinate of the first point, followed by the x coordiante of the second point and so on. However, storing this would require a prohibitively large amount of memory and recomputing it every frame would be too slow. To get around this, I instead separate out the data into D vectors of length 10,000 (for D-dimensional data), one for each dimension. When drawing a scatter plot, I bind the appropriate vectors to custom attributes defined separately for the x coordinate and the y coordinate. I then render an empty array of the appropriate size, and use a GLSL shader to swap in the attributes for the vertex coordinates. 

Finally, in the end I simply have to limit the amount of points that get rendered to the screen. There is a simple threshold defined, and a simple computation of the number of scatter plots in view determines how many points are allocated to each plot. Because the data is shuffled and there is a lot of overlap between points, the result is visually identical to drawing all points at high zoom levels. As you zoom in, more and more points are added seamlessly (it can't even be detected). With all these tricks I am able maintain interactive framerates even with the massive scale of the data and the display thereof.
