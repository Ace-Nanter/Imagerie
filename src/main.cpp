#include <cstdlib>

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>
#include <map>

#include "CImg.h"

using namespace cimg_library;

std::mt19937 mt(123456789); ///< Random generator.

/**
 * @brief Recover all pixels coordinates that need reconstruction.
 * @param input Input image.
 * @param mask Map associating pixel to replace coordinates with pixel for replacing coordinates.
 * @param outMask List of pixel coordinates out of the mask.
 */
void getMask(const CImg<>& input,
             std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
             std::vector< std::pair< unsigned int, unsigned int > >& outMask);

/**
 * @brief Initialize all pixel from mask to a random value from input image.
 * @param mask List of pixel coordinates corresponding to the mask.
 * @param outMask List of pixel coordinates out of the mask.
 */
void randomInitMask(std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
                    const std::vector< std::pair< unsigned int, unsigned int > >& outMask);

/**
 * @brief Use the probabilistic method to emplace mask pixels.
 * @param mask List of pixel coordinates corresponding to the mask.
 * @param outMask List of pixel coordinates out of the mask.
 * @param image Image that will be modified.
 */
void probabilisticMethod(std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
                         const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                         CImg<>& image);

/**
 * @brief Recompose the image taking count of the pixel the map of the mask contains.
 * @param mask Map associating pixel to replace with replacing pixels.
 * @param image Image that will be modified.
 */
void getMask(const CImg<>& input,
             std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
             std::vector< std::pair< unsigned int, unsigned int > >& outMask);

/**
 * @brief getImage Recompse the image taking count of which pixels has been replaced
 * @param mask Map associating pixel to replace with replacing pixels.
 * @param image Image that will be modified.
 */
void getImage(const std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
                CImg<>& image);

// Verbose mode
bool verbose;
bool fileStats;

// Number of iterations
unsigned int nbIterations;

/// MAIN ///
int main(int argc, char** argv)
{
    verbose = cimg_option("-v", true, "Verbose mode");
    fileStats = cimg_option("-f", false, "Write stats to file");
    bool saveResult = cimg_option("-s", false, "Save result to file");
    const char* outputFile = cimg_option("-sf", "output.bmp", "Output file name");
    nbIterations = cimg_option("-n", 5, "Number of iterations");

    CImg<float> input = CImg<float>("images/lenaGrayHiddenSmall.bmp").channel(0);
    CImgDisplay displayInput(input, "Input Image");

    // Pixels that need treatment
    std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > > mask;
    // Other pixels
    std::vector< std::pair< unsigned int, unsigned int > > outMask;
    getMask(input, mask, outMask);

    std::cout << "Number of pixel to reconstitute: " << mask.size() << std::endl;

    // Random initialization
    CImg<float> finalImage(input);
    randomInitMask(mask, outMask);
    getImage(mask, finalImage);

    // Algo
    //probabilisticMethod(mask, outMask, finalImage);

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
             std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
             std::vector< std::pair< unsigned int, unsigned int > >& outMask)
{
    // Add every pixels in the image that should be reconstructed
    cimg_forXY(input, x, y)
    {
        // Blank pixels
        if (input(x, y/*, 0*/) == 255 /*&& input(x, y, 1) == 255 && input(x, y, 2) == 255*/)
            mask.insert({{x,y}, {x,y}});
        else
            outMask.push_back({x, y});
    }
}

void randomInitMask(std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
                    const std::vector< std::pair< unsigned int, unsigned int > >& outMask)
{
    const unsigned int nbPixels = outMask.size();

    // For each pixel of the mask
    for (auto& pixel : mask)
    {
        unsigned int index = mt() % (nbPixels);
        const auto& seedPixel = outMask[index];

        // Initialize the color of the pixel to a random pixel color in the seed image
		pixel.second = seedPixel;
    }
}

void getImage(const std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
				CImg<>& image)
{
	
	for (const auto& pixel : mask)
	{
		image(pixel.first.first, pixel.first.second) = image(pixel.second.first, pixel.second.second);
	}
}

void probabilisticMethod(std::map< std::pair< unsigned int, unsigned int > , std::pair< unsigned int, unsigned int > >& mask,
                         const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                         CImg<>& image)
{

    // TODO : penser à prendre l'exponentielle. Trouver la partie du code qui fait seg fault.
    // TODO : penser que la comparaison se fait sur des probabilités : il faut prendre la probabilité la plus haute et non pas la plus basse
    double lastEnergy = std::numeric_limits<double>::max();

    for (unsigned int i = 0 ; i < nbIterations ; ++i)
    {
        double energy = 0;

        for (auto& pixel : mask)
        {
            double lowestDist = std::numeric_limits<double>::max();
            CImg_3x3(I, float);

            unsigned int i = 0;
            cimg_for3x3(image, x, y, 0, 0, I, float)
            {
                auto seedPixelCoord = std::pair<unsigned int, unsigned int>(x, y);
                // Pixel is outside mask => skip it
                if (outMask[i] != seedPixelCoord) continue; else ++i;

                // Treatments
                std::pair< unsigned int, unsigned int > coords;

                // Ipp
                const double diffIpp1 = image(pixel.first.first - 1, pixel.first.second - 1) - Ipp;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffIpp2 = Icc - image(coords.first - 1, coords.second - 1);

                // Icp
                const double diffIcp1 = image(pixel.first.first, pixel.first.second - 1) - Icp;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffIcp2 = Icc - image(coords.first, coords.second - 1);

                // Inp
                const double diffInp1 = image(pixel.first.first + 1, pixel.first.second - 1) - Inp;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffInp2 = Icc - image(coords.first + 1, coords.second - 1);

                // Ipc
                const double diffIpc1 = image(pixel.first.first - 1, pixel.first.second) - Ipc;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffIpc2 = Icc - image(coords.first - 1, coords.second);

                // Inc
                const double diffInc1 = image(pixel.first.first + 1, pixel.first.second) - Inc;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffInc2 = Icc - image(coords.first + 1, coords.second);

                // Ipn
                const double diffIpn1 = image(pixel.first.first - 1, pixel.first.second + 1) - Ipn;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffIpn2 = Icc - image(coords.first - 1, coords.second + 1);

                // Icn
                const double diffIcn1 = image(pixel.first.first, pixel.first.second + 1) - Icn;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffIcn2 = Icc - image(coords.first, coords.second + 1);

                // Inn
                const double diffInn1 = image(pixel.first.first + 1, pixel.first.second + 1) - Inn;
                coords = mask[std::make_pair(x + 1 , y + 1)];
                const double diffInn2 = Icc - image(coords.first + 1, coords.second + 1);

                double neighborhoodDist = diffIpp1*diffIpp1 + diffIcp1*diffIcp1 + diffInp1*diffInp1
                                        + diffIpc1*diffIpc1 + diffInc1*diffInc1
                                        + diffIpn1*diffIpn1 + diffIcn1*diffIcn1 + diffInn1*diffInn1
                                        + diffIpp2*diffIpp2 + diffIcp2*diffIcp2 + diffInp2*diffInp2
                                        + diffIpc2*diffIpc2 + diffInc2*diffInc2
                                        + diffIpn2*diffIpn2 + diffIcn2*diffIcn2 + diffInn2*diffInn2;

                // If best neighorhood
                if (neighborhoodDist < lowestDist)
                {
                    lowestDist = neighborhoodDist;
                    pixel.second.first = x;
                    pixel.second.second = y;
                }
            }

            energy += lowestDist;

            // Update image
            image(pixel.first.first, pixel.first.second) = image(pixel.second.first, pixel.second.second);
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
