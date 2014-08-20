/**
 * @author Jess Smith <smith.jessk@gmail.com>
 * @copyright 2014 Jess Smith
 * @license MIT
 *
 * TODO: 
 *  Add description of the inital main row's pixel boundaries
 */

// Armadillo
#include <armadillo>

using namespace arma;

class ParseRow {
private: 
  // The original vector indices of the row, the row it is above, 
  // and the row it is below. Note that an index of -1 for aboveId_ or belowId_ 
  // means that the row is respectively either above or below no current rows
  int id_, aboveId_, belowId_, 

  // Vertical pixel addresses of the appropriate beginnings or endings of various
  // rows. These are used to determine if a symbol is an exponent, subscript, etc
  startAbove_, startMain_, endMain_, endBelow_,


  // A stopgap measure to ensure proper placement of the strings into the result
  resultIndex_;

public: 
  // Assigns all passes values to their respective local versions
  ParseRow(int id, int aboveId, int belowId,
    int startAbove, int startMain, int endMain, int endBelow, int resultIndex) {
    id_ = id;
    aboveId_ = aboveId;
    belowId_ = belowId;

    startAbove_ = startAbove;
    startMain_ = startMain;
    endMain_ = endMain;
    endBelow_ = endBelow;

    resultIndex_ = resultIndex;
  }

  // Return the row's id
  int GetId() {
    return id_;
  };

  // Return the id of the row that this is above
  int GetAboveId() {
    return aboveId_;
  }

  // Return the id of the row that this is below
  int GetBelowId() {
    return belowId_;
  }

  // Return the index at which this row's characters should be applied
  int GetResultIndex() {
    return resultIndex_;
  }

  // Changes the index at which this row's characters should be applied
  void ChangeResultIndex(int amount) {
    resultIndex_ += amount;
  }

  // Return locations of the delimitters between the above, main, and sub-rows
  ivec GetBoundaries() {
    return ivec({startAbove_, startMain_, endMain_, 
      endBelow_});
  };
};