#include <armadillo>
#include <opencv2/core/core.hpp>

#ifndef Contours
#define Contours

typedef arma::Mat<unsigned char> cmat;

class Contours;

class Contours {
 private:
  // Copy of the image passed to the constructor after converting it from opencv
  // and removing excess whitespace bordering the expression.
  cmat image_;

  // The co-ordinates of the corners of the bounding box for each contour.
  umat corners;

  // Dimensions of the image upon removal of whitespace.
  unsigned int height_, width_;

  // Co-ordinates of the original of the top left corner of the new image.
  unsigned int x_, y_;

  // Number of contours found.
  unsigned int num_contours_;

  // Maps values in image_ to the index of the contour they represent.
  uvec map_;

 public:
  Contours(cv::Mat image);

  cmat GetContour(int counter) const;

 }

#endif
