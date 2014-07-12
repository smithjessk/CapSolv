#include <armadillo>

using namespace arma;
using namespace std;

int main() {
	uvec x ({1, 2, 3});
	x *= 2.4;
	cout << x << endl;
}