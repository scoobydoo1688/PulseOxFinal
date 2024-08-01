#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h> // include statements

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif // for bluetooth


#define FACTORYRESET_ENABLE         0 //has to be zero!
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "BLEUART" // define for BLE

// No other Address options.
#define DEF_ADDR 0x55

// Reset pin, MFIO pin
const int resPin = 10; // set pins
const int mfioPin = 9;

int heartRate;
int oxygen;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;  

Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN); // hardware serial 

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
} // for errors

void setup(){

  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command Mode Example"));
  Serial.println(F("---------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  // while (! ble.isConnected()) {
  //     delay(500);
  // }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("******************************"));
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
    Serial.println(F("******************************"));
  }

  Wire.begin();
  int result = bioHub.begin();
  if (!result)
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");

  Serial.println("Configuring Sensor...."); 
  int error = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 
  if(!error){
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }
  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  delay(4000); 

}

void loop(){

  // Information from the readBpm function will be saved to our "body"
  // variable.  
  body = bioHub.readBpm();
  Serial.print("Heartrate: ");
  Serial.println(body.heartRate); 
  Serial.print("Confidence: ");
  Serial.println(body.confidence); 
  Serial.print("Oxygen: ");
  Serial.println(body.oxygen); 
  Serial.print("Status: ");
  Serial.println(body.status); 
  Serial.print("Extended Status: ");
  Serial.println(body.extStatus); 
  Serial.print("Blood Oxygen R value: ");
  Serial.println(body.rValue); 
  delay(2000); // Slowing it down, we don't need to break our necks here.

    // Check for user input
  char inputs[BUFSIZE+1];

  // modified for project
  Serial.print("[Send] Heartrate: ");
  Serial.println(body.heartRate); 

  ble.print("AT+BLEUARTTX=");
  ble.print("Heartrate: ");
  ble.println(body.heartRate);
  
  ble.print("AT+BLEUARTTX=");
  ble.print("Confidence: ");
  ble.println(body.confidence);
  
  ble.print("AT+BLEUARTTX=");
  ble.print("Oxygen: ");
  ble.println(body.oxygen);

  // check response stastus
  if (! ble.waitForOK() ) {
    Serial.println(F("Failed to send?"));
  
  }

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
