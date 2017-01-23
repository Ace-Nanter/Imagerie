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
	* @brief Use the deterministic method to emplace mask pixels.
	*/
	void exec() override;
};

#endif // PROBABILISTICALGORITHM_H
