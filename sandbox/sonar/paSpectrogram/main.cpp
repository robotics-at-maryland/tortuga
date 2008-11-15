#include "../../../packages/sonar/include/spectrum/SDFTSpectrum.h"
#include "../../../packages/sonar/include/adctypes.h"
#include "../../../packages/sonar/include/fixed/fixed.h"
#include "../../running_median/DoubleHeap.h"
#include "../../../packages/core/include/AveragingFilter.h"
#include "portaudio.h"
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <iostream>
#include <stdint.h>

using namespace ram::sonar;
using namespace ram::core;

typedef adc<8> myadc;
const static int SAMPLE_RATE = 44100;
const static int N = 800;
const static int stride = 1;
const static int nPastSpectra = 800;
static uint8_t pastSpectra[nPastSpectra][N];
static int iPastSpectra = 0;
static int sampleNumber = 0;

static SDFTSpectrum<myadc, N, 1> spectrum;
static DoubleHeap<myadc::DOUBLE_WIDE::UNSIGNED>* medianFilters[N];
static AveragingFilter<myadc::DOUBLE_WIDE::UNSIGNED, stride*N*100> avgFilters[N];

static int paAudioReceivedCallback(const void* inputBuffer,
                                   void* output,
                                   unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo,
                                   PaStreamCallbackFlags statusFlags,
                                   void* userData)
{
    for (int i = 0 ; i < framesPerBuffer ; i ++)
    {
        sampleNumber += 1;
        spectrum.update(&((myadc::SIGNED*)inputBuffer)[i]);
        for (int k = 0 ; k < N ; k ++)
            avgFilters[k].addValue(fixed::magL1(spectrum.getAmplitude(k, 0)));
        if (sampleNumber == stride)
        {
            for (int k = 0 ; k < N ; k ++)
            {
                myadc::DOUBLE_WIDE::UNSIGNED mag = fixed::magL1(spectrum.getAmplitude(k, 0));
                myadc::DOUBLE_WIDE::UNSIGNED avg = avgFilters[k].getValue();
                //medianFilters[k]->push(mag);
                //myadc::DOUBLE_WIDE::UNSIGNED avg = medianFilters[k]->median();
                myadc::DOUBLE_WIDE::UNSIGNED resid;
                if (avg > mag)
                    resid = 0.5*avg;
                else
                    resid = mag - 0.5*avg;
                if (resid > (1 << 8))
                    resid = 1 << 8;
                pastSpectra[iPastSpectra][k] = resid;
            }
            ++iPastSpectra;
            if (iPastSpectra >= nPastSpectra)
                iPastSpectra = 0;
            sampleNumber = 0;
        }
    }
    return 0;
}

static void glutDisplayCallback()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0,0);
    glDrawPixels(N, nPastSpectra-iPastSpectra+1, GL_GREEN, GL_UNSIGNED_BYTE, pastSpectra[iPastSpectra]);
    glRasterPos2i(0, nPastSpectra-iPastSpectra);
    glDrawPixels(N, iPastSpectra-1, GL_GREEN, GL_UNSIGNED_BYTE, pastSpectra[0]);
    glutSwapBuffers();
}

static void glutIdleCallback()
{
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    for (int k = 0 ; k < N ; k ++)
        medianFilters[k] = new DoubleHeap<myadc::DOUBLE_WIDE::UNSIGNED>(81);
    PaStream* paStream;
    PaError paErr;
    paErr = Pa_Initialize();
    if (paErr != paNoError) goto paError;
    
    paErr = Pa_OpenDefaultStream(&paStream, 1, 0, paInt8, SAMPLE_RATE,
                                 paFramesPerBufferUnspecified,
                                 paAudioReceivedCallback, NULL);
    if (paErr != paNoError) goto paError;
    
    glutInit(&argc, argv);
    glutInitWindowSize(N, nPastSpectra);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Spectrogram");
    glutDisplayFunc(glutDisplayCallback);
    glutIdleFunc(glutIdleCallback);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0, N, 0, nPastSpectra);
    
    paErr = Pa_StartStream(paStream);
    if (paErr != paNoError) goto paError;
        
    glutMainLoop();

    paErr = Pa_StopStream(paStream);
    if (paErr != paNoError) goto paError;
    
paError:
    std::cerr << "PortAudio error: " << Pa_GetErrorText(paErr) << std::endl;
    paErr = Pa_Terminate();
    if (paErr != paNoError)
        std::cerr << "PortAudio failed to terminate: " << Pa_GetErrorText(paErr) << std::endl;
}
