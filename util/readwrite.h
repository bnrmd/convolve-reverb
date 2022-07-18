#ifndef STREAMS_H
#define STREAMS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string>
#include <fstream>

void writeWavFile(double *outputArray, int outputArraySize, int channels, char *filename);
void writeWavFileHeader(int channels, int numberSamples, double outputRate, FILE *outputFile);
double *readWavFile(int *arraySize, int *channels, char *filename);
void readWavFileHeader(int *channels, int *numSamples, FILE *inputFile);

#endif
