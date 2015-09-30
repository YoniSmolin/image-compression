/*
** Compressor.h - a header file for a file that performs compression of a gray scale
**				  image based on difference from previous image in a stream. This is
**				  only effective when the difference is small enough (less than 1/3 
**			      of the pixels have changed).
*/
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <exception>

typedef unsigned char uchar;

class CompressionError : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Overflow - found a pixel with difference greater than 32[cm]";
	}
};

class Compressor
{
public:
	Compressor(int rows, int columns);

	int CompressImage(const uchar* reference, const uchar* toBeCompressed, char* &compressed); // no need to allocate memory for toBeCompressed
	void DecompressImage(const uchar* reference, const char* compressed, uchar* decompressed); // appropriate memory allocation for decompressed assumed

	int CompressImagePNG(const uchar* toBeCompressed, uchar* &compressed);
	void DecompressImagePNG(const uchar* compressed, uchar* decompressed);

	~Compressor();

private:
	int _rows, _columns;
	char* _compressed;
};

#endif