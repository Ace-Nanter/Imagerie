#include <cstdlib>

#include <iostream>
#include <random>
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
 * @param input Image à traiter.
 */
void deterministicMethod(const std::vector< std::pair< unsigned int, unsigned int > >& mask, CImg<>& input);


/// MAIN ///
int main(int argc, char** argv)
{
    CImg<float> input("images/lenaGrayHiddenSmall.bmp");
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
    deterministicMethod(mask, input);

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
        // Pixels rouges
        if (input(x, y, 0) == 255 && input(x, y, 1) == 0 && input(x, y, 2) == 0)
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
        input(pixel.first, pixel.second, 0) = input(seedPixel.first, seedPixel.second, 0);
        input(pixel.first, pixel.second, 1) = input(seedPixel.first, seedPixel.second, 1);
        input(pixel.first, pixel.second, 2) = input(seedPixel.first, seedPixel.second, 2);
    }
}

#include <algorithm>
void deterministicMethod(const std::vector< std::pair< unsigned int, unsigned int > >& mask, CImg<>& input)
{
    // TODO check the minimization of E(F)

    for (const auto& pixel : mask)
    {
        cimg_forC(input, c)
        {
            std::pair<unsigned int, unsigned int> bestMatch(0, 0);
            double lowestDist = std::numeric_limits<double>::max();
            CImg_3x3(I, float);

            cimg_for3x3(input, x, y, 0, c, I, float)
            {
                /// BAD => O(N)
                auto pair = std::pair<unsigned int, unsigned int>(x, y);
                auto it = std::find(mask.cbegin(), mask.cend(), pair);
                if (it == mask.cend()) continue;
                ///

                const double diffIpp = input(pixel.first - 1, pixel.second - 1, c) - Ipp;
                const double diffIcp = input(pixel.first    , pixel.second - 1, c) - Icp;
                const double diffInp = input(pixel.first + 1, pixel.second - 1, c) - Inp;

                const double diffIpc = input(pixel.first - 1, pixel.second, c) - Ipc;
                //const double diffIcc = input(pixel.first    , pixel.second, c) - Icc;
                const double diffInc = input(pixel.first + 1, pixel.second, c) - Inc;

                const double diffIpn = input(pixel.first - 1, pixel.second + 1, c) - Ipn;
                const double diffIcn = input(pixel.first    , pixel.second + 1, c) - Icn;
                const double diffInn = input(pixel.first + 1, pixel.second + 1, c) - Inn;

                double neighborhoodDist = diffIpp*diffIpp + diffIcp*diffIcp + diffInp*diffInp
                                        + diffIpc*diffIpc /*+ diffIcc*diffIcc*/ + diffInc*diffInc
                                        + diffIpn*diffIpn + diffIcn*diffIcn + diffInn*diffInn;

                if (neighborhoodDist < lowestDist)
                {
                    lowestDist = neighborhoodDist;
                    bestMatch = { x, y };
                }
            }

            input(pixel.first, pixel.second,c) = input(bestMatch.first, bestMatch.second, c);
        }
    }
}
