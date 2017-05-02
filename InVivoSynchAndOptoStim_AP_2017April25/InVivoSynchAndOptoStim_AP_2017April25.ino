/* This code combines two parallel processes. It runs a continuous prgram that 
 *  generates pseudo-random pulses to allow for synchronization of data between
 *  two channels. It also runs an optical stimulation protocol that assigns 
 *  three different opto-stim paradigms to three different buttons allowing 
 *  the user to toggle between them. 
 */

// Set pins
const int buttonPin1 = 3;             // button assigned to the first opto-tag protocol
const int buttonPin2 = 5;             // button assigned to the second opto-tag protocol
const int buttonPin3 = 7;             // button assigned to the third opto-tag protocol

const int ledPin1 =  A0;              // the number of the LED pin corresponding to opto-tag protocol one
const int ledPin2 =  A2;              // the number of the LED pin corresponding to opto-tag protocol two
const int ledPin3 =  A4;              // the number of the LED pin corresponding to opto-tag protocol three
const int optostimPin = A14;          // output pin for opto-tagging

const int ANALOG_PIN = A11;           // output pin for synchronizer
const int SIGNAL_PIN = LED_BUILTIN;   // built in LED display shows synchronizer pulses live time

const int pulseLength = 60; 

// Variables
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int programState = 0;

unsigned long timeCurrentOpto;        // stores current time for opto-tagging protocol
unsigned long timeDeltaOpto;          // stores the time difference between current time and time LED was turned on
boolean LEDstateOpto;                 // declares an array of zeroes indicating all groups of LEDs are off
unsigned long intervalStart;          // declares an array of zeroes to hold the time at which the last interval began

unsigned long startTimeSynch = millis();
unsigned long currentTimeSynch = 0; 
unsigned long elapsedTimeSynch = 0; 
int index = 0; 

int Q = 1;
unsigned long previousMillisSynch = 0; 
unsigned long previousMillisHighFreq = 0; 
int ledStateSynch = LOW; 

void setup() {
  // initialize the pins used in synchronizer:
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  pinMode(ANALOG_PIN, OUTPUT);


  // initialize the LED pin as an output:
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(optostimPin, OUTPUT);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
}

// Start the synchronizer program which will run continuously as long as the Arduino is on
void syncPulser() {

  String bin_string = int_to_bin_str(Q);          // generates binary representation of integer value Q
  currentTimeSynch = millis(); 
  elapsedTimeSynch = currentTimeSynch - startTimeSynch; 
  index = (int)(elapsedTimeSynch/pulseLength);

  // if the index value is less than 16 then figure out which part of binary pulse to run
  if (index < 16) {       
    if (bin_string[index] == '1') {
      ledStateSynch = HIGH; 
      digitalWrite(SIGNAL_PIN, ledStateSynch); 
      toggle_tone(bin_string[index]);
    }
    else if (bin_string[index] == '0') {
      ledStateSynch = LOW;
      digitalWrite(SIGNAL_PIN, ledStateSynch); 
      noTone(ANALOG_PIN); 
    }
    
  }

  // if the index value is greater than or equal to 16 then the binary pulse
  // portion should be done and there should be one second of silence
  if (index >= 16) {
    ledStateSynch = LOW;
    digitalWrite(SIGNAL_PIN, ledStateSynch); 
    noTone(ANALOG_PIN); 
  }

  // one second on after the second of silence to act as a full stop (indicator 
  // that one period of signal is over)
  if (index >= (15 + (1000/pulseLength))) {
    ledStateSynch = HIGH; 
    digitalWrite(SIGNAL_PIN, ledStateSynch); 
    tone(ANALOG_PIN, 294); 
  }
   
  // if index is greater than one full period (binary plus full stop) then reset
  if (index > (15 + (2000/pulseLength))) {
    startTimeSynch = millis();
    Q += 1; 
  }

}

// Start the Opto-Tag program which will only run once a button is pressed to initiate
// one of the three programs
void optoStimMain() {
  // read the state of the pushbutton value:
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);

  if (buttonState1 == LOW) {
      programState = 1;
  }
  if (buttonState2 == LOW) {
      programState = 2;
  }
  if (buttonState3 == LOW) {
      programState = 3;
  }
  
  if(programState == 1){
    runProcess1();
  }
  if(programState == 2){
    runProcess2();
  }
  if(programState == 3){
    runProcess3();
  }

  syncPulser();
}

// Functions used by main program loops
// Synchronizer Functions: 
String int_to_bin_str(unsigned int n) {
  String sans_zeros = String(n, BIN);             // gives you binary representation of count Q
  int zero_count = 16 - sans_zeros.length();
  String leading_zeros = "";                      // initializes variable 'leading_zeros'
  for(unsigned int i=0; i < zero_count; i++) {    // pads array with zeros such that length is 16
    leading_zeros += "0";
  }
  return leading_zeros + sans_zeros;              // returns binary representation with length 16
}

void toggle_tone(boolean state) {                 
  if(state) {
    int Hz = random(200,300);
    tone(ANALOG_PIN, Hz);                         // assigns random, high frequency value
  }
  else {
    noTone(ANALOG_PIN);
  }
}

// OptoTag Functions:
void runProcess1(){
  analogWrite(ledPin1, 255); 
  analogWrite(ledPin2, 0);
  analogWrite(ledPin3, 0);
  // Process 1:
  optostim(0.5, 10.0);
}

void runProcess2(){
  analogWrite(ledPin2, 255); 
  analogWrite(ledPin1, 0);
  analogWrite(ledPin3, 0);
  // Process 2:
  optostim(0.5, 50.0);
}

void runProcess3(){
  analogWrite(ledPin3, 255); 
  analogWrite(ledPin1, 0);
  analogWrite(ledPin2, 0);
  // Process 3:
  optostim(0.5, 100.0);
}

void optostim(float freq, float width) {
  timeCurrentOpto = millis();                                  // millis() calls the time for the start of each LED stimulation

  int on = width;
  int off = (1000/freq) - width;
  
  timeDeltaOpto = timeCurrentOpto - intervalStart;    // Stores the change in time for the current LED
  if (timeDeltaOpto >= off && LEDstateOpto == 0) { // If the elapsed time has exceeded the time an LED should be off and the LED is off, turn LED on
    analogWrite(optostimPin, 255);                         // Turns LED on
    LEDstateOpto = 1;                                             // Stores LED state as ON
  }
  else if (timeDeltaOpto >= off + on && LEDstateOpto == 1) {  // If the elapsed time has exceeded the time of an LED blink (off-->on) and the LED is on, turn LED off.
    analogWrite(optostimPin, 0);                          // Turns LED off
    LEDstateOpto = 0;
    intervalStart = timeCurrentOpto;
  }
}
