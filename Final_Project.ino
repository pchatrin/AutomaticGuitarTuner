#include <arduinoFFT.h>
#include <Servo.h>
Servo myServo = Servo();

arduinoFFT FFT = arduinoFFT();

#define DELAY 1000 

const uint16_t samples = 1<<10; //2^n
const double samplingFrequency = 1000;

double vReal[samples];
double vImag[samples];

int i = 0, val;
int analogPin = A0;

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

int margin = 1; //acceptable error margin from reference frequency

//define string frequency and trigger input pin
// reference frequencies are obtained from Wikipedia
#define EL 82.41;
#define A 110.00; 
#define D 146.83; 
#define G 196.00; 
#define B 246.94; 
#define EH 329.63; 
int stringTriggered[] = {16,5,4,0,2,14}; // {EH, B, G, D, A EL}
int noString = 6; //number of strings

//int stringTriggered[] = {16}; // {EH, B, G, D, A EL}
//int noString = 1; //number of strings

//Servo Motor Parameter
int string = -1; //define which string to tune
float curFrequency = 0; //frequecy collected from sampler
float refFrequency = 0;
int doneTuning = 0; //if 0 => not done tuning
int prevString = 0;
int servoPin = D7; //pin connect to servo motor

void setup() {
  int i;
  for(i = 0; i < noString; i++){
    pinMode(stringTriggered[i], INPUT);
  }
  myServo.attach(servoPin);
  Serial.begin(9600);
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
  float curFrequency = FFT.MajorPeak(vReal, samples, samplingFrequency);

  ///find which string to tune
  int i = 0;
  for(i = 0; i < noString; i++){
    if(int(digitalRead(stringTriggered[i])) == 0){ //use pulldown resistor
      string = i + 1;
      break;
    }
    else{
      string = -1;
    }
  }

  //Set target string frequency
  switch(string){
    case 1:
      refFrequency = EH;
      break;
    case 2:
      refFrequency = B;
      break;
    case 3:
      refFrequency = G;
      break;
    case 4:
      refFrequency = D;
      break;
    case 5:
      refFrequency = A;
      break;
    case 6:
      refFrequency = EL;
      break;
    default:
      refFrequency = 0;
      break;
  }
  
  Serial.print("cur: ");
  Serial.print(curFrequency);
  Serial.println();
  Serial.print("ref: ");
  Serial.print(refFrequency);
  Serial.println();

  if(string != prevString){
    doneTuning = 0;
  }

  if(!doneTuning && abs(curFrequency - refFrequency) >= margin){
    
    float diff = refFrequency - curFrequency; //difference between measured frequency and desire freqnecy
    Serial.print("diff: ");
    Serial.print(diff);
    Serial.println();
    
    //tunning
    if(abs(diff) <= margin){ //tunned
      doneTuning = 1;
    }
    else if(diff < -(margin)){ //tune down
      Serial.print("TUNE DOWN");
      myServo.write(0);
      delay(-20*diff);
      myServo.writeMicroseconds(1500);
    }
    else if(diff > margin){ //tune up
      Serial.print("TUNE UP");
      myServo.write(180);
      delay(20*diff);
      myServo.writeMicroseconds(1500);
    }
    Serial.println();
  }

  prevString = string;
  Serial.println();
}
