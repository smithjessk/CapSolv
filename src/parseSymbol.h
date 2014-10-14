/**
 * An object representin a symbol in the parsing algorithm
 *
 *
 * Used by ./process.cpp
 *
 * @author Jess Smith <smith.jessk@gmail.com>
 * @copyright 2014 Jess Smith
 * @license MIT
 */

// Armadillo
#include <armadillo>

using namespace arma;

class ParseSymbol {
private:
	// The id this symbol corresponds to (enumeration found in enum.tsv)
	int id_,

	// x_ and y_ are the pixel coordinates of the top-left corner of the image
	// Note that x increases left to right, y increases top to bottom
	x_, y_, width_, height_,

	// The id_ of the row that this symbol was found in
	// For clarification, see ./parseRow.h
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
	};

	/** GETTER METHODS **/

	// Returns the row in which this symbol is found
	int GetRow() {
		return row_;
	}

	// Returns the id that this symbol has been predicted as
	int GetEnumId() {
		return id_;
	}

	// Returns a vector containing the coordinates of the top-left point of the
	// symbol, its width, and its height
	arma::ivec GetDimensions() {
		return ivec({x_, y_, width_, height_});
	}

	/** SETTER METHODS **/

	// Sets the id that this symbol has been predicted as
	// For details, see ../enum.tsv
	void SetId(int id) {
		id_ = id;
	}

	// Sets the row in which this symbol is found
	void SetRow(int row) {
		row_ = row;
	}
};
