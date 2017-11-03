#include "CImg.h"
#include <iostream>

using namespace cimg_library;

typedef unsigned char BYTE;
typedef unsigned char byte;

#define BW
#define _return_256

//this func is for single channel as the name shows, and you can fix a B&W img by
//it as well
int gray_equalize_single_channel(CImg<unsigned char>& target_img, int channel=0)
{
	int gray_store[256];
	int gray_map[256];
	int temp(0);
	int area = target_img.size()/3;//all the operation is for RBG bitmap so...
	for (int i(0);i != 256;++i) gray_store[i] = gray_map[i]= 0;
	cimg_forXY(target_img, x, y)
	{
		++gray_store[*target_img.data(x, y, 0, channel)];
	}
	for (int i(0);i != 256;++i)
	{
		temp = 0;
		for (int j(0);j != i;++j)
			temp += gray_store[j];
		gray_map[i] = temp * 255 / area;
	}
	cimg_forXY(target_img, x, y)
	{
		target_img(x, y, 0, channel) = gray_map[*target_img.data(x, y, 0, channel)];
	}
	return 0;
}

int add_contract_single_channel
(CImg<unsigned char>& target_img, short parameter=1,short brightness_addend=0,short channel=0)
{//you can NOT use it in a B&W img
	/*think about it, auto_append+parameter*127 need to be 127 as well */
	short auto_addend = (parameter - 1) * 127;
	int temp(0);
	cimg_forXY(target_img, x, y)
	{
		temp = parameter*(*target_img.data(x, y, 0, channel)) 
			- auto_addend + brightness_addend;
		temp = temp > 255 ? 255 : temp;
		temp = temp < 0 ? 0 : temp;
		target_img(x,y,0,channel) = temp;
	}
	return 0;
}

int gray_binary(CImg<unsigned char>& target_img, BYTE threshold)
{
	//for black&white RBG img only
	cimg_forXY(target_img, x, y)
	{
		target_img(x,y,0) = *target_img(x,y,0) < threshold ? 0 : 255;
	}
	return 0;
}

/*****************************************************************************/

CImg<BYTE> get_translation(const CImg<BYTE>& target_img, int Tx, int Ty)
{
	CImg<BYTE> temp;
	temp.resize(target_img).fill(255);
	cimg_forXY(target_img, x, y)
	{
		if (x + Tx >= 0 && x + Tx<target_img.width()
			&& y + Ty >= 0 && y + Ty<target_img.height())
			temp(x + Tx, y + Ty, 0, 0) = *target_img.RED;
	}
	return CImg<BYTE>(temp);
}

#define ROUNDF(s) ((int)((s)+0.5))
int _zoom_with_nearest_neiberbour_interpolation (CImg<BYTE>& target_img, int parameter)
{//this function shouldn't be used.
	int _new_width(ROUNDF(target_img.width()*parameter)),
		_new_height(ROUNDF(target_img.height()*parameter));
	CImg<BYTE> temp(_new_width, _new_height, 1, 3);
	cimg_forXY(temp, x, y)
	{/*get the original point in the target_img
	   and copy from it directly*/
		int x_origin = ROUNDF(x/2),
			y_origin = ROUNDF(y/2);
		if (x_origin >= 0 && x_origin < target_img.width()
			&& y_origin >= 0 && y_origin < target_img.height()) {
			temp(x, y, 0, 0) = *target_img.data(x_origin, y_origin, 0, 0);
			temp(x, y, 0, 1) = *target_img.data(x_origin, y_origin, 0, 1);
			temp(x, y, 0, 2) = *target_img.data(x_origin, y_origin, 0, 2);
		}
		else/*else it makes me unhappy and I dont give a 
			shit so I would give a white*/
			BW(temp) = 255;
	}
	target_img = temp;
	return 0;
}

template<typename T>
T _get_bilinear_interpolation_val
(float Px,float Py,T Q11,T Q12, T Q21, T Q22)
{/*see what P&Q means by going to
 https://en.wikipedia.org/wiki/Bilinear_interpolation
 Thank wiki and what's more if you see what V1&V2 means
 go to JamesFreeman's ZhuanlanZhihu*/
	double V1 = (1 - Px)*Q11 + Px*Q21,
		V2 = (1 - Px)*Q12 + Px*Q22;
	double result = V1*(1 - Py) + Py*V2;
	return T(result);
}

