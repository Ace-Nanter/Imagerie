#include <cstdlib>

#include <iostream>

#include "CImg.h"

#include "codebookalgorithm.h"

using namespace cimg_library;

/// MAIN ///
int main(int argc, char** argv)
{
    bool verbose = cimg_option("-v", false, "Verbose mode");
    bool fileStats = cimg_option("-f", false, "Write stats to file");
    bool saveResult = cimg_option("-s", false, "Save result to file");
    const char* inputFile = cimg_option("-if", "images/lenaGrayHiddenSmall.bmp", "Input file name");
    const char* outputFile = cimg_option("-of", "output.bmp", "Output file name");
    unsigned int nbIterations = cimg_option("-n", 5, "Number of iterations");
    unsigned int neighborhoodSize = cimg_option("-ns", 20, "For Codebook optimization define the neighborhood size to consider");

    CImg<float> input = CImg<float>(inputFile).channel(0);
    CImgDisplay displayInput(input, "Input Image");

    // Create algorithm
    AbstractAlgorithm* algo = new CodebookAlgorithm(input, neighborhoodSize, nbIterations, verbose, fileStats);
	
    // Algo
    algo->exec();

    // Results
    const CImg<>& result = algo->getResult();
    if (saveResult)
        result.save(outputFile);

    CImgDisplay displayFinalImage(result, "Result Image");
    while (!displayInput.is_closed() || !displayFinalImage.is_closed())
    {
        displayInput.wait();
    }

    delete algo;

    return EXIT_SUCCESS;
}
