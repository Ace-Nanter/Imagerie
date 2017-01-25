#ifndef PROBABILISTICALGORITHM_H
#define PROBABILISTICALGORITHM_H

#include "abstractalgorithm.h"

#include <vector>
#include <map>

/**
* @brief The DeterministicAlgorithm class Implements the deterministic method.
*/
class ProbabilisticAlgorithm
    : public AbstractAlgorithm
{
public:
    // Data structure defines
    using Point = std::pair< unsigned int, unsigned int >;
    using PointSet = std::vector< Point >;
    using MapMask = std::map<Point, Point>;
    using MaskSet = std::map< Point, PointSet >;

private:
    MapMask m_mappingMask;			///< Association of the pixel which are in the mask with the replacing pixels.
    MaskSet m_neighboorhoodMap;		///< Pixel that are in the mask with their neighboorhood.
    PointSet m_outMask;				///< Pixel that are out the mask.

    /**
    * @brief Recover all pixels coordinates that need reconstruction.
    */
    void computeMask();

    /**
    * @brief Initialize all pixel from mask to a random value from input image.
    */
    void randomInitMask();

    /**
    * @brief Help to compute the non causal part of the probabilty distance
    * @param value Value of the pixel analyzed
    * @param xA x coordinate of the pixel replaced
    * @param yA y coordinate of the pixel replaced
    * @param xB x coordinate of the neighboor chosen
    * @param yB y coordinate of the neighboor chosen
    * @return the distance between the pixels.
    */
    double distanceNonCausal(float value, int xA, int yA, int xB, int yB);

public:
    /**
    * @brief Constructor
    * @param input Image that will be treated.
    * @param nbIteration Number of iterations to perform.
    * @param verbose Use verbose mode.
    * @param produceStats Algorithm will produce file for statistics.
    */
    ProbabilisticAlgorithm(CImg<> input, unsigned int nbIteration = 5, bool verbose = false, bool produceStats = false);

    /**
    * @brief Use the probabilistic method to emplace mask pixels.
    */
    void exec() override;
};

#endif // PROBABILISTICALGORITHM_H
