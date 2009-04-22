#ifndef _RAM_SONAR_FFT_H
#define _RAM_SONAR_FFT_H


#include <stdint.h>

#ifdef __BFIN
    #include <filter.h>
    #include <fract_complex.h>
#else
    #include <fftw3.h>
    typedef struct {
        int16_t re, im;
    } complex_fract16;
    typedef int16_t fract16;
#endif

namespace ram {
    
    namespace sonar {

        template<int N>
        class fft_fract16 {
        public:
            complex_fract16 dftOut[N];
            
        private:
#ifdef __BFIN
            complex_fract16 rfftTwid[N/2];
#else
            double fftwIn[N];
            fftw_complex fftwOut[N];
            fftw_plan fftwPlan;
#endif
            
        public:
            
            fft_fract16()
            {
#ifdef __BFIN
                twidfftrad2_fr16(rfftTwid, N);
#else
                fftwPlan = fftw_plan_dft_r2c_1d(N, fftwIn, fftwOut, FFTW_PATIENT);
#endif
            }
            
            ~fft_fract16()
            {
#ifdef __BFIN
#else
                fftw_destroy_plan(fftwPlan);
#endif
            }
            
            void doDFT(const int16_t dftIn[N])
            {
#ifdef __BFIN
                int block_exponent;
                // Fourier transform the current block.
                rfft_fr16(dftIn, dftOut, rfftTwid, 1, N, &block_exponent, 1);
#else
                for (int i = 0 ; i < N ; i ++)
                    fftwIn[i] = dftIn[i];
                
                fftw_execute(fftwPlan);
                
                for (int i = 0 ; i < N ; i ++)
                {
                    dftOut[i].re = fftwOut[i][0];
                    dftOut[i].im = fftwOut[i][1];
                }
#endif
            }
                
            };
        
    } /* namespace sonar */

} /* namespace ram */

#endif
