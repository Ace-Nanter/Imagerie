#include "abstractalgorithm.h"

AbstractAlgorithm::AbstractAlgorithm(CImg<> input, unsigned int nbIteration, bool prematureStop, unsigned int windowSize, double gapPercentage, bool verbose, bool produceStats)
    : m_verbose(verbose)
    , m_fileStats(produceStats)
    , m_nbIterations(nbIteration)
    , m_enablePrematureStop(prematureStop)
    , m_movableWindowSize(windowSize)
    , m_gapPercentage(gapPercentage)
    , m_lastMedian(std::numeric_limits<double>::max())
    , m_lastEnergies()
    , m_image(input)
{

}

bool AbstractAlgorithm::computePrematureStop(double energy)
{
    bool ret = false;

    m_lastEnergies.push_back(energy);
    if (m_lastEnergies.size() >= m_movableWindowSize)
    {
        const unsigned int index = std::ceil(m_lastEnergies.size() / 2.0);
        std::vector<double> sortedEnergies(m_lastEnergies.begin(), m_lastEnergies.end());
        //std::sort(sortedEnergies.begin(), sortedEnergies.end());
        std::partial_sort(sortedEnergies.begin(), sortedEnergies.begin() + index, sortedEnergies.end());
        const double gap = m_gapPercentage * m_lastMedian;
        if (m_lastMedian - gap <= energy && m_lastMedian + gap >= energy)
        {
            ret = true;
        }

        m_lastMedian = sortedEnergies[index - 1];

        m_lastEnergies.pop_front();
    }

    return ret;
}
