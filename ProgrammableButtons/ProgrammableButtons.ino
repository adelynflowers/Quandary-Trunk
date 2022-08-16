#include <EEPROM.h>

/**********************************************************************
 * PINS
 **********************************************************************/
int pins[4] = {2,3,4,5};
#define wrongSound 10
#define inputSound 11
#define correctSound 12

/**********************************************************************
 * GLOBALS
 * correct - the correct sequence of buttons
 * pressed - the sequence pressed by the user
 * counter - keeps track of which index to write in the pressed array
 * SOUND_DELAY - milliseconds that sounds are played
 **********************************************************************/
int correct[16] = {2,3,4,5,0,0,0,0,0,0,0,0,0,0,0,0}; 
int pressed[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

int counter = 0;
#define SOUND_DELAY 200
int memCount = 0;
unsigned long loopStart = 0;
unsigned long elapsedTime = 0;
unsigned long holdStart = 0;
unsigned long holdTime = 0;


/**********************************************************************
 * REPROGRAM BUTTONS
 * Waits for a 4 button sequence that becomes the new correct sequence,
 * the win sound plays as confirmation
 **********************************************************************/
void reprogramButtons() {
  Serial.println("Reprogramming...");
  for (int i = 0; i < 3; i++) {
    playSound(inputSound);
    delay(500);
  }
  delay(500);
  int count = 0;
  unsigned long startTime = millis();
  unsigned long elapsedTime = 0;
  for (int i = 0; i < 16; i++) {
    correct[i] = 0;
  }
  while (elapsedTime < 5000) {
    elapsedTime = millis() - startTime;
    for (int i = 0; i < 4; i++) {
      if (digitalRead(pins[i])) {
        correct[count] = pins[i];
        playSound(inputSound);
        count++;
        Serial.println("New index recorded");
        elapsedTime = 0;
        startTime = millis();
      }
    }
  }
  Serial.println("Recording in memory");
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.put(0, correct);
  memCount = count;
  Serial.println("Finished Reprogramming");
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
 * Returns true if the correct and pressed arrays are equal
 **********************************************************************/
bool checkWin() {
  for (int i = 0; i < memCount; i++) {
    Serial.print("Checking ");
    Serial.print(pressed[i]);
    Serial.print(" against ");
    Serial.println(correct[i]);
    if (correct[i] != pressed[i]) {
      return false;
    }
  }
  return true;
}

/**********************************************************************
 * RESET SEQUENCE
 * If buttons 1 & 3 are held, the game resets
 * If buttons 2 & 4 are held, the reprogram sequence begins
 **********************************************************************/
void resetSequence() {
  while(true) {
    if (digitalRead(pins[0]) && digitalRead(pins[2])) {
      break;
    }
    if (digitalRead(pins[1]) && digitalRead(pins[3])) {
      reprogramButtons();
      break;
    }
  }
}

/**********************************************************************
 * READ MEMORY
 * Reads byte 0 in arduino EEPROM, which is where any previous custom
 * sequence was stored. If a custom sequence is detected, it writes
 * that to the correct sequence.
 **********************************************************************/
void readMemory() {
  //Read sequence from memory 
  int seq[16];
  Serial.println("Checking memory for sequence");
  EEPROM.get(0, seq);
  Serial.println("Memory sequence:");
  for (int i = 0; i < 16; i++) {
    Serial.print(seq[i]);
    if (seq[i] >= pins[0] && seq[i] <= pins[3]) 
      memCount++;
  }
  Serial.println();
  Serial.println("********************");
  if (memCount > 0) {
    for (int i = 0; i < memCount; i++) {
      correct[i] = seq[i];
    }
  }
}

/**********************************************************************
 * SETUP
 * Initialize pins and initial states
 **********************************************************************/
void setup() {
  Serial.begin(9600);
  readMemory();
  // Setup touch pins
  for (int i=0; i < 4; i++) {
    pinMode(pins[i], INPUT);
  }
  
  
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
  //Count up for time out
  if (loopStart > 0) 
    elapsedTime = millis() - loopStart;

  //Count up for reprogramming holds
     
  //Reprogram if held for 5+ seconds
  if (holdTime > 2000 && !(digitalRead(pins[0]) && !(digitalRead(pins[3])))) {
    Serial.println("Buttons held, reprogramming");
    holdTime = 0;
    holdStart = 0;
    reprogramButtons();
    counter = 0;
  }
  else if (holdStart > 0 && digitalRead(pins[0]) && digitalRead(pins[3])) {
    holdTime = millis() - loopStart;
  }
  else {
    holdTime = 0;
    holdStart = 0;
  }
 
  //Time out if no action for 10 seconds after a push
  if (elapsedTime > 10000) {
    playSound(wrongSound);
    Serial.println("Waited too long inbetween pushes, timed out");
    loopStart = 0;
    elapsedTime = 0;
    counter = 0;
  }
  //Check for held programming buttons
  if (digitalRead(pins[0]) && digitalRead(pins[3]) && (holdStart == 0)) {
    holdStart = millis();
  }
  //Loop through the 4 inputs
  for (int i = 0; i < 4; i++) {
    if (digitalRead(pins[i]) == HIGH) {
      //Dont follow the normal chain of events if we're holding for programming
      if ((i == 0 || i == 3) && (holdStart > 0))
        break;
      playSound(inputSound);
      pressed[counter] = pins[i];
      loopStart = millis();
      counter++;
      Serial.println("Button Pressed");
      delay(200); //Prevent accidental double pressing
    }
  }
  // If we've sequenced n times, we're done
  if (counter == memCount) {
    if (checkWin()) {
      Serial.println("Correct sequence, win!");
      playSound(correctSound);
      counter = 0; 
      //resetSequence();
      Serial.println("Reset finished");
      delay(500);
    }
    else {
      Serial.println("Incorrect sequence, try again");
      playSound(wrongSound);
      counter = 0;
    }
  }
}
