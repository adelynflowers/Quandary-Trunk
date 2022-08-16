#include <Keypad.h>


/**********************************************************************
 * PINS
 **********************************************************************/
#define outputDevice 13
#define inputSound 12
#define correctSound 11
#define wrongSound 10

const byte ROWS = 4; 
const byte COLS = 3; 
byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3}; 

/**********************************************************************
 * KEYPAD SETUP
 **********************************************************************/
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

/**********************************************************************
 * GLOBALS
 * correct - Array containing correct combination
 * pressed - Array containing the current combination
 * count - Tracks number of presses for writing into pressed array
 * SOUND_DELAY - Milliseconds delay for playing sounds
 **********************************************************************/
char correct[4] = {'1','2','3','4'};
char pressed[4] = {'1','1','1','1'};
int count = 0;
#define SOUND_DELAY 200


/**********************************************************************
 * RESET SEQUENCE
 * Loops indefinitely until the '1' key is pressed 4 times
 **********************************************************************/
void resetSequence() {
  int resetCount = 0;
  while (resetCount < 4) {
    char customKey = customKeypad.getKey();
    if (customKey && customKey == '1'){
      resetCount++;
    }
  }
}
/**********************************************************************
 * CHECK WIN
 * Checks if the correct array matches the pressed array
 **********************************************************************/
bool checkWin() {
  for (int i = 0; i < 4; i++) {
    if (correct[i] != pressed[i])
      return false;
  }
  return true;
}

/**********************************************************************
 * PLAY SOUND
 * Plays a sound for SOUND_DELAY milliseconds
 **********************************************************************/
void playSound(int soundPin) {
  digitalWrite(soundPin, HIGH);
  delay(SOUND_DELAY);
  digitalWrite(soundPin, LOW);
}

/**********************************************************************
 * SETUP
 * Initialize the pins as needed
 **********************************************************************/
void setup(){
  pinMode(outputDevice, OUTPUT);
  digitalWrite(outputDevice, HIGH);
  
  pinMode(inputSound, OUTPUT);
  digitalWrite(inputSound, LOW);
  
  pinMode(correctSound, OUTPUT);
  digitalWrite(correctSound, LOW);
  
  pinMode(wrongSound, OUTPUT);
  digitalWrite(wrongSound, LOW);
}
/**********************************************************************
 * LOOP
 * The general game loop
 **********************************************************************/
void loop(){
  //Get the current key
  char customKey = customKeypad.getKey();

  //If it's pressed, add it to the array and play a sound
  if (customKey){
    pressed[count] = customKey;
    playSound(inputSound);
  }

  //If 4 buttons are pressed, check if the game is won
  if (count == 4) {
    //If the game is won, play the win sound, disable the maglock and start the
    //reset sequence
    //If the game is lost, play the wrong sound and reset the count to 0
    if (checkWin()) { 
      playSound(correctSound);
      digitalWrite(outputDevice, LOW);
      resetSequence();
    }
    else {
      playSound(incorrectSound);
      count = 0;
    }
  }
}
