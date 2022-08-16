/****************************************************************************
 * PINS
 ****************************************************************************/
#define magLock 13
#define swapSound 3
int pins[4] = {4,5,6,7};
int sounds[4] = {11,8,10,9};
//int sounds[4] = {9,10,8,11};
int initialSounds[4] = {11,8,10,9};

/*****************************************************************************
 * GLOBALS
 * startTimes - Tracks when a button began being held
 * elapsed_time - Tracks how long a button has been held
 * correctSounds - Holds the correct sequence of sounds
 * reset - Indicates if we're in the reset sequence
 *****************************************************************************/
unsigned long startTimes[4] = {0,0,0,0};
unsigned long elapsed_time[4] = {0,0,0,0};
int correctSounds[4] = {8,10,9,11};
bool swapFlags[4] = {false,false,false,false};
bool reset = false;

/****************************************************************************
 * CHECK WIN
 * Returns true if the current order of sounds is equal to the correct order
 * of sounds
 ****************************************************************************/
bool checkWin() {
  for (int i = 0; i < 4; i++) {
    if (sounds[i] != correctSounds[i])
      return false;
  }
  return true;
}

/****************************************************************************
 * SWAP SOUNDS
 * Swaps the positions of two sounds 
 ****************************************************************************/
void swapSounds(int index1, int index2) {
  playSound(swapSound);
  int soundA = sounds[index1];
  sounds[index1] = sounds[index2];
  sounds[index2] = soundA;
}

/****************************************************************************
 * PLAY SOUND
 * Plays a sound from the sound board
 ****************************************************************************/
void playSound(int soundPin) {
  //Serial.println("Playing sound");
  digitalWrite(soundPin, LOW);
  delay(200);
  digitalWrite(soundPin, HIGH);
}

/****************************************************************************
 * SETUP
 * Set up pins and Serial
 ****************************************************************************/
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    //Buttons
    pinMode(pins[i], INPUT);
    //Sounds
    pinMode(sounds[i], OUTPUT);
    digitalWrite(sounds[i], HIGH);
  }
  pinMode(swapSound, OUTPUT);
  digitalWrite(swapSound, HIGH);
  pinMode(magLock, OUTPUT);
  digitalWrite(magLock, HIGH);
  Serial.println("End setup");
}

/****************************************************************************
 * DEBUG
 * Useful for tracking button hold times
 ****************************************************************************/
void debug() {
  for (int i = 0; i < 4; i++) {
    Serial.print("Elapsed time for ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(elapsed_time[i]);
  }
  Serial.println(digitalRead(sounds[1]));
  Serial.println("***********************************");
}

/****************************************************************************
 * LOOP
 * 1. Update button hold times
 * 1a. If a button is held then released, play the corresponding sound
 * 2. Find out how many buttons have been held for 2+ seconds
 * 3. If two buttons were held, swap their sounds and erase their hold times
 * 4. If 4 buttons were held..
 * 4a. If reset is true, reset the game
 * 4b. If the sounds are correct, win the game
 ****************************************************************************/
void loop() {
  //1
  //Serial.println("top of loop");
  for (int i = 0; i < 4; i++) {
    if (digitalRead(pins[i]) && startTimes[i] > 0) {
      //Serial.print("Start time: ");
      //Serial.println(startTimes[i]);
      Serial.println("Updating elapsed");
      elapsed_time[i] = millis() - startTimes[i]; 
    }
    else if (digitalRead(pins[i]) && (startTimes[i] == 0) && !(swapFlags[i])) {
      Serial.println("Beginning press");
      startTimes[i] = millis();
    }
    else if (!digitalRead(pins[i]) && elapsed_time[i] > 0) { //1a
      Serial.println("time loop sound");
      playSound(sounds[i]);
      elapsed_time[i] = 0;
      startTimes[i] = 0;
    }
    else if (!digitalRead(pins[i]) && (startTimes[i] == 0) && (swapFlags[i])) {
      Serial.println("Resetting flag");
      swapFlags[i] = false;
    }
  }
  //2
  int hold1, hold2, heldCount = 0;
  for (int i = 0; i < 4; i++) {
    if (elapsed_time[i] > 500 && startTimes[i] != -1) {
      heldCount ? hold1 = i : hold2 = i;
      heldCount++;
    }
  }
  Serial.print("hold count: ");
  Serial.println(heldCount);
 
  //3
  if (heldCount == 2) {
    Serial.println("Swapping sounds");
    swapSounds(hold1, hold2);
    elapsed_time[hold1] = 0;
    elapsed_time[hold2] = 0;
    startTimes[hold1] = 0;
    startTimes[hold2] = 0;
    swapFlags[hold1] = true; 
    swapFlags[hold2] = true;
    Serial.println(startTimes[hold1]);
    Serial.println(startTimes[hold2]);
  }
  if (heldCount == 2 && reset) {
    reset = false;
    digitalWrite(magLock, HIGH);
    playSound(swapSound);
    for (int i = 0; i < 4; i++) {
      elapsed_time[i] = 0;
      startTimes[i] = 0;
      sounds[i] = initialSounds[i];
    }
  }
  else if (checkWin()) { //4b
    Serial.println("Victory");
    //play(winSound);
    digitalWrite(magLock, LOW);
    reset = true;
  }
}
