#include <IRremote.h>

#define PIN_IR 13
#define PIN_DETECT 9


IRsend irsend;
void setup()
{
  pinMode(15, OUTPUT);
  pinMode(17, INPUT);
  Serial.begin(9600);
  analogReference(INTERNAL);
}

void loop() {
digitalWrite(15, HIGH);
Serial.println(digitalRead(17));
}
