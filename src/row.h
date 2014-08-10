#include <armadillo>

using namespace arma;

class Row {
private: 
  int id_, aboveId_, belowId_, resultIndex_, 
  startAbove_, startMain_, endMain_, endBelow_;

public: 
  Row(int id, int aboveId, int belowId, int resultIndex, 
    int startAbove, int startMain, int endMain, int endBelow) {
    id_ = id;
    aboveId_ = aboveId;
    belowId_ = belowId;
    resultIndex_ = resultIndex;
    startAbove_ = startAbove;
    startMain_ = startMain;
    endMain_ = endMain;
    endBelow_ = endBelow;
  }
};