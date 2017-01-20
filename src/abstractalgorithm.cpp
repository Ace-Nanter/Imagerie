#include "abstractalgorithm.h"

AbstractAlgorithm::AbstractAlgorithm(CImg<> input, unsigned int nbIteration, bool verbose)
    : m_verbose(verbose)
    , m_nbIterations(nbIteration)
    , m_image(input)
{

}
