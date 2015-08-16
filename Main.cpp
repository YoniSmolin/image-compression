/*
** Main.cpp - a simple program to load an image, compress + decompress it and show the results
*/

#include "Compressor.h"

#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <conio.h> // wait until any key is pressed

#define IMAGE_SIZE 512

using namespace cv;
using namespace std;

void distortImage(const uchar* original, uchar* distorted, int rows, int columns)
{
	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < columns; column++)
		{
			int index = row * columns + column;
			uchar difference = (column % 32);
			if (row % 2)
				distorted[index] = difference + original[index] > 255 ? 255 : original[index] + difference;
			else
				distorted[index] = difference > original[index] ? 0 : original[index] - difference;
		}
	}
}

int main()
{
	Mat image;
	image = imread("Beach.jpeg", CV_LOAD_IMAGE_GRAYSCALE);
	if (image.data == NULL)
	{
		cout << "Failed to load image" << endl;
		return 1;
	}
	
	uchar* distorted = new uchar[IMAGE_SIZE * IMAGE_SIZE];
	uchar* uncompressed = new uchar[IMAGE_SIZE*IMAGE_SIZE];
	distortImage(image.data, distorted, IMAGE_SIZE, IMAGE_SIZE);
	
	Compressor compressor(IMAGE_SIZE, IMAGE_SIZE);

	char* compressed = NULL;
	int compessedLength = compressor.CompressImage(distorted, image.data, compressed);
	compressor.DecompressImage(distorted, compressed, uncompressed);
	
	bool imagesAreEqual = true;
	for (int row = 0; row < IMAGE_SIZE; row++)
	{
		for (int column = 0; column < IMAGE_SIZE; column++)
		{
			int index = row * IMAGE_SIZE + column;
			imagesAreEqual = imagesAreEqual && (uncompressed[index] == image.data[index]);
			if (uncompressed[index] != image.data[index])
				cout << "orig: " << image.data[index] << ", uncompressed: " << uncompressed[index] << endl;
		}
	}

	cout << "Decompressed is " << (imagesAreEqual ? "" : "not ") << "the same as original" << endl;

	Mat uncompressedImage = Mat(IMAGE_SIZE, IMAGE_SIZE, CV_8UC1, uncompressed);
	imshow("uncompressed", uncompressedImage);
	waitKey(1);

	cout << "Press any key to continue" << endl;
	while (!_kbhit());
}