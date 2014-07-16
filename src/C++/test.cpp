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

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}


void HOG(cv::Mat img) {
    cout << "HOG" << endl;

    cv::Mat gx, gy, magnitude, angle;
    double SZ = 20, bin_n = 16;

    cv::Sobel(img, gx, CV_32F, 1, 0);
    cv::Sobel(img, gy, CV_32F, 0, 1);
    cv::cartToPolar(gx, gy, magnitude, angle);

    cout << type2str(angle.type()) << endl;

    arma::umat bins = umat(img.rows, img.cols, fill::zeros);

    // Go through each element and fill it with the appr. value
    for (int i = 0; i < bins.n_rows; i++) {
        for (int j = 0; j < bins.n_cols; j++) {
            bins(i, j) = (int) ((bin_n * angle.at<float>(i, j)) / (2 * datum::pi));
            // Could be replaced with fancy constructors
        }
    }

    //cout << bins << endl;    
    cout << "End HOG" << endl;
}

int main(int argc, char** argv) {
	// Read in image
	cv::Mat image;
    image = cv::imread( argv[1], 0);

    HOG(image);

    cout << "Got done" << endl;

    return 0;
}