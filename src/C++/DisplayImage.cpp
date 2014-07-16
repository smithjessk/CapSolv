// Standard Imports
#include <iostream>
#include <stdio.h>

// Armadillo
#include <armadillo>

// OpenCV Dependencies
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/cuda.hpp>

//#include "contours.h"

using namespace cv;
using namespace arma;
using namespace std;

// Terrible printing mechanism, so this is not used for now
typedef arma::Mat<unsigned char> cmat;

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

    // Show the thresholded image
    /**
    cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
    cv::imshow("Display Image", img);
    waitKey(0);*/

    cout << endl;
    return img;
}

// Finds the contours in the thresholded image
vector< vector<Point> > analyzeContours(cv::Mat img, cv::Mat original) {

    // Initialization
    cout << "Analyzing contours" << endl;
    float imgArea = img.rows * img.cols;
    vector< vector<Point> > contours;

    cv::Mat img1 = img.clone();

    cv::Mat imgMean;
    cv::Mat imgStdDev;
    cv::meanStdDev(img1, imgMean, imgStdDev);
    cout << "Image Mean " << imgMean << endl;

    // Find the contours
    cv::findContours(img, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Vector to hold the bounding rectangles
    vector<Rect> validRectangles(contours.size());

    // Initialize variables to be used later
    cv::Rect temp;
    float currArea;
    float ratio;
    int counter = 0;
    cv::Mat mask;
    Scalar contourMean;

    // Iterate over contours
    for (int i = 0; i < contours.size(); i++) {
        temp = boundingRect(contours[i]);
        // Caluclate each contour's area
        
        currArea = temp.width * temp.height;
        ratio = currArea / imgArea;

        // If it's big enough
        if (ratio < 0.95 && ratio > 0.0003 && currArea >= 81) {
            mask = cv::Mat(img1, temp);
            cv::Mat mask2 = cv::Mat(original, temp);
            //mask = Mat::zeros(temp.width, temp.height, CV_8U);
            //cout << "New try: " << (mask.rows * mask.cols) << endl;
            //cv::drawContours(mask, contours, i, 255, -1);
            contourMean = cv::mean(mask);

            //cout << contourMean << endl;
            float zScore = (contourMean[0] - imgMean.at<double>(0)) / imgStdDev.at<double>(0);
            float ratio = contourMean[0] / imgMean.at<double>(0);
            //cout << "Z-Score: " << zScore << endl;
            //cout << "Ratio: " << ratio << endl;


            // Check with standard deviations instead
            if (ratio < 0.85 || zScore < -0.55) {
                cout << "x, y: " << temp.x << ", " << temp.y << endl;
                cout << "width, height: " << temp.width << ", " << temp.height << endl;
                /**cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
                cv::imshow("Display Image", mask);
                waitKey(0);  */
                counter++;
            }
            // Don't draw rectangles
            // Crop image to x,y,w,h gotten from the bounding rect
            // And do operations on that
        }
    }

    cout << counter << endl;
    return contours;
}

// Computes Histogram of Ordered Gradients for an image of a single contour
// Python code was based on OpenCV Docs
// For another sample, look in samples/gpu/hog.cpp
void HOG(cv::Mat img) {
    cout << "HOG" << endl;

    cv::Mat gx, gy, magnitude, angle;
    double SZ = 20, bin_n = 16;

    cv::Sobel(img, gx, CV_32F, 1, 0);
    cv::Sobel(img, gy, CV_32F, 0, 1);
    cv::cartToPolar(gx, gy, magnitude, angle);

    //charmat image_arma = charmat(img.data, img.rows, img.cols, false, false);

    arma::umat bins = umat(img.rows, img.cols, fill::zeros);

    // Go through each element and fill it with the appr. value
    for (int i = 0; i < bins.n_rows; i++) {
        for (int j = 0; j < bins.n_cols; j++) {
            bins(i, j) = (int) ((bin_n * angle.at<float>(i, j)) / (2 * datum::pi));
            // Could be replaced with fancy constructors
            cout << bins(i, j) << " ";
        }
        cout << endl;
    }


    //cout << bins << endl;    
    cout << "End HOG" << endl;
}

int main(int argc, char** argv ) {
    // No image path
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    // Read in the image as grayscale
    cv::Mat image;
    image = cv::imread(argv[1], 0);

    cout << "Image Dimensions: " << image.rows << ", " << image.cols << endl;

    // If it is an empty image, return
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    HOG(image);

    // Apply preprocessing 
    Scalar ex = cv::mean(image);
    cout << ex << endl;

    cv::Mat threshImage = preProcessing(image);
    vector< vector<Point> > test;
    test = analyzeContours(threshImage, image);


    // Display the image in a normal, resizable window
    /**namedWindow("Display Image", cv::WINDOW_NORMAL );
    imshow("Display Image", image);

    // On keydown, exit program
    waitKey(0);*/
    cout << endl;
    
    return 0;
}