/**
 * @author Jess Smith <smith.jessk@gmail.com>
 * @copyright 2014 Jess Smith
 * @license MIT
 */

// Armadillo
#include <armadillo>

using namespace arma;

class Symbol {
private:
	// The id this symbol corresponds to (enumeration found in enum.tsv)
	int id_,

	// x_ and y_ are the pixel coordinates of the top-left corner of the image
	// Note that x increases left to right, y increases top to bottom
	x_, y_, width_, height_,

	// The id_ of the row that this symbol was found in
	row_;

public:
	ParseSymbol(int id, int x, int y, int width, int height, int row){
		// Assigns all passes values to their respective local versions
		id_ = id;

		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;

		row_ = row;
	}
}