#include <iostream>
#include <png++/png.hpp>
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>
#include <thread>

using namespace std;


class UCarray
{
public:
	unsigned char *Pix;
	UCarray(int r, int c);
	~UCarray();
	void ImagetoArray(png::image<png::gray_pixel>* input);
	void operator=(const UCarray& other);
	void ArraytoImage(png::image<png::gray_pixel>* output);
	void ArrayErode(UCarray &b);
	void ArrayDilate(UCarray &b);
private:
	int row, column;

};


UCarray::UCarray(int r, int c)
{
	row = r;
	column = c;
	Pix = (unsigned char*)malloc(row * column * sizeof(unsigned char));
	if (Pix == NULL)
	{
		fprintf(stderr, "out of memory\n");
	}

}

UCarray::~UCarray()
{
	free(Pix);
}

void UCarray::ImagetoArray(png::image<png::gray_pixel>* input)
{
	unsigned char *Pixx = Pix;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			*Pixx = (*input)[i][j];
			Pixx++;
		}
	}
}

void UCarray::ArraytoImage(png::image<png::gray_pixel>* output)
{
	unsigned char *Pixx = Pix;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			(*output)[i][j] = *Pixx;
			Pixx++;
		}
	}
}

void UCarray:: operator =(const UCarray&  other)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			*(Pix+i*column+j) = *(other.Pix+i*column+j);
		}
	}
}

void UCarray::ArrayErode(UCarray &b)
{
	unsigned char min;
	for (int i = 1; i <= row - 2; i++)
	{
		for (int j = 1; j <= column - 2; j++)
		{
			min = *(Pix+i*column+j);
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					if (*(Pix+(i + k)*column+j + l) < min)
					{
						min = *(Pix + (i + k)*column + j + l);
					}
				}
			}
			*(b.Pix+i*column+j) = min;
		}
	}
}

void UCarray::ArrayDilate(UCarray &b)
{
	unsigned char max;
	for (int i = 1; i <= row - 2; i++)
	{
		for (int j = 1; j <= column - 2; j++)
		{
			max = *(Pix + i*column + j);
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					if (*(Pix + (i + k)*column + j + l) > max)
					{
						max = *(Pix + (i + k)*column + j + l);
					}
				}
			}
			*(b.Pix + i*column + j) = max;
		}
	}
}

void InitializeULArray(unsigned long * Pix, int row, int column)
{
	unsigned long *Pixx = Pix;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				*Pixx = 0;
				Pixx++;
			}

		}
	}
}

void Census11(unsigned long * OPic, UCarray &IPic, int row, int column)
{
	int BitCnt;
	unsigned long * OPicc;
	unsigned char * IPic1, *IPic2;

	OPicc=OPic + 10 * (column +1) - 1;
	IPic1=IPic.Pix+5*(column+1);

	for (int i = 5; i <= row - 6; i++)
	{
		for (int j = 5; j <= column - 6; j++)
		{
			BitCnt = 0;
			IPic2=IPic1-5*(column+1);

			for (int k = -5; k <= 5; k++)
			{
				for (int l = -5; l <= 5; l++)
				{
					if (~(k == 0 && l == 0))
					{
						if (BitCnt%64==0)
						OPicc = OPicc + 1;

						*OPicc = *OPicc << 1;

						if (*IPic2 < *IPic1)
						{
							*OPicc = *OPicc + 1;
						}

						BitCnt++;

					}
					IPic2++;
				}
				IPic2=IPic2+column-11;
			}
			IPic1++;
		}
		IPic1 = IPic1 + 10;
		OPicc = OPicc + 20;
	}
}

void InitializeUIArray(unsigned int* Pix, int row, int column)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			*(Pix+i*column+j) = 0;
		}
	}
}

void ArrayUItoImage(png::image<png::gray_pixel>* input, unsigned int* Pix, int row, int column)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			(*input)[i][j] = (unsigned char)(*(Pix+i*column+j));
		}
	}
}

