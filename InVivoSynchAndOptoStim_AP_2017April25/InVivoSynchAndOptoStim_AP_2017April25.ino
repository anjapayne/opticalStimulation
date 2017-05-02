// constants won't change. They're used here to
// set pin numbers:
const int buttonPin1 = 3;
const int buttonPin2 = 5;
const int buttonPin3 = 7;

const int ledPin1 =  A0;      // the number of the LED pin
const int ledPin2 =  A2;      // the number of the LED pin
const int ledPin3 =  A4;      // the number of the LED pin
const int optostimPin = A14;

// variables will change:
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;

int programState = 0;

/* Stimulation Variables */
unsigned long timeCurrent;                             // Variable for storing the current time
unsigned long timeDelta;                               // Variable for storing the time difference between turning on the LED and timeCurrent
/* LED State Variables */
boolean LEDstate;                          // Declares an array of zeroes indicating all groups of LEDs are off
unsigned long intervalStart;               // Declares an array of zeroes to hold the time at which the last interval began


/// Sync Pulser Variables
// constants won't change:
#define DELAY 60 //60 MS delay between pulses
#define SIGNAL_PIN LED_BUILTIN
#define ANALOG_PIN A11
const long interval = 1000;           // interval at which to blink (milliseconds)

// variables will change:
int Q = 1;
unsigned long previousMillisOn = 0; // will store last time LED was updated
unsigned long previousMillisOff = 0; // will store last time LED was updated
unsigned long previousMillis2 = 0; // will store last time LED was updated

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(optostimPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);


  /// Sync Pulser Setup
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  pinMode(ANALOG_PIN, OUTPUT);
}

void loop() {
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


void runProcess1(){
  analogWrite(ledPin1, 255); 
  analogWrite(ledPin2, 0);
  analogWrite(ledPin3, 0);

  // Process 1 Goes Here
  optostim(0.5, 10.0);
}


void runProcess2(){
  analogWrite(ledPin2, 255); 
  analogWrite(ledPin1, 0);
  analogWrite(ledPin3, 0);

  // Process 2 Goes Here
  optostim(0.5, 50.0);
}


void runProcess3(){
  analogWrite(ledPin3, 255); 
  analogWrite(ledPin1, 0);
  analogWrite(ledPin2, 0);

  // Process 3 Goes Here
  optostim(0.5, 100.0);
}


void optostim(float freq, float width) {
  timeCurrent = millis();                                  // millis() calls the time for the start of each LED stimulation

  int on = width;
  int off = (1000/freq) - width;
  
  timeDelta = timeCurrent - intervalStart;    // Stores the change in time for the current LED
  if (timeDelta >= off && LEDstate == 0) { // If the elapsed time has exceeded the time an LED should be off and the LED is off, turn LED on
    analogWrite(optostimPin, 255);                         // Turns LED on
    LEDstate = 1;                                             // Stores LED state as ON
  }
  else if (timeDelta >= off + on && LEDstate == 1) {  // If the elapsed time has exceeded the time of an LED blink (off-->on) and the LED is on, turn LED off.
    analogWrite(optostimPin, 0);                          // Turns LED off
    LEDstate = 0;
    intervalStart = timeCurrent;
  }
}


void syncPulser() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisOn < 500) {
    pulse(Q);
    Q += 1;  
  }


  if (currentMillis - previousMillisOn >= 4000) {
    // save the last time you blinked the LED
    //previousMillisOn = currentMillisOn; 
    //currentMillisOn = 0;
    digitalWrite(SIGNAL_PIN, LOW);
    tone(ANALOG_PIN, 294);
  }
  //delay(1000);

  
  if (currentMillis - previousMillisOn >= 8000 && currentMillis - previousMillisOn < 12000) {
    // save the last time you blinked the LED
    //currentMillisOff = 0;
    digitalWrite(SIGNAL_PIN, HIGH);
    noTone(ANALOG_PIN); 
  }
  if (currentMillis - previousMillisOn <= 12000){
    previousMillisOn = currentMillis;
  }
  
  digitalWrite(SIGNAL_PIN, LOW);
  noTone(ANALOG_PIN);

  //delay(1000);

}


void pulse(unsigned int n) {
  String bin_string = int_to_bin_str(n);
  unsigned long currentMillis2 = millis();
  
  for(int i = 0; i < bin_string.length(); i++) {
    boolean pin_state = (bin_string[i] == '1');
    digitalWrite(SIGNAL_PIN, pin_state);
    toggle_tone(pin_state);
    //if (currentMillis2 - previousMillis2 >= DELAY) {
      // save the last time you blinked the LED
      //previousMillis2 = currentMillis2; 
    //}
    
    delay(DELAY); //commenting out but note, this may need to be added back in pending verification tests
      
  }
  digitalWrite(SIGNAL_PIN, LOW);
  noTone(ANALOG_PIN);
}

//returns bin string with leading zeros, 16 bit unsigned
String int_to_bin_str(unsigned int n) {
 String sans_zeros = String(n,BIN);
 int zero_count = 16 - sans_zeros.length();
 String leading_zeros = "";
 for(unsigned int i=0; i < zero_count; i++) {
   leading_zeros += "0";
 }
 return leading_zeros + sans_zeros;
}

void toggle_tone(boolean state) {
  if(state) {
    int Hz = random(200,300);
    tone(ANALOG_PIN, Hz);
  }
  else {
    noTone(ANALOG_PIN);
  }
}


