#define ARM_MATH_CM4

#include "main.h"
#include "spectrum.h"
#include "arm_math.h"

#define FFT_BUFFER_SIZE 2048

arm_rfft_fast_instance_f32 fftHandler;

static float32_t fftBuffIn[FFT_BUFFER_SIZE];
static float32_t fftBuffOut[FFT_BUFFER_SIZE];

static uint8_t fftFlag = 0;
static float32_t fftMagnitudes[FFT_BUFFER_SIZE / 2];
uint8_t spectrum_data_ready;

extern uint16_t adc_buffer[4096];
extern uint16_t zero_offset;

void RunSpectrum(void);
void SpectrumInit(void);

void SpectrumInit(void)
{
	arm_rfft_fast_init_f32(&fftHandler, FFT_BUFFER_SIZE);
	fftFlag = 1;
}

void RunSpectrum(void)
{
	if (fftFlag == 0 || spectrum_data_ready == 0)
	{
		return;
	}

	for (uint16_t i = 0; i < FFT_BUFFER_SIZE; i++)
	{
		fftBuffIn[i] = (float32_t)adc_buffer[2048 + i] - (float32_t)zero_offset;
	}

	spectrum_data_ready = 0;

	arm_rfft_fast_f32(&fftHandler, fftBuffIn, fftBuffOut, 0);

	arm_cmplx_mag_f32(fftBuffOut, fftMagnitudes, FFT_BUFFER_SIZE / 2);
}
