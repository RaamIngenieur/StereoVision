#include <iostream>
#include <png++/png.hpp>
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>

using namespace std;


class UCarray
{
public:
	unsigned char **Pix;
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
	Pix = (unsigned char**)malloc(row * sizeof(unsigned char*));
	for (int i = 0; i < row; i++)
	{
		Pix[i] = (unsigned char*) malloc(column * sizeof(unsigned char));
		if (Pix[i] == NULL)
		{
			fprintf(stderr, "out of memory\n");
		}
	}
}

UCarray::~UCarray()
{
	for(int i=0;i<row;i++)
	{
		free (Pix[i]);
	}
	free (Pix);
}

void UCarray::ImagetoArray(png::image<png::gray_pixel>* input)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			Pix[i][j] = (*input)[i][j];
		}
	}
}

void UCarray::ArraytoImage(png::image<png::gray_pixel>* output)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			(*output)[i][j] = Pix[i][j];
		}
	}
}

void UCarray:: operator =(const UCarray&  other)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			Pix[i][j] = other.Pix[i][j];
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
			min = Pix[i][j];
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					if (Pix[i+k][j+l] < min)
					{
						min = Pix[i+k][j+l];
					}
				}
			}
			b.Pix[i][j] = min;
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
			max = Pix[i][j];
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					if (Pix[i+k][j+l] > max)
					{
						max = Pix[i+k][j+l];
					}
				}
			}
			b.Pix[i][j] = max;
		}
	}
}

void InitializeULArray(unsigned long *** Pix, int row, int column)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				Pix[i][j][k] = 0;
			}

		}
	}
}

void Census11(unsigned long *** OPic, UCarray &IPic, int row, int column)
{
	int BitCnt;

	for (int i = 5; i <= row - 6; i++)
	{
		for (int j = 5; j <= column - 6; j++)
		{
			BitCnt = 0;

			for (int k = -5; k <= 5; k++)
			{
				for (int l = -5; l <= 5; l++)
				{
					if (~(k == 0 && l == 0))
					{
						OPic[i][j][BitCnt/64] = OPic[i][j][BitCnt/64] << 1;

						if (IPic.Pix[i+k][j+l] < IPic.Pix[i][j])
						{
							OPic[i][j][BitCnt/64] = OPic[i][j][BitCnt/64] + 1;
						}

						BitCnt++;

					}
				}
			}
		}
	}
}

void InitializeUIArray(unsigned int** Pix, int row, int column)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			Pix[i][j] = 0;
		}
	}
}

void ArrayUItoImage(png::image<png::gray_pixel>* input, unsigned int** Pix, int row, int column)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			(*input)[i][j] = (unsigned char)(Pix[i][j]);
		}
	}
}

void SHDRighttoLeft13(unsigned int** DMap, unsigned long  ***LPic, unsigned long  ***RPic, int row, int column)
{
	unsigned int HammDistCalc, HammDistMin = 28561;
	unsigned long Xor;
	int DMin=0, DMax=100;

	for (int i = 6; i < row - 6; i++)
	{
		for (int j = 6; j < column - 6; j++)
		{
			for (int DRange = DMax; DRange >= DMin; DRange--)
			{
				if(j + 6 + DRange < column)
				{
					HammDistCalc = 0;

					for (int k = -6; k <= 6; k++)
					{
						for (int l = -6; l <= 6; l++)
						{
							Xor = LPic[i+k][j+l+DRange][0] ^ RPic[i+k][j+l][0];
							HammDistCalc += __builtin_popcountl(Xor);
							Xor = LPic[i+k][j+l+DRange][1] ^ RPic[i+k][j+l][1];
							HammDistCalc += __builtin_popcountl(Xor);
						}
					}
				}

				if (HammDistCalc<HammDistMin)
				{
					HammDistMin=HammDistCalc;
					DMap[i][j] = DRange;
				}
			}

			HammDistMin = 28561;
		}
	}
}



void PreProcessImage(png::image<png::gray_pixel>* image, UCarray &UCInput, UCarray &UCOutput, unsigned long *** CTImage, int row, int column)
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

	UCarray  UCLInput(row, column), UCLOutput(row, column), UCRInput(row, column), UCROutput(row, column);
	unsigned long ***LCTImage, ***RCTImage;

	LCTImage = (unsigned long***)malloc(row * sizeof(unsigned long**));
	RCTImage = (unsigned long***)malloc(row * sizeof(unsigned long**));
	for (int i = 0; i < row; i++)
	{
		LCTImage[i] = (unsigned long**) malloc(column * sizeof(unsigned long*));
		RCTImage[i] = (unsigned long**) malloc(column * sizeof(unsigned long*));
		if (LCTImage[i] == NULL||RCTImage[i] == NULL)
		{
			fprintf(stderr, "out of memory\n");
			return 0;
		}
		for (int j = 0; j < column; j++)
		{
			LCTImage[i][j] = (unsigned long*) malloc(2 * sizeof(unsigned long));
			RCTImage[i][j] = (unsigned long*) malloc(2 * sizeof(unsigned long));
			if (LCTImage[i][j] == NULL||RCTImage[i][j] == NULL)
			{
				fprintf(stderr, "out of memory\n");
				return 0;
			}
		}
	}

	unsigned int** DMap;

	DMap = (unsigned int**)malloc(row * sizeof(unsigned int*));
	for (int i = 0; i < row; i++)
	{
		DMap[i] = (unsigned int*) malloc(column * sizeof(unsigned int));
		if (DMap[i] == NULL)
		{
			fprintf(stderr, "out of memory\n");
			return 0;
		}
	}


	gettimeofday(&tim, NULL);
	t5 = tim.tv_sec + (tim.tv_usec / 1000000.0);

	PreProcessImage(&imageL, UCLInput, UCLOutput, LCTImage, row,column);
	PreProcessImage(&imageR, UCRInput, UCROutput, RCTImage, row, column);

	gettimeofday(&tim, NULL);
	t6 = tim.tv_sec + (tim.tv_usec / 1000000.0) - t5;

	
	InitializeUIArray(DMap,row,column);

	SHDRighttoLeft13(DMap,LCTImage,RCTImage,row,column);

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

