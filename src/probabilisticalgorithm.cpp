#include "probabilisticalgorithm.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "random.h"

ProbabilisticAlgorithm::ProbabilisticAlgorithm(CImg<> input, unsigned int neighborhoodSize, unsigned int nbIteration, bool verbose, bool produceStats)
	: AbstractAlgorithm(input, nbIteration, verbose, produceStats)
	, m_neighborhoodSize(neighborhoodSize)
{
	computeMask();
	randomInitMask();
}

void ProbabilisticAlgorithm::computeMask()
{
	// Add every pixels in the image that should be reconstructed
	cimg_forXY(m_image, x, y)
	{
		// Blank pixels
		if (m_image(x, y/*, 0*/) == 255 /*&& input(x, y, 1) == 255 && input(x, y, 2) == 255*/)
		{
			// Get all neighbors pixel in a range of size neighborhoodSize
			Point pixel = { x, y };
			PointSet neighbors;

			// Security margin of one (for safe neighborhood loops)
			const unsigned int beginX = std::max(unsigned(0), x - m_neighborhoodSize);
			const unsigned int endX = std::min(x + m_neighborhoodSize, unsigned(m_image.width() - 1));

			const unsigned int beginY = std::max(unsigned(0), y - m_neighborhoodSize);
			const unsigned int endY = std::min(y + m_neighborhoodSize, unsigned(m_image.height() - 1));

			for (unsigned int i = beginX; i < endX && i != unsigned(x); ++i)
			{
				for (unsigned int j = beginY; j < endY && j != unsigned(y); ++j)
				{
					neighbors.push_back({ i, j });
				}
			}

			m_mask.insert({ pixel, neighbors });
		}
		else
			m_outMask.push_back({ x, y });
	}
}

void CodebookAlgorithm::randomInitMask()
{
	const unsigned int nbPixels = m_outMask.size();

	// For each pixel of the mask
	for (const auto& pixelAssoc : m_mask)
	{
		unsigned int index = mt() % (nbPixels);
		const auto& seedPixel = m_outMask[index];

		// Initialize the color of the pixel to a random pixel color in the seed image
		m_image(pixelAssoc.first.first, pixelAssoc.first.second/*, 0*/) = m_image(seedPixel.first, seedPixel.second/*, 0*/);
	}
}