CImg<BYTE> get_zoom(const CImg<BYTE>&target_img, int parameter)
{/*see what P&Q means by going to
	https://en.wikipedia.org/wiki/Bilinear_interpolation
	Thank wiki and what's more if you see what V1&V2 means
	go to JamesFreeman's ZhuanlanZhihu*/
	int _new_width(round(target_img.width()*parameter)),
		_new_height(round(target_img.height()*parameter));
	CImg<BYTE> temp(_new_width, _new_height, 1, 3);
	cimg_forXY(temp, x, y)
	{
		float Px = ((float)x) / parameter,
			Py = ((float)y) / parameter;
		if (Px >= 0 && Px < target_img.width()
			&& Py >= 0 && Py < target_img.height()) {

			BYTE Q11R = *target_img.data(Px, Py, 0, 0);
			BYTE Q11G = *target_img.data(Px, Py, 0, 1);
			BYTE Q11B = *target_img.data(Px, Py, 0, 2);

			BYTE Q12R = *target_img.data(Px, Py + 1, 0, 0);
			BYTE Q12G = *target_img.data(Px, Py + 1, 0, 1);
			BYTE Q12B = *target_img.data(Px, Py + 1, 0, 2);

			BYTE Q21R = *target_img.data(Px + 1, Py, 0, 0);
			BYTE Q21G = *target_img.data(Px + 1, Py, 0, 1);
			BYTE Q21B = *target_img.data(Px + 1, Py, 0, 2);

			BYTE Q22R = *target_img.data(Px + 1, Py + 1, 0, 0);
			BYTE Q22G = *target_img.data(Px + 1, Py + 1, 0, 1);
			BYTE Q22B = *target_img.data(Px + 1, Py + 1, 0, 2);

			float Py_decimal = Py - (int)Py,
				Px_decimal = Px - (int)Px;
			temp(x, y, 0, 0) = _get_bilinear_interpolation_val
				(Px_decimal, Py_decimal, Q11R, Q12R, Q21R, Q22R);
			temp(x, y, 0, 1) = _get_bilinear_interpolation_val
				(Px_decimal, Py_decimal, Q11G, Q12G, Q21G, Q22G);
			temp(x, y, 0, 2) = _get_bilinear_interpolation_val
				(Px_decimal, Py_decimal, Q11B, Q12B, Q21B, Q22B);
		}
		else
			BW(temp) = 255;
	}
	return temp;
}

/*****************************************************************************/

#if 0
int Robert_trans(CImg<byte>& target_img)
{
	cimg_forXY(target_img, x, y)
	{
		if (x < target_img.width() && y < target_img.height())
			BW(target_img) = std::round(std::sqrt(
				square(*target_img.data(x + 1, y, 0, 0) - *target_img.data(x, y, 0, 0)) +
				square(*target_img.data(x, y + 1, 0, 0) - *target_img.data(x, y, 0, 0))
				));
	}
	return 0;
}

template <size_t X, size_t Y>
struct trans_template {
public:
	float parameter;
	int prim_x, prim_y;
	float trans_matrix[X*Y];

public:
	trans_template(float para, int x, int y, std::initializer_list<int> data)
		:parameter(para), prim_x(x), prim_y(y)
	{
		std::copy(data.begin(), data.end(), trans_matrix);
	}
	size_t width()
	{
		return X;
	}
	size_t height()
	{
		return Y;
	}
	float at(int x, int y)
	{
		return trans_matrix[X*x + y];
	}
};

