#include "Array2d.hpp"
#include <memory>

class PPM {
public:
	PPM(char *fileName);
	PPM(int rows, int cols, int maxVal, std::shared_ptr<Array2d>, std::shared_ptr<Array2d>, std::shared_ptr<Array2d>);
	~PPM();
	
	void write(char *fileName);

	// double get(int row, int col);
	// void set(double val, int row, int col);
	// void mult(double scalar);

	int maxVal;
	int rows;
	int cols;

	std::shared_ptr<Array2d> getRed();
	std::shared_ptr<Array2d> getGreen();
	std::shared_ptr<Array2d> getBlue();



private:

	std::shared_ptr<Array2d> red;
	std::shared_ptr<Array2d> green;
	std::shared_ptr<Array2d> blue;

	std::shared_ptr<std::string> outputPixel(int row, int col);
};