void SHDRighttoLeft13(unsigned int* DMap, unsigned long  *LPic, unsigned long  *RPic, int row, int column)
{
	unsigned int HammDistCalc, HammDistMin = 28561,*DMap1;
	unsigned long Xor, *LPic1,*RPic1;
	int DMin=0, DMax=100;

	DMap1=DMap+6*(column+1);


	for (int i = 6; i < row - 6; i++)
	{
		for (int j = 6; j < column - 6; j++)
		{
			for (int DRange = DMax; DRange >= DMin; DRange--)
			{
				if(j + 6 + DRange < column)
				{
					LPic1=LPic+((i-6)*column+j-6+DRange)*2;
					RPic1=RPic+((i-6)*column+j-6)*2;
					
					HammDistCalc = 0;

					for (int k = 13; k > 0; k--)
					{
						for (int l = 26; l > 0; l--)
						{
							Xor = *LPic1++ ^ *RPic1++;
							HammDistCalc += __builtin_popcountl(Xor);
						}
						LPic1=LPic1+2*(column-13);
						RPic1=RPic1+2*(column-13);
					}
				}

				if (HammDistCalc<HammDistMin)
				{
					HammDistMin=HammDistCalc;
					*DMap1 = DRange;
				}
			}
			DMap1++;

			HammDistMin = 28561;
		}
		DMap1=DMap1+12;
	}
}



void PreProcessImage(png::image<png::gray_pixel>* image, UCarray &UCInput, UCarray &UCOutput, unsigned long * CTImage, int row, int column)
{
	UCInput.ImagetoArray(image);
	UCInput.ArrayErode(UCOutput);
	UCInput = UCOutput;
	UCInput.ArrayDilate(UCOutput);
	InitializeULArray(CTImage, row, column);
    Census11(CTImage,UCOutput,row,column);
}

int main()
{
	struct timeval tim;
	double t1, t2, t3, t4, t5, t6;
	gettimeofday(&tim, NULL);
	t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);

	png::image< png::gray_pixel > imageL("frame_1_left.png");
	png::image< png::gray_pixel > imageR("frame_1_right.png");

	gettimeofday(&tim, NULL);
	t3 = tim.tv_sec + (tim.tv_usec / 1000000.0);

	int row = imageL.get_height(), column = imageL.get_width();

	UCarray UCLInput(row, column), UCLOutput(row, column), UCRInput(row, column), UCROutput(row, column);
	unsigned long *LCTImage, *RCTImage;

	LCTImage = (unsigned long*)malloc(row * column* 2 * sizeof(unsigned long ));
	RCTImage = (unsigned long*)malloc(row * column* 2 * sizeof(unsigned long));
	if (LCTImage == NULL||RCTImage == NULL)
	{
		fprintf(stderr, "out of memory\n");
	}

	unsigned int* DMap;

	DMap = (unsigned int*)malloc(row * column * sizeof(unsigned int));
	if (DMap == NULL)
	{
		fprintf(stderr, "out of memory\n");
	}

	gettimeofday(&tim, NULL);
	t5 = tim.tv_sec + (tim.tv_usec / 1000000.0);

	PreProcessImage(&imageL, UCLInput, UCLOutput, LCTImage, row,column);
	PreProcessImage(&imageR, UCRInput, UCROutput, RCTImage, row, column);

	gettimeofday(&tim, NULL);
	t6 = tim.tv_sec + (tim.tv_usec / 1000000.0) - t5;

	InitializeUIArray(DMap,row,column);

	std::thread a(SHDRighttoLeft13, DMap,								LCTImage, 								RCTImage, 								row / 4 + 6,	column);
	std::thread b(SHDRighttoLeft13, DMap + (row / 4 - 6) * column, 		LCTImage + (row / 2 - 12) * column, 	RCTImage + (row / 2 - 12) * column, 	row / 4 + 12, 	column);
	std::thread c(SHDRighttoLeft13, DMap + (row / 2 - 6) * column, 		LCTImage + (row - 12) * column, 		RCTImage + (row - 12)* column, 			row / 4 + 12, 	column);
	std::thread d(SHDRighttoLeft13, DMap + (row * 3 / 4 - 6) * column, 	LCTImage + (row * 3 / 2 - 12) * column, RCTImage + (row *3 / 2 - 12) * column, 	row / 4, 		column);

	a.join();
	b.join();
	c.join();
	d.join();

	ArrayUItoImage(&imageL,DMap,row,column);

	gettimeofday(&tim, NULL);
	t4 = tim.tv_sec + (tim.tv_usec / 1000000.0);
	t4 = t4-t3;

	imageL.write("output.png");

	gettimeofday(&tim, NULL);
	t2 = tim.tv_sec + (tim.tv_usec / 1000000.0);
    t2= t2 - t1;

	cout<<endl<<"Total program run time : "<<t2<<endl;
	cout<<endl<<"Total processing time : "<<t4<<endl;
	cout<<endl<<"Total preprocessing time : "<<t6<<endl;

}
/* speed.cpp
 *
 *  Created on: 12 Oct 2015
 *      Author: raam
 */

