/*
**	Compressor.cpp - an implementation of simple compression of gray scale images based on an assumptio of small temporal differences.
*/

#include "Compressor.h"

#define BYTES_PER_PIXEL 3
#define BYTES_FOR_HEADER 3
#define BITS_IN_BYTE 8
#define BITS_DEDICATED_TO_DIFFERENCE 6

Compressor::Compressor(int rows, int columns) : _rows(rows), _columns(columns) 
{
	_compressed = new char[rows * columns * BYTES_PER_PIXEL + BYTES_FOR_HEADER]; // the additional int serves to store the length of the compressed image
}

Compressor::~Compressor() { delete[] _compressed; }

// returns the size of compressed image (including header)
int Compressor::CompressImage(const uchar* reference, const uchar* toBeCompressed, char* &compressed)
{
	int indexCompressed = BYTES_FOR_HEADER;

	// compress data
	for (int row = 0; row < _rows; row++)
	{
		for (int col = 0; col < _columns; col++)
		{
			int indexOriginal = row * _columns + col;
			char difference = toBeCompressed[indexOriginal] - reference[indexOriginal];

			if (difference != 0)
			{
				if (difference >> BITS_DEDICATED_TO_DIFFERENCE != 0 && difference >> BITS_DEDICATED_TO_DIFFERENCE != -1) // meaning - 6 bits are not enough to describe the difference
					throw new CompressionError;

				_compressed[indexCompressed++] = indexOriginal;      // store 8 LSBs 
				_compressed[indexCompressed++] = indexOriginal >> BITS_IN_BYTE; // store following (more significant) 8 LSBs
				_compressed[indexCompressed++] = (indexOriginal >> 2*BITS_IN_BYTE) + (difference << (BITS_IN_BYTE - BITS_DEDICATED_TO_DIFFERENCE)); // store final 2 bits of the indexOriginal and the 6 bits of difference
			}

		}
	}

	// prepare image header - contains the length of the compressed image
	int compressedLength = indexCompressed - BYTES_FOR_HEADER;
	_compressed[0] = compressedLength;
	_compressed[1] = compressedLength >> BITS_IN_BYTE;
	_compressed[2] = compressedLength >> 2*BITS_IN_BYTE;
	compressed = _compressed;
	return indexCompressed;
}

void Compressor::DecompressImage(const uchar* reference, const char* compressed, uchar* decompressed)
{
	// first step - copy 
	for (int row = 0; row < _rows; row++)
	{
		for (int col = 0; col < _columns; col++)
		{
			int index = row * _columns + col;
			decompressed[index] = reference[index];
		}
	}

	// reconstruct the length of the compressed image from the header
	int compressedLength = (uchar)compressed[0]; 
	compressedLength    += (uchar)compressed[1] << BITS_IN_BYTE;
	compressedLength    += (uchar)compressed[2] << 2*BITS_IN_BYTE;
	
	// reconstruct the image itself
	int indexCompressed = BYTES_FOR_HEADER;
	while (indexCompressed < compressedLength + BYTES_FOR_HEADER)
	{
		int index = (uchar)compressed[indexCompressed++]; // obtain 8 LSBs of index
		index    += (uchar)compressed[indexCompressed++] << BITS_IN_BYTE; // obtain next 8 LSBs
		index    += ((uchar)compressed[indexCompressed] & 3) << BITS_IN_BYTE*2; // obtain final 2 most significant bits of the index are the lower 2 bits of this byte
		decompressed[index] += (char)(compressed[indexCompressed] >> 2); // the upper 6 bits are the difference which needs to be added to the reference image in order to decompress
		indexCompressed++;
	}
}