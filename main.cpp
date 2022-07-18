/*
  Usage: ./convolve inputFile IRFile outputFile
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string>
#include <fstream>
#include "fft.h"
#include "util/readwrite.h"
#include "util/streams.h"

using namespace std;

int main(int argc, char **argv)
{
  char *inputFilename;
  char *IRFilename;
  char *outputFilename;

  if (argc != 4)
  {
    printf("Insufficient arguments. Usage:  ./convolve inputFile IRFile outputFile\n");
    exit(-1);
  }

  inputFilename = argv[1];
  IRFilename = argv[2];
  outputFilename = argv[3];

  printf("Reading input array from %s...\n", inputFilename);
  int inputChannels;
  double *inputArray;
  int inputArraySize;

  inputArray = readWavFile(&inputArraySize, &inputChannels, inputFilename);

  printf("Reading IR array from %s...\n", IRFilename);
  int IRChannels;
  double *IRArray;
  int IRArraySize;

  IRArray = readWavFile(&IRArraySize, &IRChannels, IRFilename);

  /* Convolution */
  int outputArraySize = inputArraySize + IRArraySize - 1; // determine output array size

  // determine array size of the complex arrays
  int N = 0;
  int complexArraySize = 0;
  while (complexArraySize < outputArraySize)
  {
    complexArraySize = pow(2, N);
    N++;
  }

  // Embed input and IR to complex arrays of size 2^N
  std::complex<double> *inputComplexArray;
  inputComplexArray = new std::complex<double>[complexArraySize];
  std::complex<double> *IRComplexArray;
  IRComplexArray = new std::complex<double>[complexArraySize];

  if (inputArraySize < IRArraySize)
  {
    for (int i = inputArraySize; i < complexArraySize; i++)
    {
      inputComplexArray[i] = 0.0;
      IRComplexArray[i] = 0.0;
    }
    for (int i = 0; i < IRArraySize; i++)
    {
      IRComplexArray[i] = IRArray[i];
      if (i < inputArraySize)
      {
        inputComplexArray[i] = inputArray[i];
      }
    }
  }
  else
  {
    for (int i = IRArraySize; i < complexArraySize; i++)
    {
      inputComplexArray[i] = 0.0;
      IRComplexArray[i] = 0.0;
    }
    for (int i = 0; i < inputArraySize; i++)
    {
      inputComplexArray[i] = inputArray[i];
      if (i < IRArraySize)
      {
        IRComplexArray[i] = IRArray[i];
      }
    }
  }

  std::valarray<std::complex<double>> inputComplexArrayData(inputComplexArray, complexArraySize);
  fft(inputComplexArrayData); // Perform fft on inputComplexArrayData

  std::valarray<std::complex<double>> IRComplexArrayData(IRComplexArray, complexArraySize);
  fft(IRComplexArrayData); // Perform fft on IRComplexArrayData

  // Multiply the resulting complex arrays entrywise
  // We will treat inputComplexArray as the output array so we don't need to make a new array
  for (int i = 0; i < complexArraySize; i++)
  {
    inputComplexArrayData[i] = inputComplexArrayData[i] * IRComplexArrayData[i];
  }

  // Perform IFFT on the output
  ifft(inputComplexArrayData);

  // Copy this complex array into a real array of length n = outputSize
  double *outputArray = new double[outputArraySize];

  int i;
  for (int i = 0; i < outputArraySize - 1; i += 2)
  {
    outputArray[i] = inputComplexArrayData[i].real();
    outputArray[i + 1] = inputComplexArrayData[i + 1].real();
  }

  if (i == outputArraySize - 1)
  {
    outputArray[i] = inputComplexArrayData[i].real();
  }

  int outputChannels = 1; // Currently just set for MONO
  printf("Writing result to file %s...\n", outputFilename);
  writeWavFile(outputArray, outputArraySize, outputChannels, outputFilename);

  printf("Finished");
}
