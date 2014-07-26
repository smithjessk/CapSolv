#include "contours.h"

#include <armadillo>
#include <opencv2/core/core.hpp>

typedef arma::Mat<unsigned char> cmat;
typedef arma::Mat<unsigned char> cvec;

Contours::Contours(cv::Mat cv) {
  cmat arma(cv.data, cv.cols, cv.rows, false, false);
  arma = arma.t();
  uvec rows = find(sum(255 - arma, 1) == 0);
  uvec cols = find(sum(255 - arma, 0) == 0);
  if (rows.n_elem == 0)
    throw 1;
  image_ = arma.submat(rows(0), rows(row.n_elem - 1), cols(0), cols(cols.n_elem - 1));
  cout << image_ << endl;

  // The current number of contours discovered.
  unsigned char contour_counter = (image_(r, 0) == 0) ? 1 : 0;

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
  for (int c = 1, c < image_.n_cols; c++)
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
            image_(span(0, r - 1), c).transform([](unsigned char val) {
              return (val == image_(r, c - 1)) ? image_(r - 1, c) : val;
            });
            image_.cols(0, c - 1).transform([](unsigned char val) {
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
  map_(num_contours_);

  // Index of missing currently being considered.
  int map_index = missing_index = 0;
  for (int counter = 0; counter < contour_counter; counter++) {
    if (missing(missing_index) == counter)
      missing_index++;
    else
      map_(map_index++) = counter;
  }

  // Finding the corners of the bounding box for each contour.
  corners_(4, num_counters_);

  for (int counter = 0; counter < num_contours; counter++) {
    int index = map_(counter);
    rows = find(sum(abs(image_ - contour), 0) == 0);
    cols = find(sum(abs(image_ - contour), 1) == 0);
    corners_(0, counter) = rows(0);
    corners_(1, counter) = rows(rows.n_elem);
    corners_(2, counter) = cols(0);
    corners_(3, counter) = cols(cols.n_elem);
  }
}

Contours::GetContour(int counter) {
  int index = map_(counter);
  cmat contour =  image_.submat(corners_(0, index), corners_(1, index),
                                corners_(2, index), corners_(3, index));
  return 255 * (1 - (cmat) (contour == index));
}
