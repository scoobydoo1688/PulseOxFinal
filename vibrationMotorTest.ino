#include <Adafruit_CircuitPlayground.h>

int motorPin = 10; // A3

void setup()
{
  pinMode(motorPin, OUTPUT); // set pin as output
}

void loop()
{
  analogWrite(10, 255); //vibrate
  delay(2000);  // delay one second
  analogWrite(10, 0);  //stop vibrating
  delay(2000); // wait 2 seconds.
}

