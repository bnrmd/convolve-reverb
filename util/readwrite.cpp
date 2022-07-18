/*
 * Methods to read / write from and to a WAV file. Code borrowed from tutorial.
 */

#include "streams.h"
#include "readwrite.h"

//  Standard sample rate in Hz
#define SAMPLE_RATE 44100.0

//  Standard sample size in bits
#define BITS_PER_SAMPLE 16

// Standard sample size in bytes
#define BYTES_PER_SAMPLE (BITS_PER_SAMPLE / 8)

// Rescaling factor to convert between 16-bit shorts and doubles between -1 and 1
#define MAX_SHORT_VALUE 32768

// Number of channels
#define MONOPHONIC 1
#define STEREOPHONIC 2

// Offset of the fmt chunk in the WAV header
#define FMT_OFFSET 12

/*
 * Reads a WAV file header
 */
void readWavFileHeader(int *channels, int *numSamples, FILE *inputFile)
{
  int sampleRate;
  int bytesPerSecond;
  int dataChunkSize;

  unsigned char buffer[64];
  fread(buffer, sizeof(unsigned char), FMT_OFFSET, inputFile); // FMT_OFFSET = 12 bytes

  freadIntLSB(inputFile);               // Reading Subchunk1ID
  int fmtSize = freadIntLSB(inputFile); // Reading and saving Subchunk1Size - This is the size of the remaining sub-chunk. Saving because this size can differ between different audio files.
  freadShortLSB(inputFile);             // Reading AudioFormat. audio fmt = 1

  *channels = freadShortLSB(inputFile);    // Reading and storing NumChannels
  sampleRate = freadIntLSB(inputFile);     // Reading and storing SampleRate
  bytesPerSecond = freadIntLSB(inputFile); // Reading and storing ByteRate

  int frameSize = freadShortLSB(inputFile); // Reading BlockAlign
  int bitRate = freadShortLSB(inputFile);   // Reading BitsPerSample (8 bits, 16 bits, etc.)

  // confirm bit rate is 16
  if (bitRate != BITS_PER_SAMPLE)
  {
    printf("Error: bit rate of provided WAV file is not 16. Exiting.");
    exit(-1);
  }

  // confirm sample rate is 44.1 kHz
  if (sampleRate != SAMPLE_RATE)
  {
    printf("Error: sample rate of provided WAV file is not 44.1 Hz. Exiting.");
    exit(-1);
  }

  // skip forward to dataChunkSize
  fread(buffer, sizeof(unsigned char), fmtSize - 12, inputFile);

  dataChunkSize = freadIntLSB(inputFile); // We have reached the data chunk. First read and store data chunk size.
  printf("Data chunk size: %d\n", dataChunkSize);

  *numSamples = dataChunkSize / (BYTES_PER_SAMPLE * (*channels)); // Calculate number of samples based on dataChunkSize (different when MONO / STEREO)
};

/*
 * Reads a WAV file
 */
double *readWavFile(int *arraySize, int *channels, char *filename)
{
  double *array;

  FILE *inputFileStream = fopen(filename, "rb");
  if (inputFileStream == NULL)
  {
    printf("File %s could not be opened for reading\n", filename);
    exit(-1);
  }

  int numSamples;

  readWavFileHeader(channels, &numSamples, inputFileStream);

  printf("Channels: %d\n", *channels);
  printf("Number of samples: %d\n", numSamples);

  if (numSamples <= 0)
  {
    printf("The file %s doesn't contain any samples. Exiting the program.\n", filename);
    exit(0);
  }

  *arraySize = numSamples * (*channels);

  array = new double[*arraySize];
  short *intArray = new short[*arraySize];

  int count = fread(intArray, BYTES_PER_SAMPLE, numSamples, inputFileStream);

  int largest = 0;
  for (int i = 0; i < *arraySize; i++)
  {
    if (intArray[i] > largest)
    {
      largest = intArray[i];
    }
  }

  for (int i = 0; i < *arraySize; i++)
  {
    array[i] = ((double)intArray[i]) / largest;
  }

  delete[] intArray;

  return array;
};

/*
 * Writes the header for a WAV file with the given attributes to the provided filestream
 */
void writeWavFileHeader(int channels, int numberSamples, double outputRate, FILE *outputFile)
{
  /*  Calculate the total number of bytes for the data chunk  */
  int dataChunkSize = numberSamples * BYTES_PER_SAMPLE;

  /*  Calculate the total number of bytes for the form size  */
  int formSize = 36 + dataChunkSize;

  /*  Calculate the total number of bytes per frame  */
  short int frameSize = channels * BYTES_PER_SAMPLE;

  /*  Calculate the byte rate  */
  int bytesPerSecond = (int)ceil(outputRate * frameSize);

  /*  Write header to file  */
  /*  Form container identifier  */
  fputs("RIFF", outputFile);

  /*  Form size  */
  fwriteIntLSB(formSize, outputFile);

  /*  Form container type  */
  fputs("WAVE", outputFile);

  /*  Format chunk identifier (Note: space after 't' needed)  */
  fputs("fmt ", outputFile);

  /*  Format chunk size (fixed at 16 bytes)  */
  fwriteIntLSB(16, outputFile);

  /*  Compression code:  1 = PCM  */
  fwriteShortLSB(1, outputFile);

  /*  Number of channels  */
  fwriteShortLSB((short)MONOPHONIC, outputFile);

  /*  Output Sample Rate  */
  fwriteIntLSB((int)outputRate, outputFile);

  /*  Bytes per second  */
  fwriteIntLSB(bytesPerSecond, outputFile);

  /*  Block alignment (frame size)  */
  fwriteShortLSB(frameSize, outputFile);

  /*  Bits per sample  */
  fwriteShortLSB(BITS_PER_SAMPLE, outputFile);

  /*  Sound Data chunk identifier  */
  fputs("data", outputFile);

  /*  Chunk size  */
  fwriteIntLSB(dataChunkSize, outputFile);
}

/*
 * Creates a WAV file with the contents of the provided outputArray as the samples, and writes it to the given filename
 */
void writeWavFile(double *outputArray, int outputArraySize, int channels, char *filename)
{
  // open a binary output file stream for writing
  FILE *outputFileStream = fopen(filename, "wb");
  if (outputFileStream == NULL)
  {
    printf("File %s cannot be opened for writing\n", filename);
    return;
  }

  // create an 16-bit integer array to hold rescaled samples
  short *intArray = new short[outputArraySize];

  // find the largest entry and uses that to rescale all other
  // doubles to be in the range (-1, 1) to prevent 16-bit integer overflow
  double largestDouble = 1;
  for (int i = 0; i < outputArraySize; i++)
  {
    if (outputArray[i] > largestDouble)
    {
      largestDouble = outputArray[i];
    }
  }

  for (int i = 0; i < outputArraySize; i++)
  {
    intArray[i] = (short)((outputArray[i] / largestDouble) * MAX_SHORT_VALUE);
  }

  int numSamples = outputArraySize;

  // actual file writing
  writeWavFileHeader(channels, numSamples, SAMPLE_RATE, outputFileStream);
  fwrite(intArray, sizeof(short), outputArraySize, outputFileStream);

  // clear memory from heap
  delete[] intArray;
}
