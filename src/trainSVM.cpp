// Trains the SVM
// Usage: ./trainSVM <<directory of training images>> <<list of enums to train on>>

// Standard Imports
#include <iostream>
#include <stdio.h>
#include <vector>

// Armadillo
#include <armadillo>

// OpenCV Dependencies
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;
using namespace arma;
using namespace std;

// Applies a threshold that accounts for various intensities
cv::Mat PreProcess(cv::Mat img, bool displayImgs = false) {
    // Initialization
    cv::Mat mean;
    cv::Mat stddev;

    // Calculate the mean and standard deviation 
    cv::meanStdDev(img, mean, stddev);
   
    // Determine lower threshold limit
    double lowerLimit = mean.at<double>(0) - (1.25 * stddev.at<double>(0));

    // Apply that threshold
    cv::threshold(img, img, lowerLimit, 255, cv::THRESH_BINARY);

    // Show the thresholded image
    if (displayImgs) {
        cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
        cv::imshow("Display Image", img);
        waitKey(0);
    }

    return img;
}

// Computes a length-64 feature vector representing the gradients of the image
// Built in HOG not used because of size restrictions in place
arma::umat ComputeHOG(cv::Mat img, bool displayImgs = false) {
    //cout << "HOG" << endl;

    // Used to determine how to resize
    double aspectRatio = (double)img.rows / img.cols; 
    double scalingFactor, SZ = 20, bin_n = 16;
    cv::Mat gx, gy, magnitude, angle;
    
    //cout << "Aspect Ratio: " << aspectRatio << endl;

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

    //cv::resize(img, img, cv::Size(32, 32));
    //scalingFactor = 1;

    //cv::medianBlur(img, img, 5);

    // Display the resized image
    if (displayImgs) {
        cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
        cv::imshow("Display Image", img);
        waitKey(0);
    }

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
    arma::fmat magCells = fmat(32 * scalingFactor, 32 / scalingFactor, 
        fill::zeros);

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
vector< arma::umat > AnalyzeContours(cv::Mat img, vector< arma::imat >& parseInfo, 
    bool displayImgs = false) {
    // Initialization

    float imgArea = img.rows * img.cols;
    vector< vector<Point> > contours;

    // To be returned and passed to SVM
    vector< arma::umat > validContours;

    cv::Mat img1 = img.clone();

    cv::Mat imgMean;
    cv::Mat imgStdDev;
    cv::meanStdDev(img1, imgMean, imgStdDev);

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
        if (ratio < 2 && ratio > 0.0003 && currArea >= 400) {
            mask = cv::Mat(img1, temp);
            cv::Mat mask2 = cv::Mat(img, temp);
            contourMean = cv::mean(mask);

            float intensityRatio = contourMean[0] / imgMean.at<double>(0);
            if (intensityRatio < 0.95 && (intensityRatio / currArea < 0.0003) ) {
                validContours.push_back(ComputeHOG(mask, displayImgs));
                arma::imat tempArr = {-1, temp.x, temp.y, temp.width, temp.height};
                parseInfo.push_back(tempArr);
                counter++;
            }
        }
    }

    return validContours;
}

int main(int argc, char** argv ) {
	arma::wall_clock timer;

	timer.tic();

    // Read in directory name
    string directory = argv[1];

    // Create vector of contours and of labels
    vector< vector< arma::umat > > contours;
    vector< float > responses;
    int arraySize = 0;

    for (int i = 2; i < argc; i++) {
        string num = argv[i];
        cv::Mat image = cv::imread(directory + num + ".jpg", 0);
        float number = stof(num);

        vector< arma::imat > parseInfo;

        cv::Mat threshImage = PreProcess(image, false);
        vector< arma::umat > tempContours = AnalyzeContours(threshImage, parseInfo, false);

        contours.push_back(tempContours);
        responses.push_back(number);
        arraySize += tempContours.size();
    }

    /**

    // To deal with alternate versions of the letter x
    for (int i = 0; i < 100; i++) {
        cv::Mat image = cv::imread("../evaluation/digit_training/diff_x.jpg", 0);

        vector< arma::imat > parseInfo;

        cv::Mat threshImage = PreProcess(image, false);
        vector< arma::umat > tempContours = AnalyzeContours(threshImage, parseInfo, false);

        contours.push_back(tempContours);
        responses.push_back(20);
        arraySize += tempContours.size();
    }

    // To deal with alternate minus signs
    for (int i = 0; i < 100; i++) {
        cv::Mat image = cv::imread("../evaluation/digit_training/diff_-.jpg", 0);

        vector< arma::imat > parseInfo;

        cv::Mat threshImage = PreProcess(image, false);
        vector< arma::umat > tempContours = AnalyzeContours(threshImage, parseInfo, false);

        contours.push_back(tempContours);
        responses.push_back(11);
        arraySize += tempContours.size();
    }

    **/

    float trainData[arraySize][64], responseData[arraySize][1];
    int numAdded = 0;

    for (int i = 0; i < contours.size(); i++) {
        for (int j = 0; j < contours[i].size(); j++) {
            for (int k = 0; k < 64; k++) {
                trainData[numAdded][k] = contours[i][j][k];
            }
            responseData[numAdded][0] = responses[i];
            numAdded++; 
        }
    }

    cv::Mat trainMat(arraySize, 64, CV_32FC1, trainData);
    cv::Mat responseMat(arraySize, 1, CV_32FC1, responseData);

    CvSVMParams params;
    params.svm_type = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

    CvSVM SVM;
    SVM.train(trainMat, responseMat, cv::Mat(), cv::Mat(), params);

    /** 
    cout << "Testing SVM" << endl;
    cv::Mat testImage = cv::imread(
        "../../evaluation/examples/appropo.jpg", 0);
    cv::Mat threshTest = PreProcess(testImage, true);

    vector< arma::umat > testContours = AnalyzeContours(threshTest, testImage, true);

    float testArray[testContours.size()][64];
    for (int i = 0; i < testContours.size(); i++) {
        for (int j = 0; j < 64; j++) {
            testArray[i][j] = testContours[i][j];
        }
    }
    cv::Mat testMat(testContours.size(), 64, CV_32FC1, testArray);
    cv::Mat predictionsMat(testContours.size(), 1, CV_32FC1, 0);

    SVM.predict(testMat, predictionsMat);
    cout << predictionsMat;
    */
    
    SVM.save("svm_data.dat");
    cout << "Time: " << timer.toc() << endl;

    return 0;
}
