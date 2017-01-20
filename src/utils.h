#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <map>
#include <cmath>
#include <cstdlib>

#include <iostream>



#include "CImg.h"

using namespace cimg_library;

class Utils {

	public:

		/**
		 * @brief Get the neighboorhood of a given pixel.
		 * @param image Image where to look for the pixels.
		 * @param neighboorhood Neighboorhood returned.
		 * @param x X coordinate of the given pixel.
		 * @param y Y coordinate of the given pixel.
		 * @param size Size of the neighborhood to get.
		 * @return A CImg containing the neighboorhood.
		 */
		static void getNeighboors(const CImg<> & image,
			CImg<> * neighboorhood,
			unsigned int x,
			unsigned int y,
			unsigned int size);

		/**
		 * @brief Get the neighboorhood distance (in the normal way) between two given points.
		 * @param image Image where to look for the pixels.
		 * @param xA X coordinate of the first given pixel.
		 * @param yA Y coordinate of the first given pixel.
		 * @param xB X coordinate of the second given pixel.
		 * @param yB Y coordinate of the second given pixel.
		 * @param size size of the neighboorhood to use.
		 * @return Distance between the two neighboorhoods.
		*/
		static double getNeighboorEuclidianDistance(const CImg<> & image,
			unsigned int xA, unsigned int yA,
			unsigned int xB, unsigned int yB,
			unsigned int size);
		
		/**
		* @brief Get the neighboorhood distance (in non causal situation) between two given points.
		* @param image Image where to look for the pixels.
		* @param mask needed to get coordinates of the replaced pixels.
		* @param xA X coordinate of the first given pixel.
		* @param yA Y coordinate of the first given pixel.
		* @param xB X coordinate of the second given pixel.
		* @param yB Y coordinate of the second given pixel.
		* @param size size of the neighboorhood to use.
		* @return Distance between the two neighboorhoods.
		*/
		static double getNeighboorNonCausalDistance(const CImg<> & image,
			std::map < std::pair < unsigned int, unsigned int >, std::pair < unsigned int, unsigned int >> & mask,
			unsigned int xA, unsigned int yA,
			unsigned int xB, unsigned int yB,
			unsigned int size);

};


#endif // !__UTILS_H__
