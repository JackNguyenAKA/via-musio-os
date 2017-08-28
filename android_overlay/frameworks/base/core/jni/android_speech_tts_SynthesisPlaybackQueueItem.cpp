
//
// Created by trent on 17. 6. 21.
//

#define LOG_TAG "CustomTTSFx"
#include <utils/Log.h>

#include "jni.h"
#include "JNIHelp.h"
#include "android/log.h"
#include "android_runtime/AndroidRuntime.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

#define LOG_D(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOG_E(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace android {

#define BUFFERSCALE 0.4f

const int buffer_unit = sizeof(int16_t);

static int sOversampling;
static int sFrameSize;

bool stop_variable = false;
bool error_variable = false;

#define M_PI 3.14159265358979323846

#define M_ARRAY_SIZE 1024

static bool sInit = false;

void initNativeStream() {
    sInit = false;
}

void freeNativeStream() {
}

/****************************************************************************
*
* COPYRIGHT 1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
*
* 						The Wide Open License (WOL)
*
* Permission to use, copy, modify, distribute and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice and this license appear in all source copies.
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
* ANY KIND. See http://www.dspguru.com/wol.htm for more information.
*
*****************************************************************************/

double modAtan2(double x, double y) {
    double signx;
    if (x > 0.) signx = 1.;
    else signx = -1.;

    if (x == 0.) return 0.;
    if (y == 0.) return signx * M_PI / 2.;

    return atan2(x, y);
}

