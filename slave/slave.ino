
const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into
#include <Adafruit_LSM303_U.h>
#include <Adafruit_Sensor.h>
# include "RangeSensor.h"
#include <Wire.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(12345);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

int rangePin = 6;
int tempPin = A1;
int tempVolts = 5.0; // change to 3.3 if using 3.3 volts
int leftWheel = 7;
int rightWheel = 8;

Ultrasonic ultrasonic(rangePin);

static double accelXVal = 0;
static double accelYVal = 0;
static double accelZVal = 0;

static int accelXThresh = 100;
static int accelYThresh = 100;
static int accelZThresh = 100;

static double magXVal = 0;
static double magYVal = 0;
static double magZVal = 0;

static int magXThresh = 100;
static int magYThresh = 100;
static int magZThresh = 100;

static double tempVal = 0;
static int tempThresh = 100;

static double proxVal = 0;
static int proxThresh = 6;

static int leftPower = 0;
static int rightPower = 0;

static int threshVal = 0;

boolean aThresholdSet = false;

boolean setPin = false;

void setup() {
  
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(leftWheel, OUTPUT);
  pinMode(rightWheel, OUTPUT);
  pinMode(tempPin, INPUT);
  pinMode(rangePin, INPUT);

  mag.enableAutoRange(true);

  if(!accel.begin() && !mag.begin())
  {
    while(1);
  }

  // initializing variables
  sensors_event_t event; 
  mag.getEvent(&event);    
  magXVal = event.magnetic.x;
  magYVal = event.magnetic.y;
  magZVal = event.magnetic.z; 

  tempVal = getTempData();

  accel.getEvent(&event);
  accelXVal = event.acceleration.x;
  accelYVal = event.acceleration.y;
  accelZVal = event.acceleration.z;

  proxVal = getRangeData();
}

