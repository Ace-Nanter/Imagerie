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

/**
 * @brief Définit si l'énergie est minimale ou non. Définit l'arrêt du programme. 
 * @return True si l'énergie est minimale, sinon false.
 */
const bool isEmin();

/**
 * @brief Utilise la méthode déterministe pour remplacer les pixels du masque.
 * @param input Image donnée en entrée.
 * @param mask Masque présent sur l'image.
 * @return Image recomposée.
 */
const CImg<> methodDeter(const CImg<float> input, const std::vector < std::pair < unsigned int , unsigned int > > mask);

int main(int argc, char ** argv)
{
	// Variables
	std::vector < std::pair < unsigned int , unsigned int > > mask;
	CImg<float> input("images/lenaGrayHidden.bmp");

	// Affichage
	CImgDisplay displayInput(input, "Image d'origine");
	
	mask = getMask(input);

	while(isEmin()) {
		methodDeter(input, mask);
	}

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

const bool isEmin() {
	// TODO : A implémenter
	return true;
}

const CImg<> methodDeter(const CImg<float> input, const std::vector < std::pair < unsigned int , unsigned int > > mask) {
	CImg<float> output(input);

	return output;
}
