#include "Timer.h"

bool periodicMode = false;

const char ACCEL = 'A';
const char TEMP = 'T';
const char PROX = 'P';
const char COMP = 'C';

int seq = 0;

bool accelActivated = false;
bool tempActivated = false;
bool proxActivated = false;
bool compActivated = false;

String directionCode = "L000R000";
String returns;
String userInput;

String sensorRequests = "";

int accelDelay = 5000;
int tempDelay = 5000;
int proxDelay = 5000;
int compDelay = 5000;

Timer timer;
Timer timer2;
Timer timer3;
Timer timer4;

int tempTime;

void setup()
{
  timer.every(accelDelay, requestAccel);
  timer2.every(tempDelay, requestTemp);
  timer3.every(proxDelay, requestProx);
  timer4.every(compDelay, requestComp);

  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial1.begin(9600);
  Serial1.setTimeout(10);
}

void printCommands()
{
  //not all commands are shown
  Serial.println("LIST OF COMMANDS");
  Serial.println("------------------------------------------------");
  Serial.println("TEMP                                - Toggles tempurature data");
  Serial.println("PROX                                - Toggles proximity data");
  Serial.println("COMP                                - Toggles compass data");
  Serial.println("ACCEL                               - Toggles accelerometer data");
  Serial.println("GET SENSOR                          - Gets sensor data requested");
  Serial.println("RATE NUM                            - Sets sampling rate to NUM");
  Serial.println("THRESH SENSORCODE (A, T, C, P) NUM  - Sets SENSOR threshold to NUM");
  Serial.println("PINMODE PIN MODE STATUS             -Sets pin PIN to mode MODE and status STATUS");
}

void toggleSensor(char sensor)
{
  switch (sensor) 
  {
  case 'A':
    accelActivated = !accelActivated;
    break;
  case 'T':
    tempActivated = !tempActivated;
    break;
  case 'P':
    proxActivated = !proxActivated;
    break;
  case 'C':
    compActivated = !compActivated;
    break;
  }
}

void drive()
{
  Serial1.print(directionCode);
}

void steerCar(char direction) 
{
  if (direction == 'W') 
  {
    directionCode = "L255R255";
  }
  if (direction == 'D')
  {
    directionCode = "L255R000";
  }
  if (direction == 'A')
  {
    directionCode = "L000R255";
  }
  if (direction == 'S')
  {
    directionCode = "L000R000";
  }
  if (direction == 'Q')
  {
    directionCode = "L130R255";
  }
  if (direction == 'E')
  {
    directionCode = "L255R130";
  }
  drive();
}

void setThresh(String input) 
{
    input = input.substring(10);
    periodicTask(input);
}

void setDelay(String sensor, int delayTime)
{
   if(sensor.equalsIgnoreCase("A"))
   {
      accelDelay = delayTime;
      timer.every(accelDelay, requestAccel);
   }
   else if(sensor.equalsIgnoreCase("T"))
   {
      tempDelay = delayTime;
     
      timer2.every(tempDelay, requestTemp);
   }
   else if(sensor.equalsIgnoreCase("P"))
   {
      proxDelay = delayTime;
      timer3.every(proxDelay, requestProx);
   }
   else if(sensor.equalsIgnoreCase("C"))
   {
      compDelay = delayTime;
      timer4.every(compDelay, requestComp);
   }
}

void setPinModeStatus(String pinNumber, String thisPinMode, String pinStatus)
{
  String toSend = "M";
  toSend += pinNumber;
  if(thisPinMode.equalsIgnoreCase("INPUT"))
  {
    toSend += "I";  
  }
  else
  {
    toSend += "O";  
  }
  if(pinStatus.equalsIgnoreCase("HIGH"))
  {
    toSend += "H";  
  }
  else
  {
    toSend += "L";  
  }
  Serial1.print(toSend);
}

