#include <iostream>
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <armadillo>

#include "contours.h"

using namespace cv;
using namespace arma;
using namespace std;
// Applies a threshold that accounts for various intensities
cv::Mat preProcessing(cv::Mat img) {
    // Initialization
    cout << "Applying preprocessing" << endl;
    cv::Mat mean;
    cv::Mat stddev;

    // Calculate the mean and standard deviation 
    cv::meanStdDev(img, mean, stddev);
    cout << "Mean Intensity: " << mean.at<double>(0) << endl;
    cout << "Standard Deviation of Intensity: " << stddev.at<double>(0) << endl;

    // Determine lower threshold limit
    double lowerLimit = mean.at<double>(0) - (1.5 * stddev.at<double>(0));

    // Apply that threshold
    cv::threshold(img, img, lowerLimit, 255, cv::THRESH_BINARY);
    return img;
}

int main(int argc, char** argv) {
  cout << "Started" << endl;
  // Read in image
  cv::Mat image;
  image = preProcessing(cv::imread( argv[1], 0));
  Contours contours (image);
  cout << "Got done" << endl;
  return 0;
}
