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
		return "Encountered a difference larger than 32 [cm] in absolute value.";
	}
};

class Compressor
{
public:
	Compressor(int rows, int columns);

	int CompressMatrix(const uchar* reference, const uchar* toCompress, char* &compressedDifference);
	void DecompressMatrix(const uchar* reference, const char* compressedDifference, uchar* decompressed);

	~Compressor();
private:
	int _rows, _columns;
	char* _compressed;
};

#endif