void loop() {
  String inDataStr;
  // see if there's incoming data from XBee
  if (Serial.available() > 0) 
  {
    inDataStr = getXbeeData();
  }

  // check to see if there was data read in
  if(inDataStr.length() > 0) 
  {
    // make a new outDataStr
    String outDataStr = "";
    setPin = false;
    //do pinmode and status
    if (inDataStr.indexOf('M') >= 0) 
    {
      setPin = true;
      int pin = 0;
      int mode = 0;
      int volts = 0;
      if (inDataStr.indexOf('A') >= 0) 
      {
        pin = getValue(inDataStr.substring(inDataStr.indexOf('A') + 1));
      } 
      else 
      {
        pin = getValue(inDataStr.substring(inDataStr.indexOf('M') + 1));
      }
      
      if (inDataStr.indexOf('I') >= 0) 
      {
        mode = INPUT;
      }
      if (inDataStr.indexOf('O') >= 0)
      {
        mode = OUTPUT;
      }
      if (inDataStr.indexOf('L') >= 0)
      {
        volts = LOW;
      }
      if (inDataStr.indexOf('H') >= 0)
      {
        volts = HIGH;
      }
      pinMode(pin, mode);
      digitalWrite(pin, volts);
    }

    // do Compass
    if (inDataStr.indexOf('C') >= 0 || isCompassOver()) 
    {
      // check if we are setting the threshold
      threshVal =  getValue(inDataStr.substring(inDataStr.indexOf("CX") + 2));
      
      // set thresholds if value != 0
      if (threshVal != 0) 
      {
          magXVal = threshVal;
          threshVal =  getValue(inDataStr.substring(inDataStr.indexOf("CY") + 2));
          magYVal = threshVal;
          threshVal =  getValue(inDataStr.substring(inDataStr.indexOf("CZ") + 2));
          magZVal = threshVal;
          aThresholdSet = true;
      }
      sensors_event_t event; 
      mag.getEvent(&event);
        
      magXVal = event.magnetic.x;
      magYVal = event.magnetic.y;
      magZVal = event.magnetic.z;
        
      String xData = "CX:" + (String)magXVal;
      if (magXVal > magXThresh) 
      {
        xData += "ALARM";
      }
      String yData = "Y:" + (String)magYVal;
      if (magYVal > magYThresh) 
      {
        yData += "ALARM";
      }
      String zData = "Z:" + (String)magZVal;
      if (magZVal > magZThresh) 
      {
        zData += "ALARM";
      }
      outDataStr += xData + yData + zData + " ";
    }

    // do tempurature 
    if (inDataStr.indexOf('T') >= 0 || getTempData() > tempThresh) 
    {

      // get a value for threshold
      threshVal = getValue(inDataStr.substring(inDataStr.indexOf('T') + 1));

      // check if we are setting the threshold
      if(threshVal != 0) {
        tempThresh = threshVal;
        aThresholdSet = true;
      }
      tempVal = getTempData();
      outDataStr += "T";
      outDataStr += tempVal;
      if (tempVal > tempThresh) 
      {
        outDataStr += "ALARM";
      }
      outDataStr += " ";
    }

    if (inDataStr.indexOf('Q') >= 0 || isAccelOver())
    {
      threshVal =  getValue(inDataStr.substring(inDataStr.indexOf("AX") + 2));
      if (threshVal != 0) 
      {
          accelXThresh = threshVal;
          threshVal =  getValue(inDataStr.substring(inDataStr.indexOf("AY") + 2));
          accelYThresh = threshVal;
          threshVal =  getValue(inDataStr.substring(inDataStr.indexOf("AZ") + 2));
          accelZThresh = threshVal;
          aThresholdSet = true;
      }
      sensors_event_t event; 
      accel.getEvent(&event);

      accelXVal = event.acceleration.x;
      accelYVal = event.acceleration.y;
      accelZVal = event.acceleration.z;
        
      String xData = "QX:" + (String)accelXVal;
      if (accelXVal  > accelXThresh) 
      {
        xData += "ALARM";
      }
      String yData = "Y:" + (String)accelYVal;
      if (accelYVal  > accelYThresh) 
      {
        yData += "ALARM";
      }
      String zData = "Z:" + (String)accelZVal;
      if (accelZVal  > accelZThresh) 
      {
        zData += "ALARM";
      }
      outDataStr += xData + yData + zData + " ";
    }
    if (inDataStr.indexOf('P') >= 0 || getRangeData() < proxThresh)
    {
      threshVal = getValue(inDataStr.substring(inDataStr.indexOf('P') + 1));
      if(threshVal > 0) {
        proxThresh = threshVal;
        aThresholdSet = true;
      }
      proxVal = getRangeData();
      outDataStr += "P";
      outDataStr += proxVal;
      if (proxVal < proxThresh) 
      {
        outDataStr += "ALARM";
      }
      outDataStr += " ";
    }
  
    if (inDataStr.indexOf("L") >= 0) 
    {
      leftPower = getValue(inDataStr.substring(inDataStr.indexOf("L") + 1));
    }

    if (inDataStr.indexOf("R") >= 0)
    {
      rightPower = getValue(inDataStr.substring(inDataStr.indexOf("R") + 1));
    }

    if ((getRangeData() < proxThresh && rightPower == 0) || getRangeData() > proxThresh) {
      analogWrite(leftWheel, leftPower);
    }
    if ((getRangeData() > proxThresh && leftPower == 0) || getRangeData() > proxThresh) {
      analogWrite(rightWheel, rightPower);
    }
    if (!aThresholdSet && outDataStr.length() > 0 && !setPin) 
    {
      Serial.print(outDataStr);
    }
    if (aThresholdSet) {
      Serial.print("OK");
      aThresholdSet = false;
    }
  }
  
  if (getRangeData() < proxThresh) {
      if (leftPower > 0 && rightPower > 0) {
        leftPower = 0;
        rightPower = 0;
        analogWrite(leftWheel, 0);
        analogWrite(rightWheel, 0);
      }
  }
  delay(50);
}

// given a string find the first integer in that string
int getValue(String inDataSubStr)
{
  String valueStr;
  int index = 0;
  while(isdigit(inDataSubStr[index])) 
  {
    valueStr += inDataSubStr[index];
    index++;
  }
  return valueStr.toInt();
}

String getXbeeData() 
{
  String xBeeDataStr;
  while(Serial.available() > 0) 
  {
    xBeeDataStr += (char)Serial.read();
  }
  return xBeeDataStr;
}


float getTempData()
{
  float sensorValue = analogRead(tempPin) * 5.0;
  float tempC =  ((sensorValue/1024.0) - 0.5) * 100;
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  return tempF;
}

long getRangeData()
{
  ultrasonic.DistanceMeasure();// get the current signal time;
  return ultrasonic.microsecondsToInches();
}

boolean isCompassOver() 
{
  sensors_event_t event;
  mag.getEvent(&event);
  return (event.magnetic.x > magXThresh) || (event.magnetic.y > magYThresh) || (event.magnetic.y > magZThresh);
}

boolean isAccelOver() 
{
  sensors_event_t event; 
  accel.getEvent(&event);
  return (event.acceleration.x > accelXThresh) || (event.acceleration.y > accelYThresh) || (event.acceleration.z > accelZThresh);
}


