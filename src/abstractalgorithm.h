#ifndef ABSTRACTALGORITHM_H
#define ABSTRACTALGORITHM_H

#include <deque>

#include "CImg.h"

using namespace cimg_library;

/**
 * @brief The AbstractAlgorithm class Represents an algorithm that can be executed on a an image.
 */
class AbstractAlgorithm
{
protected:
    bool m_verbose;     ///< Verbose mode.
    bool m_fileStats;   ///< Flag that indicate if we generate a statistic file for each iteration.
    unsigned int m_nbIterations;    ///< Number of iterations to perform.

    bool m_enablePrematureStop;     ///< Flag that use a movable windows of last energies to stop the algorithm prematuraly.
    unsigned int m_movableWindowSize;   ///< Number of energy considered when using premature stop.
    double m_gapPercentage;         ///< Gap percentage between last energy computed and median that is used to premautraly stop the algorithm.
    double m_lastMedian;            ///< Last iteration median.
    std::deque<double> m_lastEnergies;  ///< Store nbStoredEnergies elements corresponding to last iterations energies.

    CImg<> m_image;     ///< Image.

    /**
     * @brief Check if the algorithm should end prematuraly.
     * @param energy Last energy computed.
     * @return True if algorithm should stop.
     */
    bool computePrematureStop(double energy);

public:
    /**
     * @brief Constructor
     * @param input Image that will be treated.
     * @param nbIteration Number of iterations to perform.
     * @param prematureStop Flag for premature stop.
     * @param windowsSize Window size.
     * @param gapPercentage Gap percentage to use.
     * @param verbose Use verbose mode.
     * @param produceStats Algorithm will produce file for statistics.
     */
    AbstractAlgorithm(CImg<> input,
                      unsigned int nbIteration = 5,
                      bool prematureStop = true,
                      unsigned int windowSize = 10,
                      double gapPercentage = 0.01,
                      bool verbose = false,
                      bool produceStats = false);

    /**
     * @brief Destructor.
     */
    virtual ~AbstractAlgorithm() = default;

    /**
     * @brief Execute the implmented algorithm.
     */
    virtual void exec() =0;

    /**
     * @brief Get the resulting image after an execution of exec, otherwise input image.
     * @return CImg image.
     */
    const CImg<> getResult() const
    {
        return m_image;
    }

    /**
     * @brief Check if we are in verbose mode.
     * @return True if activated, otherwise false.
     */
    bool isVerbose() const
    {
        return m_verbose;
    }

    /**
     * @brief Set the verbose state.
     * @param verbose Verbose flag.
     */
    void setVerbose(bool verbose)
    {
        m_verbose = verbose;
    }

    /**
     * @brief Check if the algorithm should produce statistics.
     * @return True if activated, otherwise false.
     */
    bool produceStats() const
    {
        return m_fileStats;
    }

    /**
     * @brief Set the system to produce statistic at each iteration.
     * @param stats Statistic flag.
     */
    void setProduceStats(bool stats)
    {
        m_fileStats = stats;
    }

    /**
     * @brief Get the number of iterations to perform when using exec.
     * @return Number of iterations.
     */
    unsigned int getNbIterations() const
    {
        return m_nbIterations;
    }

    /**
     * @brief Set the number of iterations to perform.
     * @param nbIterations Number of iterations.
     */
    void setNbIterations(unsigned int nbIterations)
    {
        m_nbIterations = nbIterations;
    }

    /**
     * @brief Check if we use premature stop mechanism.
     * @return True if activated, otherwise false.
     */
    bool usePrematureStop() const
    {
        return m_enablePrematureStop;
    }

    /**
     * @brief Set the premature stop state.
     * @param state Premature stop flag.
     */
    void setUsePrematureStop(bool state)
    {
        m_enablePrematureStop = state;
    }


    /**
     * @brief Get the gap percentage.
     * @return Gap percentage.
     */
    unsigned int getGapPercentage() const
    {
        return m_gapPercentage;
    }

    /**
     * @brief Set the gap percentage.
     * @param gap Gap percentage.
     */
    void setGapPercentage(unsigned int gap)
    {
        m_gapPercentage = gap;
    }
};

#endif // ABSTRACTALGORITHM_H
