#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <armadillo>
//#include <contours.h>
#include "contours.h"

using namespace cv;
using namespace arma;

// Applies a threshold that accounts for various intensities
cv::Mat preProcess(cv::Mat img, bool displayImgs = false) {
    // Initialization
    cout << "Applying preprocessing" << endl;
    cv::Mat mean;
    cv::Mat stddev;

    // Calculate the mean and standard deviation
    cv::meanStdDev(img, mean, stddev);

    // Determine lower threshold limit
    double lowerLimit = mean.at<double>(0) - (1.25 * stddev.at<double>(0));
    

    // Apply that threshold
    //cv::threshold(img, img, lowerLimit, 255, cv::THRESH_BINARY);

    // Show the thresholded image
    if (displayImgs) {
        cv::namedWindow("Display Image", cv::WINDOW_NORMAL);
        cv::imshow("Display Image", img);
        waitKey(0);
    }

    cout << endl;
    return img;
}

// First resize it; the ratio to resize to depends on the picture's 
// initial aspect ratio
// Result is 64 by 1
int main(int argc, char** argv) {
    // Read in image
    cv::Mat image;
    image = cv::imread( argv[1], 0);
    image = preProcess(image);

    Contours contours (image);

    /**
    arma::umat hogData = HOG(image);
    float trainingData[2][64] = {};

int main(int argc, char** argv) {

    cout << "Got done" << endl;

    return 0;
    return 0;*/
 }
