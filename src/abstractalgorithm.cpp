#include "abstractalgorithm.h"

AbstractAlgorithm::AbstractAlgorithm(CImg<> input, unsigned int nbIteration, bool verbose, bool produceStats)
    : m_verbose(verbose)
    , m_fileStats(produceStats)
    , m_nbIterations(nbIteration)
    , m_image(input)
{

}
