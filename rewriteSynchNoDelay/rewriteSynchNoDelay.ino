// Rewrites original code written by Nick Topper to avoid the use of delays. 

// constants
#define SIGNAL_PIN LED_BUILTIN
unsigned long previousMillis = 0; 
unsigned long previousMillisHighFreq = 0; 
int ledState = LOW; 
int Q = 1; 
int pulseLength = 60; 

// variable
unsigned long startTime = millis();
unsigned long currentTime = 0; 
unsigned long elapsedTime = 0; 
int index = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
}






void loop() {

  String bin_string = int_to_bin_str(Q);          // generates binary representation of integer value Q
  currentTime = millis(); 
  elapsedTime = currentTime - startTime; 
  index = (int)(elapsedTime/pulseLength);

  // if the index value is less than 16 then figure out which part of binary pulse to run
  if (index < 16) {       
    if (bin_string[index] == '1') {
      ledState = HIGH; 
      digitalWrite(SIGNAL_PIN, ledState); 
      toggle_tone(bin_string[i]);
    }
    else if (bin_string[index] == '0') {
      ledState = LOW;
      digitalWrite(SIGNAL_PIN, ledState); 
      noTone(ANALOG_PIN); 
    }
    
  }
  if (index >= 16) {
    ledState = LOW;
    digitalWrite(SIGNAL_PIN, ledState); 
    noTone(ANALOG_PIN); 
  }
  if (index >= (15 + (1000/pulseLength))) {
    ledState = HIGH; 
    digitalWrite(SIGNAL_PIN, ledState); 
    tone(ANALOG_PIN, 294); 
  }
   
  // if index is greater than one full period (binary plus full stop) then reset
  if (index > (15 + (2000/pulseLength))) {
    startTime = millis();
    Q += 1; 
  }

}

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
    tone(ANALOG_PIN, Hz);
  }
  else {
    noTone(ANALOG_PIN);
  }
}
