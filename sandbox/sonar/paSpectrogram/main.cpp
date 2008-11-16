#include "../../../packages/sonar/include/spectrum/SDFTSpectrum.h"
#include "../../../packages/sonar/include/adctypes.h"
#include "../../../packages/sonar/include/fixed/fixed.h"
#include "../../running_median/DoubleHeap.h"
#include <portaudio.h>
#include <math.h>
#include <iostream>
#include <stdint.h>

#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/OpenGL.h>
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glut.h>
#endif


using namespace ram::sonar;

typedef adc<16> myadc;
const static int SAMPLE_RATE = 44100;
const static int N = 800;
const static int displayStride = 1;
const static int avgStride = 8;
const static int nPastSpectra = 800;
static uint32_t pastSpectra[nPastSpectra][N];
static int iPastSpectra = 0;
static int displayStrideCounter = 0;
static int avgStrideCounter = 0;
static unsigned int clampFactor = 0;

static SDFTSpectrum<myadc, N, 1> spectrum;
myadc::DOUBLE_WIDE::UNSIGNED mag[N];
static DoubleHeap<myadc::DOUBLE_WIDE::UNSIGNED>* medianFilters[N];

static int paAudioReceivedCallback(const void* inputBuffer,
                                   void* output,
                                   unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo,
                                   PaStreamCallbackFlags statusFlags,
                                   void* userData)
{
    // Loop over all received samples
    for (int i = 0 ; i < framesPerBuffer ; i ++)
    {
        // Update the DFT
        spectrum.update(&((myadc::SIGNED*)inputBuffer)[i]);
        for (int k = 0 ; k < N ; k ++)
            mag[k] = fixed::magL1(spectrum.getAmplitude(k, 0));
        
        ++displayStrideCounter;
        ++avgStrideCounter;
        
        // Every avgStride samples, update the noise floor estimate
        if (avgStrideCounter >= avgStride)
        {
            for (int k = 0 ; k < N ; k ++)
                medianFilters[k]->push(mag[k]);
            avgStrideCounter = 0;
        }
        
        // Every displayStride samples, copy the result to the display buffer
        if (displayStrideCounter >= displayStride)
        {
            for (int k = 0 ; k < N ; k ++)
            {
                const myadc::DOUBLE_WIDE::UNSIGNED avg = medianFilters[k]->median();
                myadc::DOUBLE_WIDE::UNSIGNED resid;
                if (clampFactor*avg >= mag[k])
                    resid = 0;
                else
                    resid = mag[k];
                // Right shift by eight bits because the lowest eight bits of the
                // color represents the alpha channel, which isn't visible.
                resid <<= 8;
                pastSpectra[iPastSpectra][k] = resid;
            }
            
            // Update the index into the circular display buffer
            ++iPastSpectra;
            if (iPastSpectra >= nPastSpectra)
                iPastSpectra = 0;
            
            displayStrideCounter = 0;
        }
        
    }
    return 0;
}

static void glutDisplayCallback()
{
    // We don't ever need to clear the screen, because we draw over every pixel.
    // glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw the older half of the circular buffer of spectra
    glRasterPos2i(0,0);
    glDrawPixels(N, nPastSpectra-iPastSpectra+1,
                 GL_BGRA, GL_UNSIGNED_INT_8_8_8_8,
                 pastSpectra[iPastSpectra]);
    
    // Draw the newer half of the circular buffer of spectra
    glRasterPos2i(0, nPastSpectra-iPastSpectra);
    glDrawPixels(N, iPastSpectra-1,
                 GL_BGRA, GL_UNSIGNED_INT_8_8_8_8,
                 pastSpectra[0]);
    
    // Send to display
    glutSwapBuffers();
}

static void glutIdleCallback()
{
    // Whenever the CPU is idle, request a redraw.
    glutPostRedisplay();
}

static void glutKeyboardCallback(unsigned char key, int x, int y)
{
    if (key == '+' || key == '=')
        ++clampFactor;
    else if (key == '-' && clampFactor != 0)
        --clampFactor;
    else if (key == 'q')
        exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    // Initialize median filters
    for (int k = 0 ; k < N ; k ++)
        medianFilters[k] = new DoubleHeap<myadc::DOUBLE_WIDE::UNSIGNED>(205);
    
    // Initialize PortAudio
    PaStream* paStream;
    PaError paErr;
    paErr = Pa_Initialize();
    if (paErr != paNoError) goto paError;
    
    paErr = Pa_OpenDefaultStream(&paStream, 1, 0, paInt16, SAMPLE_RATE,
                                 paFramesPerBufferUnspecified,
                                 paAudioReceivedCallback, NULL);
    if (paErr != paNoError) goto paError;
    
    // Set up GLUT and its callbacks
    glutInit(&argc, argv);
    glutInitWindowSize(N, nPastSpectra);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutCreateWindow("Spectrogram");
    glutDisplayFunc(glutDisplayCallback);
    glutIdleFunc(glutIdleCallback);
    glutKeyboardFunc(glutKeyboardCallback);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0, N, 0, nPastSpectra);
    
    // Start PortAudio
    paErr = Pa_StartStream(paStream);
    if (paErr != paNoError) goto paError;
    
    // Start GLUT
    glutMainLoop();

    // Stop PortAudio after glutMainLoop exits.  (should never happen)
    paErr = Pa_StopStream(paStream);
    if (paErr != paNoError) goto paError;
    
    goto paNoError;
    
paError:
    std::cerr << "PortAudio error: " << Pa_GetErrorText(paErr) << std::endl;
    paErr = Pa_Terminate();
    if (paErr != paNoError)
        std::cerr << "PortAudio failed to terminate: " << Pa_GetErrorText(paErr) << std::endl;
    return EXIT_FAILURE;
    
paNoError:
    return EXIT_SUCCESS;
}
