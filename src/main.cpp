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
	
    // Algo
    probabilisticMethod(mask, outMask, finalImage);

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
    
}
