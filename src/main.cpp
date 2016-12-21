#include <cstdlib>

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "CImg.h"

using namespace cimg_library;

std::mt19937 mt(123456789); ///< Générateur aléatoire

/**
 * @brief Récupère l'ensemble des coordonnées des pixels faisant l'objet d'une reconstitution.
 * @param input Image d'entrée.
 * @param mask Liste des coordonnées de l'image correspondant au mask.
 * @param outMask Liste des coordonnées de l'image correspondant à l'extérieur du mask.
 */
void getMask(const CImg<>& input,
             std::vector< std::pair< unsigned int, unsigned int > >& mask,
             std::vector< std::pair< unsigned int, unsigned int > >& outMask);

/**
 * @brief Initilialise les pixels du faisant partis du mask à une valeur aléatoire tirée depuis l'image d'entrée.
 * @param mask Vecteur des pixels du mask.
 * @param outMask Vecteur des pixels en dehors du mask.
 * @param input Image d'entrée.
 */
void randomInitMask(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                    const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                    CImg<>& input);

/**
 * @brief Utilise la méthode déterministe pour remplacer les pixels du masque.
 * @param mask Masque présent sur l'image.
 * @param image Image à traiter.
 */
void deterministicMethod(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                         const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                         CImg<>& image);

// Verbose mode
bool verbose;
bool fileStats;

// Save result
bool saveResult;

// Number of iterations
unsigned int nbIterations;

/// MAIN ///
int main(int argc, char** argv)
{
    verbose = cimg_option("-v", true, "Verbose mode");
    fileStats = cimg_option("-f", false, "Write stats to file");
    saveResult = cimg_option("-s", false, "Save result");
    nbIterations = cimg_option("-n", 5, "Number of iterations");

    CImg<float> input = CImg<float>("images/lenaGrayHiddenSmall.bmp").channel(0);
    CImgDisplay displayInput(input, "Image d'origine");

    // Pixels qui font objet du traitement
    std::vector< std::pair< unsigned int, unsigned int > > mask;
    // Autres pixels
    std::vector< std::pair< unsigned int, unsigned int > > outMask;
    getMask(input, mask, outMask);

    std::cout << "Nombre de pixel à reconstituer : " << mask.size() << std::endl;

    // Initialisation random
    CImg<float> finalImage(input);
    randomInitMask(mask, outMask, finalImage);
	
    // Algo
    deterministicMethod(mask, outMask, finalImage);

    if (saveResult)
        finalImage.save("output.bmp");

    CImgDisplay displayFinalImage(finalImage, "Image resultat");
    while (!displayInput.is_closed() || !displayFinalImage.is_closed())
    {
        displayInput.wait();
    }

    return EXIT_SUCCESS;
}


void getMask(const CImg<>& input,
             std::vector< std::pair< unsigned int, unsigned int > >& mask,
             std::vector< std::pair< unsigned int, unsigned int > >& outMask)
{
    // Ajoute tous les pixels de l'image qui doivent être reconstitués
    cimg_forXY(input, x, y)
    {
        // Pixels blanc pure
        if (input(x, y/*, 0*/) == 255 /*&& input(x, y, 1) == 255 && input(x, y, 2) == 255*/)
            mask.push_back({x, y});
        else
            outMask.push_back({x, y});
    }
}

void randomInitMask(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                    const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                    CImg<>& input)
{
    const unsigned int nbPixels = outMask.size();
    // Pour chaque pixel du masque
    for (const auto& pixel : mask)
    {
        unsigned int index = mt() % (nbPixels);
        const auto& seedPixel = outMask[index];

        // Initialise la couleur du pixel avec un pixel aléatoire de l'image
        input(pixel.first, pixel.second/*, 0*/) = input(seedPixel.first, seedPixel.second/*, 0*/);
        /*input(pixel.first, pixel.second, 1) = input(seedPixel.first, seedPixel.second, 1);
        input(pixel.first, pixel.second, 2) = input(seedPixel.first, seedPixel.second, 2);*/
    }
}

void deterministicMethod(const std::vector< std::pair< unsigned int, unsigned int > >& mask,
                         const std::vector< std::pair< unsigned int, unsigned int > >& outMask,
                         CImg<>& image)
{
    double lastEnergy = std::numeric_limits<double>::max();

    for (unsigned int i = 0 ; i < nbIterations ; ++i)
    {
        double energy = 0;

        for (const auto& pixel : mask)
        {
            std::pair<unsigned int, unsigned int> bestMatch(0, 0);
            double lowestDist = std::numeric_limits<double>::max();
            CImg_3x3(I, float);

            unsigned int i = 0;
            cimg_for3x3(image, x, y, 0, 0, I, float)
            {
                auto seedPixelCoord = std::pair<unsigned int, unsigned int>(x, y);
                // Pixel is outside mask => skip it
                if (outMask[i] != seedPixelCoord) continue; else ++i;

                // Treatments
                const double diffIpp = image(pixel.first - 1, pixel.second - 1) - Ipp;
                const double diffIcp = image(pixel.first    , pixel.second - 1) - Icp;
                const double diffInp = image(pixel.first + 1, pixel.second - 1) - Inp;

                const double diffIpc = image(pixel.first - 1, pixel.second) - Ipc;
                //const double diffIcc = image(pixel.first    , pixel.second) - Icc;    // Current pixel not in neighbohood
                const double diffInc = image(pixel.first + 1, pixel.second) - Inc;

                const double diffIpn = image(pixel.first - 1, pixel.second + 1) - Ipn;
                const double diffIcn = image(pixel.first    , pixel.second + 1) - Icn;
                const double diffInn = image(pixel.first + 1, pixel.second + 1) - Inn;

                double neighborhoodDist = diffIpp*diffIpp + diffIcp*diffIcp + diffInp*diffInp
                                        + diffIpc*diffIpc /*+ diffIcc*diffIcc*/ + diffInc*diffInc
                                        + diffIpn*diffIpn + diffIcn*diffIcn + diffInn*diffInn;

                if (neighborhoodDist < lowestDist)
                {
                    lowestDist = neighborhoodDist;
                    bestMatch = { x, y };
                }
            }

            energy += lowestDist;

            image(pixel.first, pixel.second) = image(bestMatch.first, bestMatch.second);
        }

        // Itération results
        double ratio = (lastEnergy - energy) / double(lastEnergy);
        ratio = ratio > 0 ? ratio : -ratio;

        // Write to file
        if (verbose)
        {
            if (fileStats)
            {
                std::stringstream ss;
                ss << "./loop" << i;
                std::ofstream ofs(ss.str(), std::ios::trunc | std::ios::out);
                ofs << "Last Energy : " << lastEnergy << "\nEnergy : " << energy << "\nRatio : " << ratio << "\n\n";
                ofs.close();
            }

            std::cout << "Last Energy : " << lastEnergy << "\nEnergy : " << energy << "\nRatio : " << ratio << "\n" << std::endl;
        }

        lastEnergy = energy;
    }
}
