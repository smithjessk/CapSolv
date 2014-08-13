# CapSolv

## About

CapSolv is a computer vision engine that captures, parses, and solves printed mathematics. 

It is distributed under the [MIT License.](https://github.com/smithjessk/CapSolv/blob/master/LICENSE.md)

## Usage

Note: Compiled using GCC on Linux Mint 17. Also, this probably won't work for other systems until we do some Make surgery. 

To run the classification pipeline on an image, navigate to /src/C++ and run:

    $ ./process {{your relative image path}}

## Pipeline

1. Read in the image in grayscale
2. Threshold the image to binary, with the boundary intensity being 1.25 standard deviations below the mean intensity (see [the OpenCV documentation](http://docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html); we use <code>cv::THRESH_BINARY</code>)
3. Find and record the contours in the image
4. For each contour, compute its descriptors (from Histogram of Ordered Gradients)
5. Pass each contour's descriptors to the Support Vector Machine and obtain a character (e.g., 0, a, +, integral sign)
6. Parse the expression
7. Return the resulting string

## Contact

For any questions or comments, feel free to email <smith.jessk@gmail.com> or <jonathanmclaus@gmail.com>.
