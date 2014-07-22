// Standard Imports
#include <iostream>
#include <stdio.h>
#include <vector>

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

// Computes a length-64 feature vector representing the gradients of the image
// Built in HOG not used because of size restrictions in place
arma::umat HOG(cv::Mat img) {
    cout << "HOG" << endl;

    // Used to determine how to resize
    double aspectRatio = (double)img.rows / img.cols; 
    double scalingFactor, SZ = 20, bin_n = 16;
    cv::Mat gx, gy, magnitude, angle;
    
    cout << "Aspect Ratio: " << aspectRatio << endl;

    // If we stretch to certain sizes, the images become greatly deformed
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

    // Display the resized image
    cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
    cv::imshow("Display Image", img);
    waitKey(0);

    // Compute Sobel derivatives in the x and y directions
    cv::Sobel(img, gx, CV_32F, 1, 0);
    cv::Sobel(img, gy, CV_32F, 0, 1);

    // Convert the derivatives to polar coordinates 
    cv::cartToPolar(gx, gy, magnitude, angle);

    // Quantized values generated by magnitude, angle, and the number of bins
    arma::umat bins = umat(img.rows, img.cols, fill::zeros);

    for (int i = 0; i < bins.n_rows; i++) {
        for (int j = 0; j < bins.n_cols; j++) {
            // Quantize, but first convert to Cartesian
            bins(i, j) = (int) ((bin_n * angle.at<float>(i, j)) / 
                (2 * datum::pi)); 
            // Could be replaced with fancy constructors
        }
    }   

    // Result array of length 64
    arma::umat result = umat(64, 1, fill::zeros);

    // Array that holds magnitudes; Note that dimensions depend on scaling 
    // scaling factor defined above
    arma::fmat magCells = fmat(32 * scalingFactor, 32 / scalingFactor, fill::zeros);

    // Used to indicate starting points for iterating over the magCells
    int rowsIndex = 0, colsIndex = 0;

    for (int index = 0; index < 4; index++) { 
        // Current histogram will be a length-16 vector
        arma::fmat currentHist = fmat(16, 1, fill::zeros);

        // Taking the integers 0, 1, 2, and 3 (mod 2) and their values upon
        // integer divison by 2 gives an easy way to reduce loop usage and 
        // code length
        rowsIndex = (magCells.n_rows / 2) * (index % 2);
        colsIndex = (magCells.n_cols / 2) * (index / 2);

        for (int i = rowsIndex; i < rowsIndex + (magCells.n_rows / 2); i++) {
            for (int j = colsIndex; j < colsIndex + (magCells.n_cols / 2); j++) {
                // Mimics numpy's bincount function with bins as the array,
                // magnitudes as the weights, and at least 16 bins
                currentHist(bins(i, j), 0) +=  magnitude.at<float>(i, j);
            }
        }

        // Push the current histogram into the result one
        for (int i = 16 * index; i < 16 * (index + 1); i++) {
            result(i, 0) = currentHist(i % 16, 0);
        }
    }

    return result;
}

// Finds the contours in the thresholded image
vector< arma::umat > analyzeContours(cv::Mat img, cv::Mat original) {

    // Initialization

    // To be returned and passed to SVM
    vector< arma::umat > validContours;

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
            float zScore = (contourMean[0] - imgMean.at<double>(0)) / 
                imgStdDev.at<double>(0);
            float ratio = contourMean[0] / imgMean.at<double>(0);
            //cout << "Z-Score: " << zScore << endl;
            //cout << "Ratio: " << ratio << endl;


            // Check with standard deviations instead
            if (ratio < 0.85 || zScore < -0.55) {
                //cout << "x, y: " << temp.x << ", " << temp.y << endl;
                //cout << "width, height: " << temp.width << ", " << temp.height << endl;
                /**cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
                cv::imshow("Display Image", mask);
                waitKey(0);  */
                validContours.push_back(HOG(mask));
                counter++;
            }
            // Don't draw rectangles
            // Crop image to x,y,w,h gotten from the bounding rect
            // And do operations on that
        }
    }

    cout << counter << endl;
    return validContours;
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

    // Apply preprocessing 
    Scalar ex = cv::mean(image);
    cout << ex << endl;

    cv::Mat threshImage = preProcessing(image);
    vector< arma::umat > contours;
    contours = analyzeContours(threshImage, image);


    // Display the image in a normal, resizable window
    /**namedWindow("Display Image", cv::WINDOW_NORMAL );
    imshow("Display Image", image);

    // On keydown, exit program
    waitKey(0);*/
    cout << endl;
    
    return 0;
}