#include <armadillo>
#include <opencv2/core/core.hpp>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <unordered_set>

typedef arma::Mat<unsigned char> cmat;
typedef arma::Col<unsigned char> cvec;
typedef arma::Row<unsigned char> crow;

using namespace arma;
using namespace std;

class Contours;

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
    crow sums = sum(255 - arma, 0);
    // cout << rows.t() << endl;
    // cout << cols.t() << endl;
    // if (rows.n_elem == 0 || cols.n_elem == 0) {
    //   cout << "No black pixels." << endl;
    //   throw 1;
    // }
    cout << "# filled rows: " << rows.n_elem << " first: " << rows(0)
         << " last: " << rows(rows.n_elem - 1) << endl;
    cout << "# filled cols: " << cols.n_elem << " first: " << cols(0)
         << " last: " << cols(cols.n_elem - 1) << endl;
    image_ = arma.submat(rows(0), cols(0),
                         rows(rows.n_elem - 1), cols(cols.n_elem - 1));
    cout << "Image cropped" << endl;
    cout << "Cols: " << image_.n_cols << " Rows: " << image_.n_rows << endl;

    // A map from contours to sets of contours. A contour y is in the set mapped
    // to by contour x if the two contours are adjcacent.
    auto adj = std::unordered_map<int, std::unordered_set<int>>();

    // The current number of contours discovered.
    unsigned char contour_counter = (image_(0, 0) == 0) ? 1 : 0;
    if (contour_counter != 0)
      adj.emplace(0, std::unordered_set<int>());

    // List contours that were merged into another.
    cvec missing((uword) 0);

    // Finding contours in the first column.
    for (int r = 1; r < image_.n_rows; r++) {
      if (image_(r, 0) != 255) {
        if (image_(r - 1, 0) != 255) {
          image_(r, 0) = image_(r - 1, 0);
        } else {
	  adj.emplace(contour_counter, std::unordered_set<int>());
          image_(r, 0) = contour_counter++;
	}
      }
    }

    // Finding contours in the first row.
    for (int c = 1; c < image_.n_cols; c++) {
      if (image_(0, c) != 255) {
        if (image_(0, c - 1) != 255) {
          image_(0, c) = image_(0, c - 1);
        } else {
	  adj.emplace(contour_counter, std::unordered_set<int>());
          image_(0, c) = contour_counter++;
	}
      }
    }

    // Finding contours everywhere else
    for (int c = 1; c < image_.n_cols; c++) {
      for (int r = 1; r < image_.n_rows; r++) {
        if (image_(r, c) != 255) {
          if (image_(r - 1, c) != 255 && image_(r, c - 1) != 255) {
            // Meeting of contours is found.
            if (image_(r - 1, c) == image_(r, c - 1)) {
	      // Contour intersection with itself, a trivial case.
              image_(r, c) = image_(r - 1, c);
            } else {
              // Contour intersection is recorded.
              image_(r, c) = image_(r - 1, c);
	      adj.at(image_(r - 1, c)).emplace(image_(r, c - 1));
	      adj.at(image_(r, c - 1)).emplace(image_(r - 1, c));
            }
          } else if (image_(r - 1, c) != 255) {
            image_(r, c) = image_(r - 1, c);
          } else if (image_(r, c - 1) != 255) {
            image_(r, c) = image_(r, c - 1);
          } else {
	    adj.emplace(contour_counter, std::unordered_set<int>());
            image_(r, c) = contour_counter++;
          }
        }
      }
    }

    cout << "here" << endl;

    PrintMap(adj);

    // Contour merge order is decided, with predecence for lower ordinals.
    Merge(adj);

    // The map from cardinals to ordinals is constructed.
    int map_index = 0;
    map_ = uvec(contour_counter);
    for (int counter = 0; counter < contour_counter; counter++) {
      if (adj.count(counter))
	map_(map_index++) = counter;
    }
    map_.resize(map_index);

    num_contours_ = adj.size();

    cout << "here2" << endl;

    cout << image_ << endl;

    PrintMap(adj);

    // The image is overwritten with the merged contours. A map is constructed
    // to aid in this process.
    arma::uvec fill_map = arma::uvec(contour_counter, arma::fill::ones);
    for (auto it = adj.begin(); it != adj.end(); ++it) {
      fill_map(it->first) = it->first;
      for (auto element : it->second)
	fill_map(element) = it->first;
    }
    image_.transform([&](unsigned char val) { return fill_map(val); });

    cout << "Number contours: " << num_contours_ << endl;

    // Finding the corners of the bounding box for each contour.
    corners_ = umat(4, num_contours_);

    for (int counter = 0; counter < num_contours_; counter++) {
      uvec indices = find(image_ == map_(counter));
      uvec adj_indices = indices / image_.n_rows;
      corners_(0, counter) = arma::min(indices - adj_indices * image_.n_rows);
      corners_(1, counter) = arma::max(indices - adj_indices * image_.n_rows);
      corners_(2, counter) = arma:: min(adj_indices);
      corners_(3, counter) = arma::max(adj_indices);
    }

    cout << corners_ << endl;
  }


  cmat GetContour(int counter) const {
    int index = map_(counter);
    cmat contour =  image_.submat(corners_(0, counter), corners_(2, counter),
				  corners_(1, counter), corners_(3, counter));
    return contour.transform([&](unsigned char val) {
      return (val == index) ? 0 : 255;
    });
  }

  void PrintImage() {
    for (int i = 0; i < image_.n_rows; i++) {
      for (int j = 0; j < image_.n_cols; j++) {
        if (image_(i, j) != 255)
          cout << (int) image_(i, j);
        else
          cout << " ";
        cout << " ";
      }
      cout << endl << endl;
    }
    cout << endl;
  }

  void PrintMap(const std::unordered_map<int, std::unordered_set<int>>& map) {
    for (auto it = map.begin(); it != map.end(); ++it) {
      cout << it->first << ":";
      for (auto element : it->second)
	cout << " " << element;
      cout << endl;
    }
  }

  void Merge(std::unordered_map<int, std::unordered_set<int>>& map) {
    auto seen = std::unordered_set<int>();
    auto prev = std::unordered_set<int>();
    auto queue = std::queue<int>();
    auto ret = std::unordered_map<int, std::unordered_set<int>>();
    int min;
    for (auto it = map.begin(); it != map.end(); ++it) {
      if (seen.count(it->first) == 0) {
        min = it->first;
        queue.push(it->first);
        while (!queue.empty()) {
          auto current = queue.front();
          queue.pop();
          for (auto element : map.at(current)) {
            if (prev.count(element) == 0 && seen.count(element) == 0 && element != min) {
              queue.push(element);
              if (element < min) {
                seen.emplace(min);
                min = element;
              } else {
                seen.emplace(element);
              }
            }
          }
        }
        ret.emplace(min, seen);
        for (auto element : seen)
          prev.emplace(element);
        seen.clear();
      }
    }
    map = ret;
  }
};
