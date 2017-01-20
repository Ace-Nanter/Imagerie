#ifndef CODEBOOKALGORITHM_H
#define CODEBOOKALGORITHM_H

#include "abstractalgorithm.h"

#include <map>
#include <vector>

/**
 * @brief The CodebookAlgorithm class Implements the deterministic method using codebook optimization.
 */
class CodebookAlgorithm
        : public AbstractAlgorithm
{
public:
    // Data structure defines
    using Point = std::pair< unsigned int, unsigned int >;
    using PointSet = std::vector< Point >;
    using MaskSet = std::map< Point, PointSet >;

private:
    unsigned int m_neighborhoodSize;    ///< Size of the neighborhood considered.

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
     * @param neighborhoodSize Size of ther neighborhood to consider around mask pixels.
     * @param nbIteration Number of iterations to perform.
     * @param verbose Use verbose mode.
     * @param produceStats Algorithm will produce file for statistics.
     */
    CodebookAlgorithm(CImg<> input, unsigned int neighborhoodSize, unsigned int nbIteration = 5, bool verbose = false, bool produceStats = false);

    /**
     * @brief Use the deterministic method with codebook optimization to emplace mask pixels.
     */
    void exec() override;

    /**
     * @brief Get the used neighborhood size.
     * @return Neighborhood size.
     */
    unsigned int neighborhoodSize() const
    {
        return m_neighborhoodSize;
    }
};

#endif // CODEBOOKALGORITHM_H
