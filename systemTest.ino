#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h> // include statements

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h" 

#include "BluefruitConfig.h" // include statements

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif // for bluetooth


#define FACTORYRESET_ENABLE         0 // has to be zero!
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "BLEUART" // define for BLE

// No other Address options.
#define DEF_ADDR 0x55

// Reset pin, MFIO pin
const int resPin = 10; // A3
const int mfioPin = 9; //A2

int width = 411;
int samples = 400; // set resolution to 18 bits
int pulseWidthVal;
int sampleVal;
int heartRate;
int oxygen; 

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;

Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN); // hardware serial 

void error(const __FlashStringHelper*err) {
  ble.print("AT+BLEUARTTX=");
  ble.println(err);
  while (1);
} // for errors

void setup(){

  delay(500);

  // Serial.begin(115200);

  /* Initialise the module */
  ble.print("AT+BLEUARTTX=");
  ble.println(F("Initialising the Bluefruit LE module: "));

  // helps debug
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  ble.print("AT+BLEUARTTX=");
  ble.println( F("OK!") );

  // if ( FACTORYRESET_ENABLE )
  // {
  //   /* Perform a factory reset to make sure everything is in a known state */
  //   Serial.println(F("Performing a factory reset: "));
  //   if ( ! ble.factoryReset() ){
  //     error(F("Couldn't factory reset"));
  //   }
  // }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  ble.print("AT+BLEUARTTX=");
  ble.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  ble.print("AT+BLEUARTTX=");
  ble.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  // Serial.println(F("Then Enter characters to send to Bluefruit"));
  // Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  // while (! ble.isConnected()) {
  //     delay(500);
  // }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    ble.print("AT+BLEUARTTX=");
    ble.println(F("******************************"));

    ble.print("AT+BLEUARTTX=");
    ble.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);

    ble.print("AT+BLEUARTTX=");
    ble.println(F("******************************"));
  }

  // start sensor
  Wire.begin();
  int result = bioHub.begin();
  if (!result) {
    ble.print("AT+BLEUARTTX=");
    ble.println("Sensor started!");
  } else {
    ble.print("AT+BLEUARTTX=");
    ble.println("Could not communicate with the sensor!!!");
  }

  ble.print("AT+BLEUARTTX=");
  ble.println("Configuring Sensor...."); 
  int error = bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings. 
  if(!error){
    ble.print("AT+BLEUARTTX=");
    ble.println("Sensor configured.");
  }
  else {
    ble.print("AT+BLEUARTTX=");
    ble.println("Error configuring sensor.");

    ble.print("AT+BLEUARTTX=");
    ble.print("Error: "); 
    ble.println(error); 
  }
  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  delay(4000); 

  error = bioHub.setPulseWidth(width);
  
  if (!error){
      ble.print("AT+BLEUARTTX=");
      ble.println("Pulse Width Set.");
  }
  else {
      ble.print("AT+BLEUARTTX=");
      ble.println("Could not set Pulse Width.");

      ble.print("AT+BLEUARTTX=");
      ble.print("Error: "); 
      ble.println(error); 
  }

  // Check that the pulse width was set. 
  pulseWidthVal = bioHub.readPulseWidth();
  ble.print("AT+BLEUARTTX=");
  ble.print("Pulse Width: ");
  ble.println(pulseWidthVal);

  error = bioHub.setSampleRate(samples);
  if (!error){
      ble.print("AT+BLEUARTTX=");
      ble.println("Sample Rate Set.");
  }
  else {
      ble.print("AT+BLEUARTTX=");
      ble.println("Could not set Sample Rate!");

      ble.print("AT+BLEUARTTX=");
      ble.print("Error: "); 
      ble.println(error); 
  }

  // Check sample rate.
  sampleVal = bioHub.readSampleRate();

  ble.print("AT+BLEUARTTX=");
  ble.print("Sample rate is set to: ");
  ble.println(sampleVal); 

  // Some time to read your settings.
  delay(2000);

}

void loop(){

  // Information from the readBpm function will be saved to our "body"
  // variable.  
  body = bioHub.readSensorBpm();
  // Serial.print("Infrared LED counts: ");
  // Serial.println(body.irLed); 
  // Serial.print("Red LED counts: ");
  // Serial.println(body.redLed); 
  // Serial.print("Heartrate: ");
  // Serial.println(body.heartRate); 
  // Serial.print("Confidence: ");
  // Serial.println(body.confidence); 
  // Serial.print("Blood Oxygen: ");
  // Serial.println(body.oxygen); 
  // Serial.print("Status: ");
  // Serial.println(body.status); 

  // serial monitor not used, so above section is unnecessary
  delay(1000); // Slowing it down

  //   // Check for user input
  // char inputs[BUFSIZE+1];

  // if status is not 3 (finger/wrist detected), data will not print
  if (body.status != 3) {
    ble.print("AT+BLEUARTTX=");
    ble.println("Realign wrist!");

  // print data otherwise
  } else {
    // modified for project
    ble.print("AT+BLEUARTTX=");
    ble.print("Heartrate: ");
    ble.println(body.heartRate);
    delay(10);
    
    ble.print("AT+BLEUARTTX=");
    ble.print("Confidence: ");
    ble.println(body.confidence);
    delay(10);
    
    ble.print("AT+BLEUARTTX=");
    ble.print("Oxygen: ");
    ble.println(body.oxygen);
    delay(10);

  }

  // if user wanted to send String to serial monitor
  
  // // check response stastus
  // if (! ble.waitForOK() ) {
  //   Serial.println(F("Failed to send?"));
  
  // }

  // // Check for incoming characters from Bluefruit
  // ble.println("AT+BLEUARTRX");
  // ble.readline();
  // if (strcmp(ble.buffer, "OK") == 0) {
  //   // no data
  //   return;
  // }
  // // Some data was found, its in the buffer
  // Serial.print(F("[Recv] ")); Serial.println(ble.buffer);
  // ble.waitForOK();

}
