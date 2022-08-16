/**********************************************************************
 * CAPACATIVE TOUCH SETUP
 **********************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>

// Reset Pin is used for I2C or SPI
#define CAP1188_RESET  9

// CS pin is used for software or hardware SPI
#define CAP1188_CS  10

// These are defined for software SPI, for hardware SPI, check your 
// board's SPI pins in the Arduino documentation
#define CAP1188_MOSI  11
#define CAP1188_MISO  12
#define CAP1188_CLK  13

// For I2C, connect SDA to your Arduino's SDA pin, SCL to SCL pin
// On UNO/Duemilanove/etc, SDA == Analog 4, SCL == Analog 5
// On Leonardo/Micro, SDA == Digital 2, SCL == Digital 3
// On Mega/ADK/Due, SDA == Digital 20, SCL == Digital 21

// Use I2C, no reset pin!
Adafruit_CAP1188 cap = Adafruit_CAP1188();

/**********************************************************************
 * PINS
 **********************************************************************/
int pins[4] = {5,6,7,8};
#define outputDevice 9
#define wrongSound 10
#define inputSound 11
#define correctSound 12

/**********************************************************************
 * GLOBALS
 * correct - correct sequence of presses
 * pressed - pressed sequence
 * counter - keeps track of index for pressed
 **********************************************************************/
int correct[4] = {5,6,7,8}; 
int pressed[4] = {0,0,0,0};
int counter = 0;
#define SOUND_DELAY 200

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
 * Returns true if the pressed and correct arrays are equal
 **********************************************************************/
bool checkWin() {
  for (int i = 0; i < 4; i++) {
    if (correct[i] != pressed[i]) {
      return false;
    }
  }
  return true;
}

/**********************************************************************
 * RESET SEQUENCE
 * Infinite loop until all 4 touch sensors are held
 **********************************************************************/
void resetSequence() {
  while(true) {
    int resetCount = 0;
    uint8_t touched = cap.touched();
    if (touched == 0) {
      // No touch detected
      return;
    }
    for (uint8_t i=0; i<8; i++) {
      if (touched & (1 << i)) {
        Serial.print("C"); Serial.print(i+1 / 2); Serial.print("\t");
        resetCount++;
        playSound(inputSound);
      }
    }
    if (resetCount == 4) break;
  }
}

/**********************************************************************
 * SETUP
 * Set up the pins and initial values
 **********************************************************************/
void setup() {
  pinMode(outputDevice, OUTPUT);
  digitalWrite(outputDevice, HIGH);
  
  pinMode(wrongSound, OUTPUT);
  digitalWrite(wrongSound, HIGH);
  
  pinMode(inputSound, OUTPUT);
  digitalWrite(inputSound, HIGH);
  
  pinMode(correctSound, OUTPUT);
  digitalWrite(correctSound, HIGH);

  Serial.begin(9600);
  Serial.println("CAP1188 test!");

  // Initialize the sensor, if using i2c you can pass in the i2c address
  // if (!cap.begin(0x28)) {
  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");

}

/**********************************************************************
 * LOOP
 * General game loop
 **********************************************************************/
void loop() {
  uint8_t touched = cap.touched();
  if (touched == 0) {
    // No touch detected
    return;
  }
  for (uint8_t i=0; i<8; i++) {
    if (touched & (1 << i)) {
      Serial.print("C"); Serial.print((i+1) /2); Serial.println("\t");
      pressed[counter] = (i+1) / 2;
      counter++;
      playSound(inputSound);
    }
  }
  // If we've gotten 4 inputs, check the win state
  if (counter == 4) {
    if (checkWin()) {
      Serial.println("boiiiii");
      playSound(correctSound);
      counter = 0; 
      resetSequence();
    }
    else {
      Serial.println("boy....");
      playSound(wrongSound);
      counter = 0;
    }
  }
}
