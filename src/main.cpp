#include <cstdlib>

#include "CImg.h"

using namespace cimg_library;

int main(int argc, char ** argv)
{
	CImg<unsigned char> image("images/test.bmp");
	
	CImgDisplay display(image, "Image");
	
	while (!display.is_closed())
	{
		display.wait();
	}
	
    return EXIT_SUCCESS;
}
