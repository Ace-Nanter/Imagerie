#include "codebookprobabilistic.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "random.h"

CodebookProbabilistic::CodebookProbabilistic(CImg<> input,
                                             unsigned int neighborhoodSize,
                                             unsigned int nbIteration,
                                             bool prematureStop,
                                             unsigned int windowSize,
                                             double gapPercentage,
                                             bool verbose,
                                             bool produceStats)
    : AbstractAlgorithm(input, nbIteration, prematureStop, windowSize, gapPercentage, verbose, produceStats)
	, m_neighborhoodSize(neighborhoodSize)
{
	computeMask();
	randomInitMask();
}

void CodebookProbabilistic::computeMask()
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
            const unsigned int beginX = std::max(unsigned(1), x - m_neighborhoodSize);
			const unsigned int endX = std::min(x + m_neighborhoodSize, unsigned(m_image.width() - 1));

            const unsigned int beginY = std::max(unsigned(1), y - m_neighborhoodSize);
			const unsigned int endY = std::min(y + m_neighborhoodSize, unsigned(m_image.height() - 1));

			for (unsigned int i = beginX; i < endX && i != unsigned(x); ++i)
			{
				for (unsigned int j = beginY; j < endY && j != unsigned(y); ++j)
				{
					neighbors.push_back({ i, j });
				}
			}

            m_neighboorMask.insert({ pixel, neighbors });
            m_mappingMask.insert({ pixel, pixel });
		}
		else
			m_outMask.push_back({ x, y });
	}
}

void CodebookProbabilistic::randomInitMask()
{
	const unsigned int nbPixels = m_outMask.size();

	// For each pixel of the mask
    for (auto& pixelAssoc : m_mappingMask)
	{
		unsigned int index = mt() % (nbPixels);
		const auto& seedPixel = m_outMask[index];

		// Initialize the color of the pixel to a random pixel color in the seed image
        pixelAssoc.second = seedPixel;
		m_image(pixelAssoc.first.first, pixelAssoc.first.second/*, 0*/) = m_image(seedPixel.first, seedPixel.second/*, 0*/);
	}
}

void CodebookProbabilistic::exec()
{
	double lastEnergy = std::numeric_limits<double>::max();

	for (unsigned int i = 0; i < m_nbIterations; ++i)
	{
		double energy = 0;

        // For every pixel in the mask
        for (const auto& pixelAssoc : m_neighboorMask)
		{
			std::pair<unsigned int, unsigned int> bestMatch(0, 0);
			double lowestDist = std::numeric_limits<double>::max();

			const auto& pixel = pixelAssoc.first;
			const auto& neighbors = pixelAssoc.second;

            // For every pixel in the neighboorhood
			for (const auto& neighbor : neighbors)
			{
                // Treatments

                // First distance
                const double diffIpp1 = m_image(pixel.first - 1, pixel.second - 1) - m_image(neighbor.first - 1, neighbor.second - 1);
                const double diffIcp1 = m_image(pixel.first, pixel.second - 1) - m_image(neighbor.first, neighbor.second - 1);
                const double diffInp1 = m_image(pixel.first + 1, pixel.second - 1) - m_image(neighbor.first + 1, neighbor.second - 1);

                const double diffIpc1 = m_image(pixel.first - 1, pixel.second) - m_image(neighbor.first - 1, neighbor.second);
                const double diffInc1 = m_image(pixel.first + 1, pixel.second) - m_image(neighbor.first + 1, neighbor.second);

                const double diffIpn1 = m_image(pixel.first - 1, pixel.second + 1) - m_image(neighbor.first - 1, neighbor.second + 1);
                const double diffIcn1 = m_image(pixel.first, pixel.second + 1) - m_image(neighbor.first, neighbor.second + 1);
                const double diffInn1 = m_image(pixel.first + 1, pixel.second + 1) - m_image(neighbor.first + 1, neighbor.second + 1);

                double distance = diffIpp1 *diffIpp1 + diffIcp1 * diffIcp1 + diffInp1 * diffInp1
                    + diffIpc1 * diffIpc1 /*+ diffIcc*diffIcc*/ + diffInc1 * diffInc1
                    + diffIpn1 * diffIpn1 + diffIcn1 * diffIcn1 + diffInn1* diffInn1;

                // Second distance

                const float pointValue = m_image(neighbor.first, neighbor.second);

                const double diffIpp2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first - 1, pixel.second - 1);
                const double diffIcp2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first, pixel.second - 1);
                const double diffInp2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first + 1, pixel.second - 1);

                const double diffIpc2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first - 1, pixel.second);
                const double diffInc2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first + 1, pixel.second);

                const double diffIpn2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first - 1, pixel.second + 1);
                const double diffIcn2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first, pixel.second + 1);
                const double diffInn2 = distanceNonCausal(pointValue, pixel.first, pixel.second, pixel.first + 1, pixel.second + 1);

                distance += diffIpp2 *diffIpp2 + diffIcp2 * diffIcp2 + diffInp2 * diffInp2
                    + diffIpc2 * diffIpc2 + diffInc2 * diffInc2
                    + diffIpn2 * diffIpn2 + diffIcn2 * diffIcn2 + diffInn2* diffInn2;


                // If best probability
                if (distance < lowestDist)
                {
                    lowestDist = distance;
                    bestMatch = { neighbor.first, neighbor.second };
                }
			}

			energy += lowestDist;

            // Set new pixel color and update Map
            m_mappingMask[pixelAssoc.first] = bestMatch;
            m_image(pixel.first, pixel.second) = m_image(bestMatch.first, bestMatch.second);
		}

		// Iteration results
		double ratio = (lastEnergy - energy) / double(lastEnergy);
		ratio = ratio > 0 ? ratio : -ratio;

		// Stats and verbose
		if (m_verbose)
		{
			if (m_fileStats)
			{
				std::stringstream ss;
				ss << "./loop" << i;
				std::ofstream ofs(ss.str(), std::ios::trunc | std::ios::out);
				ofs << "Last Energy : " << lastEnergy << "\nEnergy : " << energy << "\nRatio : " << ratio << "\n\n";
				ofs.close();
			}

			std::cout << "Loop : " << i << "\nLast Energy : " << lastEnergy << "\nEnergy : " << energy << "\nRatio : " << ratio << "\n" << std::endl;
		}

		lastEnergy = energy;
	}
}

double CodebookProbabilistic::distanceNonCausal(float value, int xA, int yA, int xB, int yB) {

    double distance = 0;

    auto p = m_mappingMask.find({ xB, yB });
    if (p != m_mappingMask.end()) {
        p->second.first = xA + (xA - xB);
        p->second.second = yA + (yA - yB);

        distance = value - m_image(p->second.first, p->second.second);
    }
    else {		// No association found, the ponderation is null
        distance = 0;
    }

    return distance;
}
