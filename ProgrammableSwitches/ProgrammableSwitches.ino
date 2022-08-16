/*
 * Press button to check inputs
 * Press button after all switches are high to reset
 * Hold button for 2 seconds, switch to desired config, then hold again to reprogram
 */

/**********************************************************************
 * PINS
 **********************************************************************/
int pins[9] = {3,4,5,6,7,8,9,10,11};
#define outputDevice 12
#define wrongSound 13
#define inputSound 14
#define correctSound 15
#define confirmInputs 16

/**********************************************************************
 * GLOBALS
 * correct - the correct position for all 9 switches
 * SOUND_DELAY - sound playing duration
 **********************************************************************/
int correct[9] = {1,1,1,1,1,1,1,1,1};
#define SOUND_DELAY 200

/**********************************************************************
 * CHECK INPUTS
 * 
 **********************************************************************/
bool checkInputs() {
  for (int i = 0; i < 9; i++) {
    if (digitalRead(pins[i]) != correct[i]) {
      return false;
    }
  }
  return true;
}

/**********************************************************************
 * REPROGRAM SWITCHES
 * Plays the win sound to confirm you are in the sequence. Waits
 * for the button to be held for 2 seconds and then writes the current
 * switch states as the correct states. Ends with win sound to confirm
 **********************************************************************/
void reprogramSwitches() {
  playSound(correctSound);
  while (true) {
    if (digitalRead(confirmInputs)) {
      delay(2000);
      if (digitalRead(confirmInputs)) {
        for (int i = 0; i < 9; i++) {
          correct[i] = digitalRead(pins[i]);
        }
      }
      break;
    }
  }
  playSound(correctSound);
}

/**********************************************************************
 * PLAY SOUND
 * Plays a sound for SOUND_DELAY milliseconds
 **********************************************************************/
void playSound(int soundPin) {
  digitalWrite(soundPin, LOW);
  delay(SOUND_DELAY);
  digitalWrite(soundPin, HIGH);
}

/**********************************************************************
 * CHECK WIN
 * Returns true if the current state of the switches matches the correct
 * array
 **********************************************************************/
bool checkWin() {
  for (int i = 0; i < 9; i++) {
    if (correct[i] != digitalRead(pins[i])) {
      return false;
    }
  }
  return true;
}

/**********************************************************************
 * RESET SEQUENCE
 * If all switches are set to high, reset. If the button is held for
 * 2 seconds, begin the reprogramming sequence
 **********************************************************************/
void resetSequence() {
  while(true) {
    bool reset = true;
    //Check for reset break
    for (int i = 0; i < 9; i++) {
      if (digitalRead(pins[i]) != 1) {
        reset = false;
      }
    }
    if (reset) {
      break;
    }
    //Check for reprogram
    if (digitalRead(confirmInputs)) {
      delay(2000);
      if (digitalRead(confirmInputs)) {
        reprogramSwitches();
      }
    }
    
  }
}

/**********************************************************************
 * SETUP
 * Initialize pins and initial states
 **********************************************************************/
void setup() {
  for (int i=0; i < 9; i++) {
    pinMode(pins[i], INPUT);
  }

  pinMode(confirmInputs, INPUT);
  
  // Setup output pin
  pinMode(outputDevice, OUTPUT);
  digitalWrite(outputDevice, LOW);
  
  pinMode(wrongSound, OUTPUT);
  digitalWrite(wrongSound, HIGH);
  
  pinMode(inputSound, OUTPUT);
  digitalWrite(inputSound, HIGH);
  
  pinMode(correctSound, OUTPUT);
  digitalWrite(correctSound, HIGH);
}

/**********************************************************************
 * LOOP
 * General game loop
 **********************************************************************/
void loop() {
  //Loop through the 4 inputs
  if (digitalRead(confirmInputs)) {
    if (checkWin()) {
      playSound(correctSound);
      digitalWrite(outputDevice, HIGH);
      resetSequence();
      digitalWrite(outputDevice, LOW);
    }
    else {
      playSound(wrongSound);
    }
  }
  

  
  

}