void fft(float *fftBuffer, long fftFrameSize, long sign)
/*
	FFT routine, (C)1996 S.M.Bernsee.
*/
{
    float wr, wi, arg, *p1, *p2, temp;
    float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
    long i, bitm, j, le, le2, k;

    for (i = 2; i < 2 * fftFrameSize - 2; i += 2) {
        for (bitm = 2, j = 0; bitm < 2 * fftFrameSize; bitm <<= 1) {
            if (i & bitm) j++;
            j <<= 1;
        }
        if (i < j) {
            p1 = fftBuffer + i;
            p2 = fftBuffer + j;
            temp = *p1;
            *(p1++) = *p2;
            *(p2++) = temp;
            temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }
    for (k = 0, le = 2; k < (long) (log(fftFrameSize) / log(2.) + .5); k++) {
        le <<= 1;
        le2 = le >> 1;
        ur = 1.0;
        ui = 0.0;
        arg = M_PI / (le2 >> 1);
        wr = cos(arg);
        wi = sign * sin(arg);
        for (j = 0; j < le2; j += 2) {
            p1r = fftBuffer + j;
            p1i = p1r + 1;
            p2r = p1r + le2;
            p2i = p2r + 1;
            for (i = j; i < 2 * fftFrameSize; i += le) {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr;
                *p2i = *p1i - ti;
                *p1r += tr;
                *p1i += ti;
                p1r += le;
                p1i += le;
                p2r += le;
                p2i += le;
            }
            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}

void streamPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp,
                      float sampleRate, float *indata, float *outdata)
/**
 * A modification of smbPitchShift algorithm for streaming buffer.
 */
{
    static float sInQueue[M_ARRAY_SIZE * sizeof(float)];
    static float sOutQueue[M_ARRAY_SIZE * sizeof(float)];

    float sFFTworkspace[2 * M_ARRAY_SIZE * sizeof(float)];
    float sLastPhase[(M_ARRAY_SIZE / 2 + 1) * sizeof(float)];
    float sSumPhase[(M_ARRAY_SIZE / 2 + 1) * sizeof(float)];

    static float sOutputAccumulator[2 * M_ARRAY_SIZE * sizeof(float)];

    float sAnalysisFreq[M_ARRAY_SIZE * sizeof(float)];
    float sAnalysisMag[M_ARRAY_SIZE * sizeof(float)];
    float sSynthesisFreq[M_ARRAY_SIZE * sizeof(float)];
    float sSynthesisMag[M_ARRAY_SIZE * sizeof(float)];

    static long sRover = 0;

    double magn, phase, tmp, window, real, imag;
    double freqPerBin, expct;
    long i, k, qpd, index, inQueueLatency, stepSize, fftFrameSize2;

    fftFrameSize2 = fftFrameSize / 2;
    stepSize = fftFrameSize / osamp;
    freqPerBin = sampleRate / (double) fftFrameSize;
    expct = 2. * M_PI * (double) stepSize / (double) fftFrameSize;
    inQueueLatency = fftFrameSize - stepSize;
    if (sRover == 0) sRover = inQueueLatency;

    if (sInit == false) {
        memset(sInQueue, 0, fftFrameSize * sizeof(float));
        memset(sOutQueue, 0, fftFrameSize * sizeof(float));
        memset(sOutputAccumulator, 0, 2 * fftFrameSize * sizeof(float));

        LOG_E("Arrays filled with zeros");
        sInit = true;
    }
    // There is no need to init our local arrays.
    // Doing it would be more safer, but it works fine without it.

    for (i = 0; i < numSampsToProcess; i++) {

        sInQueue[sRover] = indata[i];
        outdata[i] = sOutQueue[sRover - inQueueLatency];
        sRover++;

        if (sRover >= fftFrameSize) {
            sRover = inQueueLatency;

            for (k = 0; k < fftFrameSize; k++) {
                window = -.5 * cos(2. * M_PI * (double) k / (double) fftFrameSize) + .5;
                sFFTworkspace[2 * k] = sInQueue[k] * window;
                sFFTworkspace[2 * k + 1] = 0.;
            }


            /* ***************** ANALYSIS ******************* */
            fft(sFFTworkspace, fftFrameSize, -1);

            for (k = 0; k <= fftFrameSize2; k++) {

                real = sFFTworkspace[2 * k];
                imag = sFFTworkspace[2 * k + 1];

                magn = 2. * sqrt(real * real + imag * imag);
                phase = modAtan2(imag, real);

                tmp = phase - sLastPhase[k];
                sLastPhase[k] = phase;

                tmp -= (double) k * expct;

                qpd = tmp / M_PI;
                if (qpd >= 0) qpd += qpd & 1;
                else qpd -= qpd & 1;
                tmp -= M_PI * (double) qpd;

                tmp = osamp * tmp / (2. * M_PI);

                tmp = (double) k * freqPerBin + tmp * freqPerBin;

                sAnalysisMag[k] = magn;
                sAnalysisFreq[k] = tmp;

            }

            /* ***************** PROCESSING ******************* */
            memset(sSynthesisMag, 0, fftFrameSize * sizeof(float));
            memset(sSynthesisFreq, 0, fftFrameSize * sizeof(float));
            for (k = 0; k <= fftFrameSize2; k++) {
                index = k * pitchShift;
                if (index <= fftFrameSize2) {
                    sSynthesisMag[index] += sAnalysisMag[k];
                    sSynthesisFreq[index] = sAnalysisFreq[k] * pitchShift;
                }
            }

            /* ***************** SYNTHESIS ******************* */
            for (k = 0; k <= fftFrameSize2; k++) {

                magn = sSynthesisMag[k];
                tmp = sSynthesisFreq[k];

                tmp -= (double) k * freqPerBin;

                tmp /= freqPerBin;

                tmp = 2. * M_PI * tmp / osamp;

                tmp += (double) k * expct;

                sSumPhase[k] += tmp;
                phase = sSumPhase[k];

                sFFTworkspace[2 * k] = magn * cos(phase);
                sFFTworkspace[2 * k + 1] = magn * sin(phase);
            }

            for (k = fftFrameSize + 2; k < 2 * fftFrameSize; k++) sFFTworkspace[k] = 0.;

            fft(sFFTworkspace, fftFrameSize, 1);

            for (k = 0; k < fftFrameSize; k++) {
                window = -.5 * cos(2. * M_PI * (double) k / (double) fftFrameSize) + .5;
                sOutputAccumulator[k] += 2. * window * sFFTworkspace[2 * k] / (fftFrameSize2 * osamp);
            }
            for (k = 0; k < stepSize; k++) sOutQueue[k] = sOutputAccumulator[k];

            memmove(sOutputAccumulator, sOutputAccumulator + stepSize, fftFrameSize * sizeof(float));

            for (k = 0; k < inQueueLatency; k++) sInQueue[k] = sInQueue[k + stepSize];
        }
    }
}

void scaleBuffer(int16_t* buffer, int buffer_size, float buffer_scale) {
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = buffer[i] * buffer_scale;
    }
}

jbyte *fxAppliedByteBuffer(jbyte *byteBuffer, int byte_buffer_size, float pitch_shift,
                           int sample_rate, int oversampling, int frame_size) {

    int16_t *PCMBuffer;
    float *floatBuffer;

    jbyte *resultByteBuffer;

    //int16_t and float have different sizes, but same number of elements(byte_buffer_size / 2)
    PCMBuffer = (int16_t *) malloc(byte_buffer_size / buffer_unit * buffer_unit);
    floatBuffer = (float *) malloc(byte_buffer_size / buffer_unit * sizeof(float));

    const float NORM_MUL = 32768;
    const float NORM_DIV = (float) 1 / NORM_MUL;

    resultByteBuffer = (jbyte *) malloc(byte_buffer_size * sizeof(jbyte));

    // copy byte data to PCM buffer (2 jbyte == 1 int16_t)
    memcpy(PCMBuffer, byteBuffer, byte_buffer_size * sizeof(jbyte));

    // scale buffer for fixing clipping
    scaleBuffer(PCMBuffer, byte_buffer_size / buffer_unit, BUFFERSCALE);

    for (int i = 0; i < byte_buffer_size / buffer_unit; i++) {
        // change to -1 ~ 1
        floatBuffer[i] = (float) PCMBuffer[i] * NORM_DIV;
    }

    streamPitchShift(pitch_shift, byte_buffer_size / buffer_unit, (long) frame_size,
                     (long) oversampling, sample_rate, floatBuffer, floatBuffer);

    for (int i = 0; i < byte_buffer_size / buffer_unit; i++) {
        // change to original scale
        PCMBuffer[i] = (int) (floatBuffer[i] * NORM_MUL);
    }

    // copy to result buffer
    memcpy(resultByteBuffer, PCMBuffer, byte_buffer_size * sizeof(jbyte));

    free(PCMBuffer);
    free(floatBuffer);

    return resultByteBuffer;
}

static void
android_speech_tts_SynthesisPlaybackQueueItem_initRealtimeFx(
        JNIEnv *env, jobject thiz, jint overSampling, jint frameSize) {

    sOversampling = overSampling;
    sFrameSize = frameSize;
    initNativeStream();
    LOG_D("Arrays Initialized!");
    }

static void
android_speech_tts_SynthesisPlaybackQueueItem_resetRealtimeFx(JNIEnv *env, jobject thiz){

    freeNativeStream();
    LOG_D("Arrays Freed!");
    }

static jbyteArray
android_speech_tts_SynthesisPlaybackQueueItem_byteBufferFx(
        JNIEnv *env, jobject thiz, jbyteArray buffer_, jint bufferSizeInBytes, jfloat pitchShift,
        jint sampleRate) {

    jbyte *byteBuffer = env->GetByteArrayElements(buffer_, NULL);

    // release buffer data
    env->ReleaseByteArrayElements(buffer_, byteBuffer, 0);

    // process buffer with Pitch Shift
    jbyte *processedBuffer = fxAppliedByteBuffer(byteBuffer, (int) bufferSizeInBytes, pitchShift,
                                                 (int) sampleRate, sOversampling, sFrameSize);

    // make return jbyteArray with processedBuffer
    jbyteArray resultArray = env->NewByteArray(bufferSizeInBytes);

    env->SetByteArrayRegion(resultArray, 0, bufferSizeInBytes, processedBuffer);

    free(processedBuffer);

    return resultArray;
}

/*
 * JNI registration.
 */
static JNINativeMethod synthMethods[] = {
    /* name, signature, funcPtr */
    { "initRealtimeFx",     "(II)V",
                            (void*) android_speech_tts_SynthesisPlaybackQueueItem_initRealtimeFx },
    { "resetRealtimeFx",    "()V",
                            (void*) android_speech_tts_SynthesisPlaybackQueueItem_resetRealtimeFx },
    { "byteBufferFx",       "([BIFI)[B",
                            (void*) android_speech_tts_SynthesisPlaybackQueueItem_byteBufferFx },
};

int register_android_speech_tts_SynthesisPlaybackQueueItem(JNIEnv* env) {

    return AndroidRuntime::registerNativeMethods(env, "android/speech/tts/SynthesisPlaybackQueueItem",
                                            synthMethods, NELEM(synthMethods));
}
};