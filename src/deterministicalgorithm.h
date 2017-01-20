#ifndef DETERMINISTICALGORITHM_H
#define DETERMINISTICALGORITHM_H

#include "abstractalgorithm.h"

#include <vector>

/**
 * @brief The DeterministicAlgorithm class Implements the deterministic method.
 */
class DeterministicAlgorithm
        : public AbstractAlgorithm
{
public:
    // Data structure defines
    using Point = std::pair< unsigned int, unsigned int >;
    using PointSet = std::vector< Point >;
    using MaskSet = PointSet;

private:
    MaskSet m_mask;     ///< Pixel that are in the mask.
    PointSet m_outMask; ///< Pixel that are out the mask.

    /**
     * @brief Recover all pixels coordinates that need reconstruction.
     */
    void computeMask();

    /**
     * @brief Initialize all pixel from mask to a random value from input image.
     */
    void randomInitMask();

public:
    /**
     * @brief Constructor
     * @param input Image that will be treated.
     * @param nbIteration Number of iterations to perform.
     * @param verbose Use verbose mode.
     * @param produceStats Algorithm will produce file for statistics.
     */
    DeterministicAlgorithm(CImg<> input, unsigned int nbIteration = 5, bool verbose = false, bool produceStats = false);

    /**
     * @brief Use the deterministic method to emplace mask pixels.
     */
    void exec() override;
};

#endif // DETERMINISTICALGORITHM_H
