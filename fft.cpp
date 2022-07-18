/*
 * Contains methods for FFT and IFFT algorithms.
 * Methods have been borrowed from http://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
 */

#include "fft.h"

#define PI 3.14159265358979

/**
 * Cooley–Tukey FFT (in-place, divide-and-conquer)
 *
 * @param[in] x the array to apply FFT on
 */
void fft(std::valarray<std::complex<double>> &x)
{
  const size_t N = x.size();
  if (N <= 1)
    return;

  // divide
  std::valarray<std::complex<double>> even = x[std::slice(0, N / 2, 2)];
  std::valarray<std::complex<double>> odd = x[std::slice(1, N / 2, 2)];

  // conquer
  fft(even);
  fft(odd);

  // combine
  for (size_t k = 0; k < N / 2; ++k)
  {
    std::complex<double> t = std::polar(1.0, -2 * PI * k / N) * odd[k];
    x[k] = even[k] + t;
    x[k + N / 2] = even[k] - t;
  }
}

/**
 * Inverse FFT (in-place)
 *
 * @param[in] x the array to apply Inverse FFT on
 */
void ifft(std::valarray<std::complex<double>> &x)
{
  // conjugate the complex numbers
  x = x.apply(std::conj);

  // forward fft
  fft(x);

  // conjugate the complex numbers again
  x = x.apply(std::conj);

  // scale the numbers
  x /= x.size();
}
