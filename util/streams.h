#ifndef STREAMS_H
#define STREAMS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string>
#include <fstream>

size_t fwriteIntLSB(int data, FILE *stream);
int freadIntLSB(FILE *stream);
size_t fwriteShortLSB(short int data, FILE *stream);
short int freadShortLSB(FILE *stream);

#endif
