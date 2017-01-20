#include "utils.h"

using namespace cimg_library;

void Utils::getNeighboors(const CImg<> & image,
	CImg<> * neighboorhood,
	unsigned int x,
	unsigned int y,
	unsigned int size)
{
	for (int i = 0; i < size; i++) {
		for (unsigned int j = 0; j < size; j++)
		{
			const double xCoord = x - (size / 2) + i;
			const double yCoord = y - (size / 2) + j;

			// If there is a problem with the neighboorhood we throw an exception
			if (xCoord < 0 || xCoord >= image.width()
				|| yCoord < 0 || yCoord >= image.height()) {
				throw new std::exception("Incorrect neighboorhood asked !");
			}
			else {
				(*neighboorhood)(i, j) = image(xCoord, yCoord);
			}
		}
	}

	for (int k = 0; k < 3; k++) {
		for (int l = 0; l < 3; l++) {
			double yolo = (*neighboorhood)(k, l);
			std::cout << yolo << std::endl;
		}
	}
}

double Utils::getNeighboorEuclidianDistance(const CImg<> & image,
	unsigned int xA, unsigned int yA,
	unsigned int xB, unsigned int yB,
	unsigned int size)
{
	double distance = 0;

	// Create neighboorhoods
	CImg<> neighboorhoodA = CImg<>(size, size);
	CImg<> neighboorhoodB = CImg<>(size, size);
	
	// Get neighboorhoods
	Utils::getNeighboors(image, &neighboorhoodA, xA, yA, size);
	Utils::getNeighboors(image, &neighboorhoodB, xB, yB, size);

	// Treatments
	cimg_forXY(neighboorhoodA, x, y)
	{
		// Avoid the case we are pointing current point
		if ((x != (size / 2)) || (y != (size / 2)))
		{
			const double diff = neighboorhoodA(x, y) - neighboorhoodB(x, y);
			distance += (diff * diff);
		}
	}

	return distance;
}

double Utils::getNeighboorNonCausalDistance(const CImg<> & image,
	std::map < std::pair < unsigned int, unsigned int >, std::pair < unsigned int, unsigned int >> & mask,
	unsigned int xA, unsigned int yA,
	unsigned int xB, unsigned int yB,
	unsigned int size)
{
	double finalResult = 0;
	double distance = 0;

	// Create neighboorhood
	CImg<> neighboorhoodA = CImg<>(size, size);

	// Get neighboorhoods
	Utils::getNeighboors(image, &neighboorhoodA, xA, yA, size);

	// Treatments
	for (int i = 0; i < size; i++)
	{
		for (unsigned int j = 0; j < size; j++)
		{
			const double xCoord = xB - (size / 2) + i;
			const double yCoord = yB - (size / 2) + j;

			// First distance
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					double yolo = neighboorhoodA(k, l);
					std::cout << yolo << std::endl;
				}
			}

			const double diff1 = neighboorhoodA(i, j) - image(xCoord, yCoord);
			distance += (diff1 * diff1);
			
			// Second distance
			
			if (xCoord != xB || yCoord != yB)	// Avoid the center
			{
				// If there is a problem with the neighboorhood we throw an exception
				if (xCoord < 0 || xCoord >= image.width()
					|| yCoord < 0 || yCoord >= image.height())
				{
					throw new std::exception("Incorrect neighboorhood asked !");
				}

				// Getting f(alpha) coordinates
				std::pair < unsigned int, unsigned int > coordsFA;

				if (mask.count({ xCoord, yCoord }))		// If the neighboor is in the mask
				{
					coordsFA = mask[{ xCoord, yCoord }];
				}
				else									// The neighboor is part of the picture
				{
					coordsFA = { xCoord, yCoord };
				}
				
				const double diff2 = image(xA, yA) - image(coordsFA.first + (size / 2) - i, coordsFA.second + (size / 2) - j);
				distance += (diff2 * diff2);
			}
		}
	}

	finalResult = exp(0-distance);			// TODO : problem here

	return finalResult;
}