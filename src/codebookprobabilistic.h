#ifndef CODEBOOK_PROBABILISTIC_H
#define CODEBOOK_PROBABILISTIC_H

#include "abstractalgorithm.h"

#include <map>
#include <vector>

/**
* @brief The CodebookProbabilistic class Implements the probabilistic method using codebook optimization.
*/
class CodebookProbabilistic
	: public AbstractAlgorithm
{
public:
	// Data structure defines
	using Point = std::pair< unsigned int, unsigned int >;
	using PointSet = std::vector< Point >;
	using MapMask = std::map<Point, Point>;
	using MaskSet = std::map< Point, PointSet >;

private:
	unsigned int m_neighborhoodSize;    ///< Size of the neighborhood considered.

    MaskSet m_neighboorMask;///< Pixel that are in the mask.
    MapMask m_mappingMask;  ///< Association of the pixel which are in the mask with the replacing pixels.
    PointSet m_outMask;     ///< Pixel that are out the mask.

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
     * @param neighborhoodSize Size of ther neighborhood to consider around mask pixels.
     * @param nbIteration Number of iterations to perform.
     * @param prematureStop Flag for premature stop.
     * @param windowsSize Window size.
     * @param gapPercentage Gap percentage to use.
     * @param verbose Use verbose mode.
     * @param produceStats Algorithm will produce file for statistics.
     */
    CodebookProbabilistic(CImg<> input,
                          unsigned int neighborhoodSize,
                          unsigned int nbIteration = 5,
                          bool prematureStop = true,
                          unsigned int windowSize = 10,
                          double gapPercentage = 0.01,
                          bool verbose = false,
                          bool produceStats = false);

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

#endif // CODEBOOK_PROBABILISTIC_H
