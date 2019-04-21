#include <Servo.h> //include servo motor library
Servo myServo = Servo();

//Frequency Measuring Parameter
int inputPin = A0; //pin to collect data from
float curFrequency = 0; //frequecy collected from sampler
int margin = 1; //acceptable error margin from reference frequency

//define string frequency and trigger input pin
// reference frequencies are obtained from Wikipedia
#define EL 82.41;
#define A 110.00; 
#define D 146.83; 
#define G 196.00; 
#define B 246.94; 
#define EH 329.63; 
int stringTriggered[] = {2,3,4,5,6,7}; // {EH, B, G, D, A EL}
int noString = 6; //number of strings

//Servo Motor Parameter
int string = 0; //define which string to tune
float refFrequency = 0;
int doneTuning = 0; //if 0 => not done tuning
int prevString = 0;
int servoPin = 9; //pin connect to servo motor
int LEDpin = 8; //pin for LED

void setup() {
  //assign pinmode
  int i;
  for(i = 0; i < noString); i++){
    pinMode(stringTriggered[i], INPUT);
  }
  myServo.attach(servoPin);
  pinMode(LEDpin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  curFrequency = analogRead(inputPin); //read current frequency from sampler
  Serial.println(curFrequency);
  
  ///find which string to tune
  int i = 0;
  for(i = 0; i < noString; i++){
    if(int(digitalRead(stringTriggered[i])) == 1){ //use pulldown resistor
      string = stringTriggered[i] - 1;
      break;
    }
    else{
      ;
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
  
  if(string != prevString){
    doneTuning = 0;
  }
  
  if(!doneTuning && abs(curFrequency - refFrequency) >= margin){
    tuneString();
  }

  digitalWrite(LEDpin , doneTuning); //LED lights up if doneTuning == 1; goes dark if doneTuning == 0;
  prevString = string;
  
}

void tuneString(void){
  
  float diff = refFrequency - curFrequency; //difference between measured frequency and desire freqnecy
  
  //tunning
  if(diff < -(margin)){ //tune up
    myServo.write(180);
    delay(-20*diff);
    myServo.writeMicroseconds(1500);
  }
  else if(diff > margin){ //tune down
    myServo.write(0);
    delay(20*diff);
    myServo.writeMicroseconds(1500);
  }
  else{ //tunned
    doneTuning = 1;
  }
}
