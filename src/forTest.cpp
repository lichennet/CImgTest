
//#define cimg_use_jpeg


#include "CImg.h"
using namespace cimg_library;

#include <iostream>

#ifndef cimg_imagepath
#define cimg_imagepath "img/"
#endif

// Main procedure
//----------------
int main(int argc, char** argv) {

  // Usage of the program displayed at the command line
  cimg_usage("CImg test!");

#if 0
  // Construct a new image called 'edge' of size (2*factor,2*factor)
  // and of type 'unsigned char'.
  CImg<unsigned char> img(256, 256);
  cimg_for_insideXY(img, x, y, 50) img(x, y) = x+y;
  cimg_for_borderXY(img, x, y, 50) img(x, y) = x-y;
  img.display();
  while(1);
#endif

#if 0
  CImg<unsigned char> N(5,5);
#if 0
  CImg<unsigned char> src = CImg<float>::get_load("img/parrot.ppm").norm().normalize(0,255), dest(src, false);

  cimg_for3x3(src, x, y, 0, 0, N, unsigned char)
	  dest(x, y, 0) = N.sum()/(3.0*3.0) + 0.5;

#else
  CImg<unsigned char> src("img/parrot.ppm"), dest(src, false);

  cimg_forC(src, k)
	cimg_for3x3(src, x, y, 0, k, N, unsigned char)
	  dest(x, y, k) = N.sum()/(3.0*3.0) + 0.5;
#endif
  CImgList<unsigned char> visu (src, dest);
  visu.display("2 imgs");
  while(1);
#endif


#if 0
  CImg<unsigned char> src = CImg<float>::get_load("img/car.jpg").norm().normalize(0,255);
  src.display();
#else
  const CImg<float> img("img/car.jpg"), res = img.get_equalize(256);
  CImg<float> tmpimg = res;
  CImgDisplay disp(tmpimg, "result", 0);

  while (!disp.is_closed()) {

      float kr, kg, kb;
      kr = 0.8022;
      kg = 2.8346;
      kb = -1.0869;
      {
        cimg_forXY(res, x, y) {
          float r, g, b;
          float tmp;

          r = res(x, y, 0);
          g = res(x, y, 1);
          b = res(x, y, 2);

          tmp = kb*b + r*kr + g*kg;
          tmpimg(x, y, 0) = tmp;
          tmpimg(x, y, 1) = tmp;
          tmpimg(x, y, 2) = tmp;
        }


        tmpimg.display(disp);
        // Temporize event loop
        cimg::wait(70);
      }
  }

  std::cout << "end loop" << std::endl;

  (img,res).display();
#endif

  while (1);
//  CImg<unsigned char> src("img/car.jpg");


  return 0;
}


// (*) Comments to the vocabulary used:
// If (s,t) is the current pixel, and G=(Gs,Gt) is the gradient at (s,t),
// then the _direction_pixel_ of (s,t) shall be the one of the eight neighbour pixels
// of (s,t) in whose direction the gradient G shows.
// The _contra_direction_pixel is the pixel in the opposite direction in which the gradient G shows.
// The _corresponding_gradient_value_ of the pixel (x,y) with gradient G = (Gx,Gy)
// shall be |Gx| + |Gy| ~=~ sqrt(Gx^2 + Gy^2).
