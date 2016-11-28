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
 * @return Liste des coordonnées des pixels de l'image.
 */
std::vector< std::pair< unsigned int, unsigned int > > getMask(const CImg<>& input);

/**
 * @brief Initilialise les pixels du faisant partis du mask à une valeur aléatoire tirée depuis l'image d'entrée.
 * @param mask Vecteur des pixels du mask.
 * @param input Image d'entrée.
 */
void randomInitMask(const std::vector< std::pair< unsigned int, unsigned int > >& mask, CImg<>& input);

/**
 * @brief Définit si l'énergie est minimale ou non. Définit l'arrêt du programme.
 * @return True si l'énergie est minimale, sinon false.
 */
bool isEmin();

/**
 * @brief Utilise la méthode déterministe pour remplacer les pixels du masque.
 * @param input Image donnée en entrée.
 * @param mask Masque présent sur l'image.
 * @return Image recomposée.
 */
CImg<> methodDeter(const CImg<float> input, const std::vector < std::pair < unsigned int , unsigned int > > mask);


/// MAIN ///
int main(int argc, char** argv)
{
    CImg<float> input("images/lenaGrayHidden.bmp");
    CImgDisplay displayInput(input, "Image d'origine");

    // Pixel qui font objet du traitement
    std::vector< std::pair< unsigned int, unsigned int > > mask = getMask(input);

    std::cout << "Nombre de pixel à reconstituer : " << mask.size() << std::endl;

    // Initialisation random
    CImg<float> finalImage(input);
    randomInitMask(mask, finalImage);
	
    // Algo
    while(isEmin())
    {
        methodDeter(input, mask);
    }

    CImgDisplay displayFinalImage(finalImage, "Image resultat");
    while (!displayInput.is_closed() || !displayFinalImage.is_closed())
    {
        displayInput.wait();
    }

    return EXIT_SUCCESS;
}


std::vector< std::pair< unsigned int, unsigned int > > getMask(const CImg<>& input)
{
    std::vector< std::pair< unsigned int, unsigned int > > mask;

    // Ajoute tous les pixels de l'image qui doivent être reconstitués
    cimg_forXY(input, x, y)
    {
        // Pixels rouges
        if (input(x, y, 0) == 255 && input(x, y, 1) == 0 && input(x, y, 2) == 0)
            mask.push_back({x, y});
    }

    return mask;
}

void randomInitMask(const std::vector< std::pair< unsigned int, unsigned int > >& mask, CImg<>& input)
{
    // Pour chaque pixel du masque
    for (const auto& pixel : mask)
    {
        unsigned int x = mt() % (input.width());
        unsigned int y = mt() % (input.height());

        // Initialise la couleur du pixel avec un pixel aléatoire de l'image
        input(pixel.first, pixel.second, 0) = input(x, y, 0);
        input(pixel.first, pixel.second, 1) = input(x, y, 1);
        input(pixel.first, pixel.second, 2) = input(x, y, 2);
    }
}

bool isEmin()
{
	// TODO : A implémenter

    return false;
}

CImg<> methodDeter(const CImg<float> input, const std::vector < std::pair < unsigned int , unsigned int > > mask)
{
	CImg<float> output(input);

    // TODO

    return output;
}
