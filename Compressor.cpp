/*
**	Compressor.cpp - an implementation of simple compression of gray scale images based on small temporal difference.
*/

#include "Compressor.h"

#define COMPRESSED_LENGTH_BYTES 3

Compressor::Compressor(int rows, int columns) : _rows(rows), _columns(columns) 
{
	_compressed = new char[rows * columns * COMPRESSED_LENGTH_BYTES + sizeof(int) / sizeof(char)];
}

Compressor::~Compressor() { delete[] _compressed; }

int Compressor::CompressMatrix(const uchar* reference, const uchar* modified, char* &compressedDifference)
{
	int indexCompressed = COMPRESSED_LENGTH_BYTES;
	for (int row = 0; row < _rows; row++)
	{
		for (int col = 0; col < _columns; col++)
		{
			int indexOriginal = row * _columns + col;
			char difference = modified[indexOriginal] - reference[indexOriginal];
			if (difference != 0)
			{
				if (difference >> 6 != 0 && difference >> 6 != -1)
					throw new CompressionError;

				_compressed[indexCompressed++] = indexOriginal; // only lower 8 bits are saved
				_compressed[indexCompressed++] = indexOriginal >> 8; // next 8 bits are saved
				_compressed[indexCompressed++] = (indexOriginal >> 16) + (int)(difference << 2); // final 2 bits of the indexOriginal and the whole difference
			}
		}
	}
	int compressedLength = indexCompressed - COMPRESSED_LENGTH_BYTES;
	_compressed[0] = compressedLength;
	_compressed[1] = compressedLength >> 8;
	_compressed[2] = compressedLength >> 16;
	compressedDifference = _compressed;
	return indexCompressed;
}

void Compressor::DecompressMatrix(const uchar* reference, const char* compressedDifference, uchar* decompressed)
{
	for (int row = 0; row < _rows; row++)
	{
		for (int col = 0; col < _columns; col++)
		{
			int indexOriginal = row * _columns + col;
			decompressed[indexOriginal] = reference[indexOriginal];
		}
	}


	int indexCompressed = COMPRESSED_LENGTH_BYTES;
	int compressedLength = (uchar)compressedDifference[0];
	compressedLength += ((uchar)compressedDifference[1]) << 8;
	compressedLength += ((uchar)compressedDifference[2]) << 16;
	while (indexCompressed < compressedLength + COMPRESSED_LENGTH_BYTES)
	{
		int indexOriginal = (uchar)compressedDifference[indexCompressed++];
		indexOriginal += ((uchar)compressedDifference[indexCompressed++]) << 8;
		indexOriginal += ((uchar)compressedDifference[indexCompressed++] & 3) << 16;
		decompressed[indexOriginal] += (char)(compressedDifference[indexCompressed - 1] >> 2);
	}
}