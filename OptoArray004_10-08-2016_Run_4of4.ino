/* Constants */
#define LEDpinstart 2                                 // Pin # that 1st LED driver is connected to
#define numDrivers 8                                  // Total number of LED drivers

/* LED State Variables */
boolean LEDstate[numDrivers];                          // Declares an array of zeroes indicating all groups of LEDs are off
unsigned int pulsesCount[numDrivers];                  // Declares an array of zeroes to count the number of times an LED has blinked within a single burst
unsigned int burstsCount[numDrivers];                  // Declares an array of zeroes to count the number of bursts that have occurred
unsigned long intervalStart[numDrivers];               // Declares an array of zeroes to hold the time at which the last interval began

/* Stimulation Variables */
unsigned long timeCurrent;                             // Variable for storing the current time
unsigned long timeDelta;                               // Variable for storing the time difference between turning on the LED and timeCurrent

// freq = Frequency of signal
// width = Pulse width (ms)
// pulses = # of pulses per burst
// ISI = interstimulus interval per burst (ms)
// bursts = # of bursts
// intensity = LED brightness (0 to 1)

// Group 1
float freq1 = 20.0;
float width1 = 15.0;
float pulses1 = 100;
float ISI1 = 10000.0;
float bursts1 = 100.0;
float intensity1 = 0.3;

// Group 2
float freq2 = 40.0;
float width2 = 15.0;
float pulses2 = 100;
float ISI2 = 10000.0;
float bursts2 = 100.0;
float intensity2 = 0.1;

// Group 3
float freq3 = 10.0;
float width3 = 15.0;
float pulses3 = 100;
float ISI3 = 10000.0;
float bursts3 = 100.0;
float intensity3 = 0.5;

// Group 4
float freq4 = 20.0;
float width4 = 15.0;
float pulses4 = 100;
float ISI4 = 10000.0;
float bursts4 = 100.0;
float intensity4 = 0.3;

// Group 5
float freq5 = 1.0;
float width5 = 15.0;
float pulses5 = 0;
float ISI5 = 10000.0;
float bursts5 = 1.0;
float intensity5 = 0.0;

// Group 6
float freq6 = 30.0;
float width6 = 15.0;
float pulses6 = 100;
float ISI6 = 10000.0;
float bursts6 = 100.0;
float intensity6 = 0.2;

// Group 7
float freq7 = 40.0;
float width7 = 15.0;
float pulses7 = 100;
float ISI7 = 10000.0;
float bursts7 = 100.0;
float intensity7 = 0.1;

// Group 8
float freq8 = 50.0;
float width8 = 15.0;
float pulses8 = 100;
float ISI8 = 10000.0;
float bursts8 = 100.0;
float intensity8 = 0.1;



// LEDon                      // The time duration an LED is on during a pulse (ms)
float LEDon[] = {
  width1, width2, width3, width4, width5, width6, width7, width8
};

// LEDoff                     // The time duration an LED is off during a pulse (ms)
float LEDoff[] = {
  (1000 / freq1) - width1, (1000 / freq2) - width2, (1000 / freq3) - width3, (1000 / freq4) - width4,
  (1000 / freq5) - width5, (1000 / freq6) - width6, (1000 / freq7) - width7, (1000 / freq8) - width8
};

// numPulses                  // Number of pulses in a burst
float numPulses[] = {
  pulses1, pulses2, pulses3, pulses4, pulses5, pulses6, pulses7, pulses8
};

// burstOff                     // The rest time between the end of a burst and the beginning of the next burst (ms)
float burstOff[] = {
  ISI1 - (pulses1 * (1000 / freq1)), ISI2 - (pulses2 * (1000 / freq2)), ISI3 - (pulses3 * (1000 / freq3)), ISI4 - (pulses4 * (1000 / freq4)),
  ISI5 - (pulses5 * (1000 / freq5)), ISI6 - (pulses6 * (1000 / freq6)), ISI7 - (pulses7 * (1000 / freq7)), ISI8 - (pulses8 * (1000 / freq8))
};

// Number of Bursts
float numBursts[] = {
  bursts1, bursts2, bursts3, bursts4, bursts5, bursts6, bursts7, bursts8
};

// LED brightness
float intensity[] = {255 * intensity1, 255 * intensity2, 255 * intensity3, 255 * intensity4,
                     255 * intensity5, 255 * intensity6, 255 * intensity7, 255 * intensity8
                    };


// INITIALIZATION                                                                              // This code runs only once each time the power supply is connected
void setup() {
  for (int LEDpin = LEDpinstart; LEDpin <= LEDpinstart + numDrivers - 1; LEDpin++)
    pinMode (LEDpin, OUTPUT);      // Configures each digital pin as an output
  Serial.begin(57600);           // Sets baud rate (communication rate between computer and Arduino)
}

// MAIN PROGRAM                                                                                // This code will run repeatedly as long as power supply is plugged in
void loop() {
  timeCurrent = millis();                                  // millis() calls the time for the start of each LED stimulation
  for (int i = 0; i < numDrivers; i++) {                   // Iterates over each LED
    timeDelta = timeCurrent - intervalStart[i];    // Stores the change in time for the current LED
    if (burstsCount[i] < numBursts[i]) {           // If the LED hasn't burst the required number of times yet...
      if (pulsesCount[i] < numPulses[i]) {                      // If the LED hasn't blinked the required number of times yet...
        if (timeDelta >= LEDoff[i] && LEDstate[i] == 0) { // If the elapsed time has exceeded the time an LED should be off and the LED is off, turn LED on
          analogWrite(LEDpinstart + i, intensity[i]);                         // Turns LED on
          LEDstate[i] = 1;                                             // Stores LED state as ON
        }
        else if (timeDelta >= LEDoff[i] + LEDon[i] && LEDstate[i] == 1) {  // If the elapsed time has exceeded the time of an LED blink (off-->on) and the LED is on, turn LED off.
          analogWrite(LEDpinstart + i, 0);                          // Turns LED off
          LEDstate[i] = 0;                                             // Stores LED state as OFF
          pulsesCount[i]++;                                            // Increments the number of blinks by 1
          intervalStart[i] = timeCurrent;                              //
        }
      }
      else if (timeDelta >= burstOff[i]) {                                       // Reset for next burst
        analogWrite(LEDpinstart + i, 0);
        LEDstate[i] = 0;
        pulsesCount[i] = 0;                                              // Resets the number of blinks to zero
        burstsCount[i]++;                                                // Increments the number of bursts by 1
        intervalStart[i] = timeCurrent;                                  //
      }
    }
  }
}
