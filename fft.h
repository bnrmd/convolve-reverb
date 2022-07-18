#ifndef FFT_H
#define FFT_H

#include <complex>
#include <valarray>

void fft(std::valarray<std::complex<double>> &x);
void ifft(std::valarray<std::complex<double>> &x);

#endif
