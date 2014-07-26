#include <iostream>
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
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

<<<<<<< HEAD
    // Calculate the mean and standard deviation 
    cv::meanStdDev(img, mean, stddev);
    cout << "Mean Intensity: " << mean.at<double>(0) << endl;
    cout << "Standard Deviation of Intensity: " << stddev.at<double>(0) << endl;

    // Determine lower threshold limit
    double lowerLimit = mean.at<double>(0) - (1.5 * stddev.at<double>(0));
=======
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

// First resize it; the ratio to resize to depends on the picture's 
// initial aspect ratio
// Result is 64 by 1
arma::umat HOG(cv::Mat img) {
    cout << "HOG" << endl;

    double aspectRatio = (double)img.rows / img.cols;
    double scalingFactor;
    cout << "Aspect Ratio: " << aspectRatio << endl;


    if (aspectRatio < 0.75) {
        cv::resize(img, img, cv::Size(64, 16)); // Note that size is cols, rows
        scalingFactor = 0.5;
    } 
    else if (aspectRatio > 2) {
        cv::resize(img, img, cv::Size(16, 64));
        scalingFactor = 2;
    }
    else {
        cv::resize(img, img, cv::Size(32, 32));
        scalingFactor = 1;
    }

    cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
    cv::imshow("Display Image", img);
    waitKey(0);

    cv::Mat gx, gy, magnitude, angle;
    double SZ = 20, bin_n = 16;

    cv::Sobel(img, gx, CV_32F, 1, 0);
    cv::Sobel(img, gy, CV_32F, 0, 1);
    cv::cartToPolar(gx, gy, magnitude, angle);

    arma::umat bins = umat(img.rows, img.cols, fill::zeros);

    // Go through each element and fill it with the appr. value
    for (int i = 0; i < bins.n_rows; i++) {
        for (int j = 0; j < bins.n_cols; j++) {
            bins(i, j) = (int) ((bin_n * angle.at<float>(i, j)) / 
                (2 * datum::pi));
            // Could be replaced with fancy constructors
        }
    }
    

    arma::umat result = umat(64, 1, fill::zeros);
    arma::fmat magCells = fmat(32 * scalingFactor, 32 / scalingFactor, fill::zeros);
    int rowsIndex = 0, colsIndex = 0;

    // Building the vector
    for (int index = 0; index < 4; index++) { 
        arma::fmat currentHist = fmat(16, 1, fill::zeros);

        rowsIndex = (magCells.n_rows / 2) * (index % 2);
        colsIndex = (magCells.n_cols / 2) * (index / 2);

        for (int i = rowsIndex; i < rowsIndex + (magCells.n_rows / 2); i++) {
            for (int j = colsIndex; j < colsIndex + (magCells.n_cols / 2); j++) {
                currentHist(bins(i, j), 0) +=  magnitude.at<float>(i, j);
            }
        }

        for (int i = 16 * index; i < 16 * (index + 1); i++) {
            result(i, 0) = currentHist(i % 16, 0);
        }
    }

    return result;
}

int main(int argc, char** argv) {
	// Read in image
	cv::Mat image;
    image = cv::imread( argv[1], 0);

    arma::umat hogData = HOG(image);
    float trainingData[2][64] = {};

    for (int i = 0; i < 64; i++) {
      trainingData[0][i] = hogData(i, 0);
      trainingData[1][i] = hogData(i, 0);
    }
    cv::Mat trainingMat(2, 64, CV_32FC1, trainingData);

    float responses[2] = {1, 5};
    cv::Mat responsesMat(2, 1, CV_32FC1, responses);

    CvSVMParams params;
    params.svm_type = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

    CvSVM SVM;
    SVM.load("svm_data.dat");

    cout << "Got done" << endl;
>>>>>>> 8c22c88acb8bae8437047b7d8bfbbafe611f7e93

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
