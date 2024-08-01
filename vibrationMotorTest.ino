#include <Adafruit_CircuitPlayground.h>

int motorPin = 10;


void setup()
{
  pinMode(motorPin, OUTPUT);
}

void loop()
{
  analogWrite(10, 255); //vibrate
  delay(2000);  // delay one second
  analogWrite(10, 0);  //stop vibrating
  delay(2000); //wait 50 seconds.
}

