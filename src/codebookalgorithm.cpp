#include "codebookalgorithm.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "random.h"

CodebookAlgorithm::CodebookAlgorithm(CImg<> input, unsigned int neighborhoodSize, unsigned int nbIteration, bool verbose, bool produceStats)
    : AbstractAlgorithm(input, nbIteration, verbose, produceStats)
    , m_neighborhoodSize(neighborhoodSize)
{
    computeMask();
    randomInitMask();
}

void CodebookAlgorithm::computeMask()
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

            for (unsigned int i = beginX ; i < endX && i != unsigned(x) ; ++i)
            {
                for (unsigned int j = beginY ; j < endY && j != unsigned(y) ; ++j)
                {
                    neighbors.push_back({ i, j });
                }
            }

            m_mask.insert({ pixel, neighbors });
        }
        else
            m_outMask.push_back({x, y});
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

void CodebookAlgorithm::exec()
{
    double lastEnergy = std::numeric_limits<double>::max();

    for (unsigned int i = 0 ; i < m_nbIterations ; ++i)
    {
        double energy = 0;

        for (const auto& pixelAssoc : m_mask)
        {
            std::pair<unsigned int, unsigned int> bestMatch(0, 0);
            double lowestDist = std::numeric_limits<double>::max();

            const auto& pixel = pixelAssoc.first;
            const auto& neighbors = pixelAssoc.second;

            for (const auto& neighbor : neighbors)
            {
                // Treatments
                const double diffIpp = m_image(pixel.first - 1, pixel.second - 1) - m_image(neighbor.first - 1, neighbor.second - 1);
                const double diffIcp = m_image(pixel.first    , pixel.second - 1) - m_image(neighbor.first    , neighbor.second - 1);
                const double diffInp = m_image(pixel.first + 1, pixel.second - 1) - m_image(neighbor.first + 1, neighbor.second - 1);

                const double diffIpc = m_image(pixel.first - 1, pixel.second) - m_image(neighbor.first - 1, neighbor.second);
                //const double diffIcc = m_image(pixel.first    , pixel.second) - m_image(neighbor.first    , neighbor.second);    // Current pixel not in neighbohood
                const double diffInc = m_image(pixel.first + 1, pixel.second) - m_image(neighbor.first + 1, neighbor.second);

                const double diffIpn = m_image(pixel.first - 1, pixel.second + 1) - m_image(neighbor.first - 1, neighbor.second + 1);
                const double diffIcn = m_image(pixel.first    , pixel.second + 1) - m_image(neighbor.first    , neighbor.second + 1);
                const double diffInn = m_image(pixel.first + 1, pixel.second + 1) - m_image(neighbor.first + 1, neighbor.second + 1);

                double neighborhoodDist = diffIpp*diffIpp + diffIcp*diffIcp + diffInp*diffInp
                                        + diffIpc*diffIpc /*+ diffIcc*diffIcc*/ + diffInc*diffInc
                                        + diffIpn*diffIpn + diffIcn*diffIcn + diffInn*diffInn;

                // If best neighorhood
                if (neighborhoodDist < lowestDist)
                {
                    lowestDist = neighborhoodDist;
                    bestMatch = { neighbor.first, neighbor.second };
                }
            }

            energy += lowestDist;

            // Set new pixel color
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
