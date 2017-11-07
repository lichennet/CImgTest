
//#define cimg_use_jpeg


#include "CImg.h"
using namespace cimg_library;

#include <iostream>
using namespace std;

#ifndef cimg_imagepath
#define cimg_imagepath "img/"
#endif


// otsu
template<typename T>
T graythresh(const CImg<T> &img)
{
  T thres;
  float w, u, variance, avg;
  w = u = variance = avg = 0;

  CImg<float> hist = img.get_histogram(256, 0, 255);
  hist /= img.size();

  cimg_forX(hist, x) {
    avg += x*hist(x);
  }
  cimg_forX(hist, x) {
    w += hist(x);
    u += x*hist(x);
    float t = avg*w - u;
    float v = t*t / (w*(1-w));
    if (v > variance) {
      variance = v;
      thres = x;
    }
  }

  return thres;
}


// Main procedure
//----------------
int main(int argc, char** argv) {

  // Usage of the program displayed at the command line
  cimg_usage("CImg test!");

#if 0
  const CImg<unsigned char> src("img/lena.bmp");
  CImg<unsigned char> dest(src.width(), src.height(), 1, 1);
  dest.fill(0);

  CImg<unsigned char> N(5,5);

//  cimg_for_in5x5(src, 3, 3, src.width()-4, src.height()-4, x,y,0,0, N, unsigned char)
  cimg_for5x5(src, x, y, 0, 0, N, unsigned char)
	  dest(x, y, 0) = N.sum()/(5.0*5.0) + 0.5;

  CImgList<unsigned char> visu (src, dest);
  visu.display("2 imgs");
#endif

#if 1
  const CImg<float> src("img/car3.jpg"), src_gray = src.get_norm().normalize(0, 255);
  CImg<float> dest(src.width(), src.height(), 1, 1);
  cimg_forXY(src_gray, x, y)
    dest(x, y) = src_gray(x, y);

#if 0
  CImg<unsigned char> N(5,5);
  cimg_for5x5(src, x, y, 0, 0, N, unsigned char)
    dest(x, y) = N.sum()/(5.0*5.0) + 0.5;
#else
  dest.blur(1.0);
#endif

  // sobel
  CImg<float> tmp(dest);
//  cimg_for_inXY(dest, 1, 1, src.width()-2, src.height()-2, x,y) {
  cimg_forXY(dest, x,y) {
    int temp;
    temp  = -1*dest(x-1,y-1) + 1*dest(x+1,y-1);
    temp += -2*dest(x-1,y)   + 2*dest(x+1,y);
    temp += -1*dest(x-1,y+1) + 1*dest(x+1,y+1);
    tmp(x,y)  = abs(temp);
  }
  dest = tmp.get_normalize(0,255);


  dest.threshold(graythresh(dest));


  (src, tmp, dest).display();
#endif

  return 0;
}

