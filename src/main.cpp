#include <cstdlib>

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "CImg.h"

using namespace cimg_library;

std::mt19937 mt(123456789); ///< Random generator.

/**
 * @brief Recover all pixels coordinates that need reconstruction.
 * @param input Input image.
 * @param mask List of pixel coordinates corresponding to the mask.
 * @param outMask List of pixel coordinates out of the mask.
 */
void getMask(const CImg<>& input,
             std::vector< std::pair< unsigned int, unsigned int > >& mask,
             std::vector< std::pair< unsigned int, unsigned int > >& outMask);

/**
 * @brief Initialize all pixel from mask to a random value from input image.
 * @param mask List of pixel coordinates corresponding to the mask.
 * @param outMask List of pixel coordinates out of the mask.
 * @param input Input image.
 */
void randomInitMask(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                    const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                    CImg<>& input);

/**
 * @brief Use the deterministic method to emplace mask pixels.
 * @param mask List of pixel coordinates corresponding to the mask.
 * @param outMask List of pixel coordinates out of the mask.
 * @param image Image that will be modified.
 */
void deterministicMethod(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                         const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                         CImg<>& image);

// Verbose mode
bool verbose;
bool fileStats;

// Number of iterations
unsigned int nbIterations;

/// MAIN ///
int main(int argc, char** argv)
{
    verbose = cimg_option("-v", false, "Verbose mode");
    fileStats = cimg_option("-f", false, "Write stats to file");
    bool saveResult = cimg_option("-s", false, "Save result to file");
    const char* inputFile = cimg_option("-if", "images/lenaGrayHiddenSmall.bmp", "Input file name");
    const char* outputFile = cimg_option("-of", "output.bmp", "Output file name");
    nbIterations = cimg_option("-n", 5, "Number of iterations");

    CImg<float> input = CImg<float>(inputFile).channel(0);
    CImgDisplay displayInput(input, "Input Image");

    // Pixels that need treatment
    std::vector< std::pair< unsigned int, unsigned int > > mask;
    // Other pixels
    std::vector< std::pair< unsigned int, unsigned int > > outMask;
    getMask(input, mask, outMask);

    std::cout << "Number of pixel to reconstitute: " << mask.size() << std::endl;

    // Random initialization
    CImg<float> finalImage(input);
    randomInitMask(mask, outMask, finalImage);
	
    // Algo
    deterministicMethod(mask, outMask, finalImage);

    // Results
    if (saveResult)
        finalImage.save(outputFile);

    CImgDisplay displayFinalImage(finalImage, "Result Image");
    while (!displayInput.is_closed() || !displayFinalImage.is_closed())
    {
        displayInput.wait();
    }

    return EXIT_SUCCESS;
}


void getMask(const CImg<>& input,
             std::vector< std::pair< unsigned int, unsigned int > >& mask,
             std::vector< std::pair< unsigned int, unsigned int > >& outMask)
{
    // Add every pixels in the image that should be reconstructed
    cimg_forXY(input, x, y)
    {
        // Blank pixels
        if (input(x, y/*, 0*/) == 255 /*&& input(x, y, 1) == 255 && input(x, y, 2) == 255*/)
            mask.push_back({x, y});
        else
            outMask.push_back({x, y});
    }
}

void randomInitMask(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                    const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                    CImg<>& input)
{
    const unsigned int nbPixels = outMask.size();

    // For each pixel of the mask
    for (const auto& pixel : mask)
    {
        unsigned int index = mt() % (nbPixels);
        const auto& seedPixel = outMask[index];

        // Initialize the color of the pixel to a random pixel color in the seed image
        input(pixel.first, pixel.second/*, 0*/) = input(seedPixel.first, seedPixel.second/*, 0*/);
    }
}

void deterministicMethod(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                         const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                         CImg<>& image)
{
    double lastEnergy = std::numeric_limits<double>::max();

    for (unsigned int i = 0 ; i < nbIterations ; ++i)
    {
        double energy = 0;

        for (const auto& pixel : mask)
        {
            std::pair<unsigned int, unsigned int> bestMatch(0, 0);
            double lowestDist = std::numeric_limits<double>::max();
            CImg_3x3(I, float);

            unsigned int i = 0;
            cimg_for3x3(image, x, y, 0, 0, I, float)
            {
                auto seedPixelCoord = std::pair<unsigned int, unsigned int>(x, y);
                // Pixel is outside mask => skip it
                if (outMask[i] != seedPixelCoord) continue; else ++i;

                // Treatments
                const double diffIpp = image(pixel.first - 1, pixel.second - 1) - Ipp;
                const double diffIcp = image(pixel.first    , pixel.second - 1) - Icp;
                const double diffInp = image(pixel.first + 1, pixel.second - 1) - Inp;

                const double diffIpc = image(pixel.first - 1, pixel.second) - Ipc;
                //const double diffIcc = image(pixel.first    , pixel.second) - Icc;    // Current pixel not in neighbohood
                const double diffInc = image(pixel.first + 1, pixel.second) - Inc;

                const double diffIpn = image(pixel.first - 1, pixel.second + 1) - Ipn;
                const double diffIcn = image(pixel.first    , pixel.second + 1) - Icn;
                const double diffInn = image(pixel.first + 1, pixel.second + 1) - Inn;

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
            image(pixel.first, pixel.second) = image(bestMatch.first, bestMatch.second);
        }

        // Iteration results
        double ratio = (lastEnergy - energy) / double(lastEnergy);
        ratio = ratio > 0 ? ratio : -ratio;

        // Stats and verbose
        if (verbose)
        {
            if (fileStats)
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
