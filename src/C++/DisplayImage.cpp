#include <iostream>
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

// Applies a threshold that accounts for various intensities
Mat preProcessing(Mat img) {
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
    cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
    cv::imshow("Display Image", img);
    waitKey(0);

    cout << endl;
    return img;
}

// Finds the contours in the thresholded image
vector< vector<Point> > analyzeContours(Mat img, Mat original) {


    // Initialization
    cout << "Analyzing contours" << endl;
    float imgArea = img.rows * img.cols;
    vector< vector<Point> > contours;

    Mat img1 = img.clone();

    Mat imgMean;
    Mat imgStdDev;
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
            mask = Mat(img1, temp);
            Mat mask2 = Mat(original, temp);
            //mask = Mat::zeros(temp.width, temp.height, CV_8U);
            //cout << "New try: " << (mask.rows * mask.cols) << endl;
            //cv::drawContours(mask, contours, i, 255, -1);
            contourMean = cv::mean(mask);

            //cout << contourMean << endl;
            float zScore = (contourMean[0] - imgMean.at<double>(0)) / imgStdDev.at<double>(0);
            float ratio = contourMean[0] / imgMean.at<double>(0);
            cout << "Z-Score: " << zScore << endl;
            cout << "Ratio: " << ratio << endl;


            // Check with standard deviations instead
            if (ratio < 0.85 || zScore < -0.55) {
                cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
                cv::imshow("Display Image", mask);
                waitKey(0);  
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

int main(int argc, char** argv ) {
    // No image path
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    // Read in the image as grayscale
    cv::Mat image;
    image = cv::imread( argv[1], 0);

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