void parseInput(String input)
{
  if (input.equalsIgnoreCase("commands"))
  {
    printCommands();
  }
  else if (input.equalsIgnoreCase("temp"))
  {
    toggleSensor('T');
  }
  else if (input.equalsIgnoreCase("prox"))
  {
    toggleSensor('P');
  }
  else if (input.equalsIgnoreCase("comp"))
  {
    toggleSensor('C');
  }
  else if (input.equalsIgnoreCase("accel"))
  {
    toggleSensor('A');
  }
  else if (input.equalsIgnoreCase("w"))
  {
    steerCar('W');
  }
  else if (input.equalsIgnoreCase("d"))
  {
    steerCar('D');
  }
  else if (input.equalsIgnoreCase("a"))
  {
    steerCar('A');
  }
  else if (input.equalsIgnoreCase("s"))
  {
    steerCar('S');
  }
  else if (input.equalsIgnoreCase("q"))
  {
    steerCar('Q');
  }
  else if (input.equalsIgnoreCase("e"))
  {
    steerCar('E');
  }
  else if (input.length() >= 9 && input.substring(0, 9).equalsIgnoreCase("SETTHRESH")) 
  {
    setThresh(input);
  }
  else if (input.length() > 4 && input.substring(0, 3).equalsIgnoreCase("get")) 
  {
    periodicTask(input.substring(4, 5));
  }
  else if (input.equalsIgnoreCase("mode periodic")) 
  {
    periodicMode = true;
  }
  else if (input.equalsIgnoreCase("mode demand")) 
  {
    periodicMode = false;
  }
  else if(input.length() > 7 && input.substring(0, 5).equalsIgnoreCase("DELAY"))
  {
    String sensor = input.substring(6, 7);
    String sensorDelay = input.substring(input.indexOf(" ", 6) + 1);
    int delayTime = sensorDelay.toInt();
    setDelay(sensor, delayTime);
  }
  else if(input.length() > 7 && input.substring(0, 7).equalsIgnoreCase("PINMODE"))
  {
    int breakPoint = input.indexOf(" ", 7) + 1;
    String strPinNumber = input.substring(input.indexOf(" ", 7) + 1, input.indexOf(" ", breakPoint));
    int inputIndex = input.indexOf("input");
    int outputIndex = input.indexOf("output");
    String thisPinMode = (inputIndex > 0) ? input.substring(inputIndex, inputIndex + 5) : input.substring(outputIndex, outputIndex + 6);
    int highIndex = input.indexOf("high");
    int lowIndex = input.indexOf("low");
    String pinStatus = (highIndex > 0) ? input.substring(highIndex, highIndex + 4) : input.substring(lowIndex, lowIndex + 3);
    setPinModeStatus(strPinNumber, thisPinMode, pinStatus);
  }
}

void checkForInput()
{
  //read input
  while (Serial.available())
  {
    delay(10);
    if (Serial.available() > 0)
    {
      char c = Serial.read();
      userInput += c;
    }
  }

  //parse
  if (userInput.length() > 0)
  {
    parseInput(userInput);
    userInput = "";
  }
}

String parseReturns(String returns)
{
  
  int tempIndex = returns.indexOf("T");
  int proxIndex = returns.indexOf("P");
  int compIndex = returns.indexOf("C");
  int accelIndex = returns.indexOf("Q");
  String formatted;

 if(periodicMode)
 {
  formatted += "~~~~PERIODICAL MODE~~~~\n" ;
 }
 else
 {
  formatted += "~~~~ON DEMAND MODE~~~~\n";
  }
  
  if (proxIndex >= 0)
  {
    formatted += "Proximity: " + returns.substring(proxIndex + 1, returns.indexOf(" ", proxIndex)) + "\n";
  }
  if (compIndex >= 0)
  {
    formatted += "Compass: " + returns.substring(compIndex + 1, returns.indexOf(" ", compIndex)) + "\n";
  }
  if (accelIndex >= 0)
  {
    formatted += "Accelerometer: " + returns.substring(accelIndex + 1, returns.indexOf(" ", accelIndex)) + "\n";
  }
  if (tempIndex >= 0)
  {
    formatted += "Temperature: " + returns.substring(tempIndex + 1, returns.indexOf(" ", tempIndex)) + "\n";
  }
  if(returns.equalsIgnoreCase("OK"))
  {
    formatted = "Threshold set";
  }
  return formatted;
}

void periodicTask(String sensor)
{
  Serial1.print(directionCode + sensor);
  
  //wait for response
  while (Serial1.available() == 0) {}
  
  //read response
  while (Serial1.available())
  {
    delay(10);
    if (Serial1.available() >0)
    {
      char c = Serial1.read();
      returns += c;
    }
  }

  if (returns.length() > 0)
  {
    String formatted = parseReturns(returns);
    if (formatted.length() != 0)
    {
      Serial.println(formatted);
    }
    returns = "";
  }
}

void requestAccel()
{
  if (accelActivated)
  {
    sensorRequests += "Q";
  }
}

void requestTemp()
{
  if (tempActivated)
  {
    sensorRequests += "T";
  }
}

void requestProx()
{
  if (proxActivated)
  {
    sensorRequests += "P";
  }
}

void requestComp()
{
  if (compActivated)
  {
    sensorRequests += "C";
  }
}

void loop()
{
  if(periodicMode)
  {
    timer.update();
    timer2.update();
    timer3.update();
    timer4.update();
    delay(100);
    if(sensorRequests.length() > 0)
    {
      periodicTask(sensorRequests);
      sensorRequests = "";  
    }
  }
  checkForInput();
}

