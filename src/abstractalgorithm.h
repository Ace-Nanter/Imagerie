#ifndef ABSTRACTALGORITHM_H
#define ABSTRACTALGORITHM_H

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

    CImg<> m_image;     ///< Image.

public:
    /**
     * @brief Constructor
     * @param input Image that will be treated.
     * @param nbIteration Number of iterations to perform.
     * @param verbose Use verbose mode.
     * @param produceStats Algorithm will produce file for statistics.
     */
    AbstractAlgorithm(CImg<> input, unsigned int nbIteration = 5, bool verbose = false, bool produceStats = false);

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
};

#endif // ABSTRACTALGORITHM_H
