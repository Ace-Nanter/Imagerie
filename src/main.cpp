#include <cstdlib>

#include <iostream>

#include "CImg.h"

#include "codebookalgorithm.h"

using namespace cimg_library;

/**
 * @brief Compare two images and return an image that show the differences between them (Black means equals).
 * @param origin Original image.
 * @param result Result image.
 * @return Image showing the difference.
 */
CImg<> compare(const CImg<>& origin, const CImg<>& result);

/// MAIN ///
int main(int argc, char** argv)
{
    const bool verbose = cimg_option("-v", false, "Verbose mode");
    const bool fileStats = cimg_option("-f", false, "Write stats to file");
    const bool prematureStop = cimg_option("-p", true, "Enable/Disable premature stop (default enabled)");
    const unsigned int windowSize = cimg_option("-w", 10, "Window size tused to perform premature stop");
    const double gap = cimg_option("-g", 0.01, "Gap in percentage to use to compare to median");
    const bool saveResult = cimg_option("-s", false, "Save result to file");
    const char* originalFile = cimg_option("-oif", "images/lenaGray.bmp", "Original image file name");
    const char* inputFile = cimg_option("-if", "images/lenaGrayHiddenSmall.bmp", "Input image file name");
    const char* outputFile = cimg_option("-of", "output.bmp", "Output file name");
    const char* outputCompareFile = cimg_option("-ocf", "outputCompare.bmp", "Output comparison image file name");
    const unsigned int nbIterations = cimg_option("-n", 5, "Number of iterations");
    const unsigned int neighborhoodSize = cimg_option("-ns", 20, "For Codebook optimization define the neighborhood size to consider");

    const CImg<float> origin = CImg<float>(originalFile).channel(0);
    const CImg<float> input = CImg<float>(inputFile).channel(0);
    CImgDisplay displayInput(input, "Input Image");

    // Create algorithm
    AbstractAlgorithm* algo = new CodebookAlgorithm(input, neighborhoodSize, nbIterations, prematureStop, windowSize, gap, verbose, fileStats);
	
    // Algo
    algo->exec();

    // Results
    const CImg<>& result = algo->getResult();
    const CImg<> comparison = compare(origin, result);

    if (saveResult)
    {
        result.save(outputFile);
        comparison.save(outputCompareFile);
    }

    CImgDisplay displayFinalImage(result, "Result Image");
    CImgDisplay displayCompareImage(comparison, "Compare Image", 0);
    while (!displayInput.is_closed() || !displayFinalImage.is_closed() || !displayCompareImage.is_closed())
    {
        displayInput.wait();
    }

    delete algo;

    return EXIT_SUCCESS;
}

/// Other functions ///
CImg<> compare(const CImg<>& origin, const CImg<>& result)
{
    CImg<> ret(origin);

    ret = (origin - result).abs();

    return ret;
}
