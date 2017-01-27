#include "deterministicalgorithm.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "random.h"

DeterministicAlgorithm::DeterministicAlgorithm(CImg<> input,
                                               unsigned int nbIteration,
                                               bool prematureStop,
                                               unsigned int windowSize,
                                               double gapPercentage,
                                               bool verbose,
                                               bool produceStats)
    : AbstractAlgorithm(input, nbIteration, prematureStop, windowSize, gapPercentage, verbose, produceStats)
{
    computeMask();
    randomInitMask();
}

void DeterministicAlgorithm::computeMask()
{
    // Add every pixels in the image that should be reconstructed
    cimg_forXY(m_image, x, y)
    {
        // Blank pixels
        if (m_image(x, y/*, 0*/) == 255 /*&& input(x, y, 1) == 255 && input(x, y, 2) == 255*/)
            m_mask.push_back({x, y});
        else
            m_outMask.push_back({x, y});
    }
}

void DeterministicAlgorithm::randomInitMask()
{
    const unsigned int nbPixels = m_outMask.size();

    // For each pixel of the mask
    for (const auto& pixel : m_mask)
    {
        unsigned int index = mt() % (nbPixels);
        const auto& seedPixel = m_outMask[index];

        // Initialize the color of the pixel to a random pixel color in the seed image
        m_image(pixel.first, pixel.second/*, 0*/) = m_image(seedPixel.first, seedPixel.second/*, 0*/);
    }
}

void DeterministicAlgorithm::exec()
{
    double lastEnergy = std::numeric_limits<double>::max();

    bool end = false;
    unsigned int i = 0;
    while (!end && i < m_nbIterations)
    {
        double energy = 0;

        for (const auto& pixel : m_mask)
        {
            std::pair<unsigned int, unsigned int> bestMatch(0, 0);
            double lowestDist = std::numeric_limits<double>::max();
            CImg_3x3(I, float);

            unsigned int i = 0;
            cimg_for3x3(m_image, x, y, 0, 0, I, float)
            {
                auto seedPixelCoord = std::pair<unsigned int, unsigned int>(x, y);
                // Pixel is outside mask => skip it
                if (m_outMask[i] != seedPixelCoord) continue; else ++i;

                // Treatments
                const double diffIpp = m_image(pixel.first - 1, pixel.second - 1) - Ipp;
                const double diffIcp = m_image(pixel.first    , pixel.second - 1) - Icp;
                const double diffInp = m_image(pixel.first + 1, pixel.second - 1) - Inp;

                const double diffIpc = m_image(pixel.first - 1, pixel.second) - Ipc;
                //const double diffIcc = m_image(pixel.first    , pixel.second) - Icc;    // Current pixel not in neighbohood
                const double diffInc = m_image(pixel.first + 1, pixel.second) - Inc;

                const double diffIpn = m_image(pixel.first - 1, pixel.second + 1) - Ipn;
                const double diffIcn = m_image(pixel.first    , pixel.second + 1) - Icn;
                const double diffInn = m_image(pixel.first + 1, pixel.second + 1) - Inn;

                double neighborhoodDist = diffIpp*diffIpp + diffIcp*diffIcp + diffInp*diffInp
                                        + diffIpc*diffIpc /*+ diffIcc*diffIcc*/ + diffInc*diffInc
                                        + diffIpn*diffIpn + diffIcn*diffIcn + diffInn*diffInn;

                // If best neighorhood
                if (neighborhoodDist < lowestDist)
                {
                    lowestDist = neighborhoodDist;
                    bestMatch = { x, y };
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

        if (m_enablePrematureStop && computePrematureStop(energy))
        {
            if (m_verbose)
            {
                std::cout << "Algorithm prematuraly stopped at iteration: " << i << std::endl;
            }

            end = true;
        }

        ++i;
    }
}
