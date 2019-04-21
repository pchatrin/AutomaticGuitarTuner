//File for ESP8226 to perform Fast Fourier Transform

#include <arduinoFFT.h>     //library to perform FFT   
#include "MegunoLink.h"     //library for filtering
#include "Filter.h"

arduinoFFT FFT = arduinoFFT();
ExponentialFilter<long> ADCFilter(5, 0);

#define DELAY 1000 

const uint16_t samples = 1<<10; //2^n
const double samplingFrequency = 1000;

double vReal[samples];
double vImag[samples];

int i = 0, val;
int analogPin = A0;
int outputPin = 5; //pin D1

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
}

void loop() {
  uint32_t last = micros();
  
  for(i = 0; i < samples; i++){
    vReal[i] = analogRead(analogPin) / 4;
    vImag[i] = 0;
    while(micros() - last < DELAY);
    last = micros();
  }
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);    //Compute Weight
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD);                    //Compute FFT
  FFT.ComplexToMagnitude(vReal, vImag, samples);                      //Compute Magnitude
  //FFT.MajorPeak(vReal, samples, samplingFrequency)                    //Compute Major Peak Frequency;
  Serial.print(double(FFT.MajorPeak(vReal, samples, samplingFrequency)));
  Serial.println(" ");
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
  break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
  break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
  break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