template <size_t trans_template_x, size_t trans_template_y>
int template_trans_single_channel
(CImg<byte> &target_img, trans_template<trans_template_x, trans_template_y> target_template, short channel = 0)
{
	CImg<byte> copy(target_img);
	/*it calcu the pixel the template cannot reach*/
	size_t x_unreachable = target_img.width() - (trans_template_x - target_template.prim_x) + 1,
		y_unreachable = target_img.height() - (trans_template_y - target_template.prim_y) + 1;
	std::cout << x_unreachable << " " << y_unreachable << std::endl;
	cimg_forXY(copy, x, y)
	{
		/*for the pixel the template can reach*/
		if (x > target_template.prim_x&&x<x_unreachable&&
			y>target_template.prim_y&&y < y_unreachable)
		{
			float sum_result(0);
			for (int i(0);i != trans_template_x;++i)
				for (int j(0);j != trans_template_y;++j)
				{
					/*calcu the sum of each template elem*the pixel*/
					sum_result +=
						target_template.at(i, j)*
						(*target_img.data(x - target_template.prim_x + i, y - target_template.prim_y + j, 0, channel));
				}
			copy(x, y, 0, channel) = _return_256(std::round(sum_result));
		}
		else/*just remain the same as I dont want to design a edge template for each template*/
			copy(x, y, 0, channel) = *target_img.data(x, y, 0, channel);
	}
	target_img = copy;
	return 0;
}

static trans_template<3, 3> Gauss_template()
{
	return trans_template <3, 3>(float(1)/flroundoat(16), 1, 1,
	{ 1,2,1,2,4,2,1,2,1 });
}

int Guass_trans(CImg<byte>& target_img)
{
	for (int i(0);i != 3;++i)
		template_trans_single_channel(target_img, Gauss_template(), i);
	return 0;
}

/*****************************************************************************/

//#include "geometry_calcu.h"

/*my idea is to divide the process into two small process:
  1.find the approximately find where is the num plate
  2.give a exact*/
void _Freeman_fuzzy_find(CImg<byte> &target_img, int step, int width)
{
	int THRESHOLD = target_img.height()*10;
	/*creat four edge for the recursion img we will cut*/
	size_t left_edge(0), right_edge(target_img.width()-1), up_edge(0), down_edge(target_img.height()-1);
	/*creat two int to store the last sum and the present sum of the gray 
	scale in the "width*target_img.height()" rectangle*/
	int last_sum(0), current_sum(0);
	/*creat a trigger, I.every time find a x-axis edge, ++trigger 
	II.trigger reach 2, enter the loop to find y-axis edge, find one, ++
	III.trigger reach 4 check the len/height, if true, ++*/
	short trigger(0);
	CImg<byte> copy(target_img);
	copy.blur(30);
	copy.display();
	gray_binary(copy,50);
	copy.display();
	for (int x(0); x < copy.width() ;x += step)
	{
		last_sum = current_sum;
		current_sum = 0;
		/*in the loop below we get the sum of the rectangle of its pixels*/
		for (int start(x);start != x + width;start++)
			for (int y(0);y < copy.height();++y)
				current_sum += *copy.data(start, y, 0, 0);
		std::cout << current_sum <<"   "<< last_sum << std::endl;
		if (current_sum - last_sum > THRESHOLD && trigger==0 && x>5)
		{
			left_edge = x - step;
			trigger++;
		}
		if (current_sum - last_sum < -THRESHOLD&&trigger==1)
		{
			right_edge = x + step;	
			trigger++;
		}
		/*if left edge and right edge is found, 
		then we enter the next step t find the up edge and the down edge*/
		if (trigger == 2)
		{
			int last_sum_y(0), current_sum_y(0);
			for (int y(0);y < copy.height();y += step)
			{
				last_sum_y = current_sum_y;
				current_sum_y = 0;
				/*in the loop_return_256 below we get the sum of the rectangle of its pixels*/
				for (int start(y);start < y + width;start++)
					for (int xx(left_edge);xx < right_edge;++xx)
						current_sum_y += *copy.data(xx, start, 0, 0);
				if (current_sum_y - last_sum_y > 2 * THRESHOLD && trigger<3)
				{
					up_edge = y - step;
					trigger++;
				}
				if (last_sum_y - current_sum_y > 2 * THRESHOLD && trigger<4)
				{
					down_edge = y + step;
					trigger++;
				}
				if (trigger == 4)
				{
					float len_wid_ratio = float(right_edge - left_edge) / float(down_edge - up_edge);
					if (len_wid_ratio > 1 && len_wid_ratio < 4)
						break;
				}
			}
		}
		if (trigger == 4)
			break;
	}
	img_cut(target_img, left_edge, up_edge, right_edge, down_edge);
	target_img.display();
}

#endif
