#include <armadillo>
#include <opencv2/core/core.hpp>

typedef arma::Mat<unsigned char> cmat;
typedef arma::Col<unsigned char> cvec;

using namespace arma;

//class Contours;

class Contours {
 private:
  // Copy of the image passed to the constructor after converting it from opencv
  // and removing excess whitespace bordering the expression.
  cmat image_;

  // The co-ordinates of the corners of the bounding box for each contour.
  arma::umat corners_;

  // Dimensions of the image upon removal of whitespace.
  unsigned int height_, width_;

  // Co-ordinates of the original of the top left corner of the new image.
  unsigned int x_, y_;

  // Number of contours found.
  unsigned int num_contours_;

  // Maps values in image_ to the index of the contour they represent.
  arma::uvec map_;

 public:
  Contours(cv::Mat image) {
    cout << "Constructing Contours" << endl;
    cout << "Cols: " << image.cols << " Rows: " << image.rows << endl;
    cmat arma(image.data, image.cols, image.rows, false, false);
    cout << "Converted Mat to cmat" << endl;
    arma = arma.t();
    cout << "Cols: " << arma.n_cols << " Rows: " << arma.n_rows << endl;
    uvec rows = find(sum(255 - arma, 1) != 0);
    uvec cols = find(sum(255 - arma, 0) != 0);
    // cout << rows << endl;
    // cout << cols << endl;
    if (rows.n_elem == 0)
      throw 1;
    cout << rows.n_elem << " " << cols.n_elem << endl;
    cout << rows(0) << endl;
    cout << rows(rows.n_elem - 1) << endl;
    cout << cols(0) << endl;
    cout << cols(cols.n_elem - 1) << endl;
    image_ = arma.submat(rows(0), rows(rows.n_elem - 1),
                         cols(0), cols(cols.n_elem - 1));
    cout << "Image cropped" << endl;
    cout << "Cols: " << arma.n_cols << " Rows: " << arma.n_rows << endl;


    // The current number of contours discovered.
    unsigned char contour_counter = (image_(0, 0) == 0) ? 1 : 0;

    // List contours that were merged into another.
    cvec missing(256);

    // Finding contours in the first column.
    for (int r = 1; r < image_.n_rows; r++)
      if (image_(r, 0) != 255)
  if (image_(r - 1, 0) != 255)
    image_(r, 0) = image_(r - 1, 0);
  else
    image_(r, 0) = contour_counter++;

    // Finding contours in the first row.
    for (int c = 1; c < image_.n_cols; c++)
      if (image_(0, c) != 255)
  if (image_(0, c - 1) != 255)
    image_(0, c) = image_(0, c - 1);
  else
    image_(0, c) = contour_counter++;

    // Finding contours everywhere else
    for (int c = 1; c < image_.n_cols; c++) {
      for (int r = 1; r < image_.n_rows; r++) {
  if (image_(r, c) != 255) {
    if (image_(r - 1, c) != 255 && image_(r, c - 1) != 255) {
      // Meeting of contours.
      if (image_(r - 1, c) == image_(r, c - 1)) {
        image_(r, c) == image_(r - 1, c);
      } else {
        // Merging of contours.
        image_(r, c) = image_(r - 1, c);
        image_(span(0, r - 1), c).transform([&](unsigned char val) {
    return (val == image_(r, c - 1)) ? image_(r - 1, c) : val;
        });
        image_.cols(0, c - 1).transform([&](unsigned char val) {
    return (val == image_(r, c - 1)) ? image_(r - 1, c) : val;
        });
        missing.insert_rows(missing.n_elem, image_(r, c-1));
      }
    } else if (image_(r - 1, c) != 255) {
      image_(r, c) = image_(r - 1, c);
    } else if (image_(r, c - 1) != 255) {
      image_(r, c) = image_(r, c - 1);
    } else {
      image_(r, c) = contour_counter++;
    }
  }
      }
    }

    num_contours_ = contour_counter - missing.n_elem;

    // Creating the map.
    map_ = uvec(num_contours_);

    // Index of missing currently being considered.
    int map_index, missing_index;
    map_index = missing_index = 0;
    for (int counter = 0; counter < contour_counter; counter++) {
      if (missing(missing_index) == counter)
  missing_index++;
      else
  map_(map_index++) = counter;
    }

    // Finding the corners of the bounding box for each contour.
    corners_ = umat(4, num_contours_);

    for (int counter = 0; counter < num_contours_; counter++) {
      int index = map_(counter);
      rows = find(sum(abs(image_ - index), 0) == 0);
      cols = find(sum(abs(image_ - index), 1) == 0);
      corners_(0, counter) = rows(0);
      corners_(1, counter) = rows(rows.n_elem);
      corners_(2, counter) = cols(0);
      corners_(3, counter) = cols(cols.n_elem);
    }
  }


  cmat GetContour(int counter) const {
    int index = map_(counter);
    cmat contour =  image_.submat(corners_(0, index), corners_(1, index),
          corners_(2, index), corners_(3, index));
    return contour.transform([&](unsigned char val) {
      return (val == index) ? 0 : 255;
    });
  }
 };