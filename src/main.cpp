#include <cstdlib>

#include <iostream>

#include "CImg.h"

#include "deterministicalgorithm.h"
#include "probabilisticalgorithm.h"
#include "codebookdeterministic.h"
#include "codebookprobabilistic.h"

using namespace cimg_library;

/**
 * @brief Compare two images and return an image that show the differences between them (Black means equals).
 * @param origin Original image.
 * @param result Result image.
 * @return Image showing the difference.
 */
CImg<> compare(const CImg<>& origin, const CImg<>& result);

/**
 * @brief The Method enum Enumerate all possible algorithms.
 */
enum Method
{
    DETERMINISTIC = 1,
    DETERMINISTIC_CODEBOOK = 2,
    PROBABILISTIC = 3,
    PROBABILISTIC_CODEBOOK = 4,
};

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
    const int method = cimg_option("-a", Method::DETERMINISTIC_CODEBOOK, "Algorithm to use: \n\
                                                                          1 = Deterministic Method \n\
                                                                          2 = Codebook Optimization (Deterministic Method)\n\
                                                                          3 = Probabilistic Method \n\
                                                                          4 = Codebook Optimization (Probabilistic Method)");

    const CImg<float> origin = CImg<float>(originalFile).channel(0);
    const CImg<float> input = CImg<float>(inputFile).channel(0);
    CImgDisplay displayInput(input, "Input Image");

    // Create algorithm
    AbstractAlgorithm* algo = nullptr;
    switch (method)
    {
    case Method::DETERMINISTIC:
        algo = new DeterministicAlgorithm(input, nbIterations, prematureStop, windowSize, gap, verbose, fileStats);
        break;
    case Method::DETERMINISTIC_CODEBOOK:
        algo = new CodebookDeterministic(input, neighborhoodSize, nbIterations, prematureStop, windowSize, gap, verbose, fileStats);
        break;
    case Method::PROBABILISTIC:
        algo = new ProbabilisticAlgorithm(input, nbIterations, prematureStop, windowSize, gap, verbose, fileStats);
        break;
    case Method::PROBABILISTIC_CODEBOOK:
        algo = new CodebookProbabilistic(input, neighborhoodSize, nbIterations, prematureStop, windowSize, gap, verbose, fileStats);
        break;
    default:
        algo = new CodebookDeterministic(input, neighborhoodSize, nbIterations, prematureStop, windowSize, gap, verbose, fileStats);
        break;
    }
	
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
