#include <cstdlib>

#include <iostream>
#include <vector>

#include "CImg.h"

using namespace cimg_library;

/**
 * @brief Récupère l'ensemble des coordonnées des pixels faisant l'objet d'une reconstitution.
 * @param input Image d'entrée.
 * @return Liste des coordonnées des pixels de l'image.
 */
std::vector< std::pair< unsigned int, unsigned int > > getMask(const CImg<>& input);

int main(int argc, char** argv)
{
    CImg<float> input("images/lenaGrayHidden.bmp");
    CImgDisplay displayInput(input, "Image d'origine");

    std::vector< std::pair< unsigned int, unsigned int > > mask = getMask(input);

    std::cout << "Nombre de pixel à reconstituer : " << mask.size() << std::endl;
	
    while (!displayInput.is_closed())
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
