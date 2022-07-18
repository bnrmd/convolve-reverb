/*
 * Contains methods for reading / writing integers to streams.
 */

#include "streams.h"

/*
 * Writes an integer to the provided stream in little-endian form
 */
size_t fwriteIntLSB(int data, FILE *stream)
{
  unsigned char array[4];

  array[3] = (unsigned char)((data >> 24) & 0xFF);
  array[2] = (unsigned char)((data >> 16) & 0xFF);
  array[1] = (unsigned char)((data >> 8) & 0xFF);
  array[0] = (unsigned char)(data & 0xFF);

  return fwrite(array, sizeof(unsigned char), 4, stream);
}

/*
 * Reads an integer from the provided stream in little-endian form
 */
int freadIntLSB(FILE *stream)
{
  unsigned char array[4];

  fread(array, sizeof(unsigned char), 4, stream);

  int data;
  data = array[0] | (array[1] << 8) | (array[2] << 16) | (array[3] << 24);

  return data;
}

/*
 * Writes a short integer to the provided stream in little-endian form
 */
size_t fwriteShortLSB(short int data, FILE *stream)
{
  unsigned char array[2];

  array[1] = (unsigned char)((data >> 8) & 0xFF);
  array[0] = (unsigned char)(data & 0xFF);

  return fwrite(array, sizeof(unsigned char), 2, stream);
}

/*
 * Reads a short integer from the provided stream in little-endian form
 */
short int freadShortLSB(FILE *stream)
{
  unsigned char array[2];

  fread(array, sizeof(unsigned char), 2, stream);

  int data;
  data = array[0] | (array[1] << 8);

  return data;
}
