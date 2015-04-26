#include "PPM.hpp"
#include <fstream>
// #include <ifstream>
#include <memory>

#define CHARACTERS_PER_PIXEL 3 + 1 + 3 + 1 + 3 + 1
#define MAX_CHARACTERS_PER_LINE 70-1
#define MAX_PIXELS_PER_LINE MAX_CHARACTERS_PER_LINE / CHARACTERS_PER_PIXEL

PPM::PPM(char *fileName)
{
	std::ifstream file;
	file.open(fileName);
}

PPM::PPM(int r, int c, int m, std::shared_ptr<Array2d> redArray, std::shared_ptr<Array2d> greenArray, std::shared_ptr<Array2d> blueArray) :
maxVal(m), rows(r), cols(c), red(redArray), green(greenArray), blue(blueArray)
{

}

PPM::~PPM()
{

}

unsigned char convert(double val)
{
	return (unsigned char) val;
}

std::shared_ptr<std::string> PPM::outputPixel(int row, int col)
{
	std::string buff = std::string();
	buff += std::to_string(convert(red->get(row, col))) + " ";
	buff += std::to_string(convert(green->get(row, col))) + " ";
	buff += std::to_string(convert(blue->get(row, col))) + " ";

	return std::make_shared<std::string>(buff);
}

void PPM::write(char *fileName)
{
	std::ofstream file;
	file.open(fileName);

	file << "P3" << std::endl;

	file << cols << " " << rows << std::endl;

	file << maxVal << std::endl;


	int pixelCount = 0;
	std::string buff = std::string();
	for(int i=0; i<rows; i++)
	{
		int j=0;
		while(j < cols)
		{
			buff += *(outputPixel(i, j));
			pixelCount += 1;

			if(pixelCount >= MAX_PIXELS_PER_LINE)
			{
				//dump to file
				file << buff << std::endl;

				//reset pixel count and buffer
				pixelCount = 0;
				buff = std::string();
			}

			j+=1;
		}

	}

	if(pixelCount > 0)
	{
		file << buff << std::endl;
	}

}

std::shared_ptr<Array2d> PPM::getRed()
{
	return red;
}

std::shared_ptr<Array2d> PPM::getGreen()
{
	return green;
}

std::shared_ptr<Array2d> PPM::getBlue()
{
	return blue;
}