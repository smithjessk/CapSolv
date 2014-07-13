#include <iostream>
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <armadillo>
//#include <contours.h>

using namespace cv;
using namespace arma;
using namespace std;

int main(int argc, char** argv) {
	// Read in image
	cv::Mat image;
    image = cv::imread( argv[1], 0);

    cout << "Got done" << endl;

    return 0;
}