#include <EEPROM.h>

int samplingInterval = -1; // -1 will be my default value
int ultrasonicAlertThreshold = -1;
int ldrAlertThreshold = -1;
bool inMainMenu = true;
bool inSensorSettings = false;
bool inResetLoggerData = false;
bool inSystemStatus = false;
bool inRgbLedControl = false;
int readOption = -1;  // these will be used as temp variables to remember options for submenus (needed as flags too, to remember in what submenu we are)
int readOption2 = -1;

int photocellPin = A0;   // the cell and 10K pulldown are connected to a0
int photocellValue = 0;  // the analog reading from the sensor divider
const int ledPinBlue = 3;
const int ledpinGreen = 5;
const int ledPinRed = 6;
int blueLedValue = -1;
int greenLedValue = -1;
int redLedValue = -1;

const int trigPin = 9;
const int echoPin = 10;
long duration = 0;
long distance = 0;
unsigned long int prevTimeSampleTaken = 0;
unsigned long int prevTimeTrigger = 0;    // tried to make ultrasonic code using micros instead delaymicros, I commented the function, but you can check if it works, for me this sensor is not reliable.

bool allowPrintSensorValues = false;   // flag used for printing sensor's values at a set sample rate

const int triggerDelayHIGH = 12; //also used at ultrasonic code
const int triggerDelayLOW = 2;


const int numberOfSavedLogs = 10;

bool completeCycle = true; // used at ultrasonic code

int currentSavedLog = 0; //iterator for logs

bool ledAutomaticMode = false; // flag for toggle automatic mod 

const int colors = 3; 
int rgbColor[colors] = { 0, 0, 0 }; // array used to set led rgb value for manual mode
int rgbColorPins[colors] = { 3, 5, 6 }; //also the pins
int currentCollor = 0; // iterator for manual mode

const int minSampleRate = 1; //some variables to not have magic numbers
const int maxSampleRate = 10;
const int minimumAlertThreshold = 1;

const int doubleBytes = 8;
const int longBytes = 4;
const int intBytes = 2;
const int maxLedLuminosity = 255;
const int minLedLuminosity = 0;
const int totalResetBytes = (longBytes + intBytes) * numberOfSavedLogs;  //number of total bytes that have to be reseted for 10 logs of long and 10 logs of int

const int inputThatStopsReadings = 6;

double sumLuminosity = 0;
double sumDistance = 0;
const int averageLuminosityAdress = 100;
const int averageDistanceAdress = averageLuminosityAdress + doubleBytes;
unsigned long int counter = 0;




void setup() {
  pinMode(ledPinBlue, OUTPUT);
  pinMode(ledpinGreen, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
  Serial.begin(9600);
  printMainMenu();
}

void loop() {
  handleInputAndMenuOperations();     // used for read serial input and also for navigating between menus and submenus using set flags
  ultrasonicSensorReading();          // function for ultrasonic sensor from lab
  ldrSensorReading();                 // function for ldr sensor from lab
  printReadings();                    // function that print sensor's values and put them in EEPROM 
  ledBehavior();                      // function used to check led's flags, like for example if the toggle for auto mode is on/off and to send values to led using analog write
}

void handleInputAndMenuOperations() {
  if (Serial.available()) {
    int read = Serial.parseInt();
    if (inMainMenu == true) {
      mainMenu(read);
    } else {
      if (inSensorSettings == true && readOption == -1 && readOption2 == -1) {       // the trick is read -> save option -> read -> save option -> read (and this is just for 3 submenus, imagine 10, not the brightest idea, but it works)
        sensorSettings(read, readOption, readOption);
      } else if (inSensorSettings == true && readOption != -1 && readOption2 == -1) {
        sensorSettings(readOption, read, readOption2);
      } else if (inSensorSettings == true && readOption != -1 && readOption2 != -1) {
        sensorSettings(readOption, readOption2, read);
      }
      if (inResetLoggerData == true && readOption == -1) {
        resetLoggerData(read, readOption);
      } else if (inResetLoggerData == true && readOption != -1) {
        resetLoggerData(readOption, read);
      }
      if (inSystemStatus == true && allowPrintSensorValues == false) {
        systemStatus(read);
      } else if (inSystemStatus == true && allowPrintSensorValues == true) { // you have to input the set value (6) that will toggle the flag for reading and printing sensor values. Other value, but 5, will not work.
        if (read == inputThatStopsReadings) {                             //   YOHOOOO -1 MAGIC WORDS ! LETS F**** GOOO !!!!
          systemStatus(read);
        }
      }
      if (inRgbLedControl == true && readOption == -1) {
        rgbLedControl(read, readOption);
      } else if (inRgbLedControl == true && readOption != -1) {
        rgbLedControl(readOption, read);
      }
    }
  }
}

//function that just prints a warning if the user's input is not one of the options from menu/submenus
void handleError() {   
  Serial.println();
  Serial.println(F("Warning! The selected option is invalid !"));
}

void printMainMenu() {
  Serial.println();
  Serial.println(F("Main Menu:"));
  Serial.println(F("1.Sensor Settings"));
  Serial.println(F("2.Reset logger data"));
  Serial.println(F("3.System Status"));
  Serial.println(F("4.RGB LED Control"));
}

void mainMenu(int option) {
  switch (option) {
    case 1:
      inMainMenu = false;             //example of flags that are set
      inSensorSettings = true;
      printSensorSettings();
      break;
    case 2:
      inMainMenu = false;
      inResetLoggerData = true;
      printResetLoggerData();
      break;
    case 3:
      inMainMenu = false;
      inSystemStatus = true;
      printSystemStatus();
      break;
    case 4:
      inMainMenu = false;
      inRgbLedControl = true;
      printRgbLedControl();
      break;
    default:
      handleError();
      printMainMenu();
      break;
  }
}

void printSensorSettings() {
  Serial.println();
  Serial.println(F("1.Sensor Settings:"));
  Serial.println(F("  1.1.Sensor Sampling Interval"));
  Serial.println(F("  1.2.Ultrasonic Alert Threshold"));
  Serial.println(F("  1.3.LDR Alert Threshold"));
  Serial.println(F("  1.4.Reset Sensor Settings"));
  Serial.println(F("  1.5.Back "));
}

void printSensorSettingsResetMenu() {
  Serial.println();
  Serial.println(F("  1.4.1.Reset Sampling Interval"));
  Serial.println(F("  1.4.2.Reset Ultrasonic Alert Threshold"));
  Serial.println(F("  1.4.3.Reset LDR Alert Threshold"));
  Serial.println(F("  1.4.4.RESET ALL"));
  Serial.println(F("  1.4.5.Back "));
}

void sensorSettings(int option, int serialInput, int confirmationDialogDecision) {
  readOption = option; // save the first option
  switch (option) {
    case 1:
      if (serialInput <= maxSampleRate && serialInput >= minSampleRate) { //checks for input to be between 1 and 10
        samplingInterval = serialInput * 1000;        // multiply by 1000 for using ms
        Serial.print(F("Sensor's sampling rate set to: "));
        Serial.print(samplingInterval / 1000);     // div by 1000 to print the sampling rate in seconds
        Serial.println(F(" seconds"));
        readOption = -1; //reset the flag
        printSensorSettings();
      } else {
        Serial.println();
        Serial.println(F("Set sensor's sampling interval to a value between 1 and 10 seconds"));
      }
      break;
    case 2:
      if (serialInput > minimumAlertThreshold) {
        ultrasonicAlertThreshold = serialInput;
        Serial.print(F("Ultrasonic minimum alert threshold set to: "));
        Serial.println(ultrasonicAlertThreshold);
        readOption = -1;
        printSensorSettings();
      } else {
        Serial.println();
        Serial.println(F("Set MINIMUM alert threshold ( > 1 )for the ultrasonic sensor"));
      }
      break;
    case 3:
      if (serialInput > minimumAlertThreshold) {
        ldrAlertThreshold = serialInput;
        Serial.print(F("LDR minimum alert threshold set to: "));
        Serial.println(ldrAlertThreshold);
        readOption = -1;
        printSensorSettings();
      } else {
        Serial.println();
        Serial.println(F("Set MINIMUM alert threshold ( > 1) for the LDR sensor"));
      }
      break;
    case 4:
      readOption2 = serialInput; // save the second option (in this case is not a value, but a submenu option, for what sensor to be reset)
      switch (serialInput) {
        case 1:
          if (confirmationDialogDecision == 1) {      // check if the user chose 1 (YES) for reset
            samplingInterval = -1;
            Serial.println(F("Sampling rate has been reset !"));
            readOption2 = -1;
            printSensorSettingsResetMenu();
          } else if (confirmationDialogDecision == 2) {
            readOption2 = -1;
            Serial.println(F("Sampling rate has not been reset !"));
            printSensorSettingsResetMenu();
          } else {
            printConfirmationDialog();
          }
          break;
        case 2:
          if (confirmationDialogDecision == 1) {
            ultrasonicAlertThreshold = -1;
            Serial.println(F("Ultrasonic Alert Threshold has been reset !"));
            readOption2 = -1;
            printSensorSettingsResetMenu();
          } else if (confirmationDialogDecision == 2) {
            readOption2 = -1;
            Serial.println(F("Ultrasonic Alert Threshold has not been reset !"));
            printSensorSettingsResetMenu();
          } else {
            printConfirmationDialog();
          }
          break;
        case 3:
          if (confirmationDialogDecision == 1) {
            ldrAlertThreshold = -1;
            Serial.println(F("LDR Alert Threshold has been reset !"));
            readOption2 = -1;
            printSensorSettingsResetMenu();
          } else if (confirmationDialogDecision == 2) {
            readOption2 = -1;
            Serial.println(F("LDR Alert Threshold has not been reset !"));
            printSensorSettingsResetMenu();
          } else {
            printConfirmationDialog();
          }
          break;
        case 4:
          if (confirmationDialogDecision == 1) {
            samplingInterval = -1;
            ultrasonicAlertThreshold = -1;
            ldrAlertThreshold = -1;
            Serial.println(F("ALL sensor settings have been reset !"));
            readOption2 = -1;
            printSensorSettingsResetMenu();
          } else if (confirmationDialogDecision == 2) {
            readOption2 = -1;
            Serial.println(F("Sensor settings have not been reset !"));
            printSensorSettingsResetMenu();
          } else {
            printConfirmationDialog();
          }
          break;
        case 5:
          readOption = -1;    //reset flags to go back in previous menu
          readOption2 = -1;
          printSensorSettings();
          break;
        default:              
          readOption2 = -1;   // reset second option flag to stay in this submenu (not go futher to the confirmation dialog menu)
          printSensorSettingsResetMenu();
          break;
      }
      break;
    case 5:
      inMainMenu = true;
      inSensorSettings = false;
      readOption = -1;
      printMainMenu();    //reset flags to go back in previous menu
      break;
    default:
      handleError();
      readOption = -1;    // reset first option flag to stay in this menu
      printSensorSettings();
      break;
  }
}


void printResetLoggerData() {
  Serial.println();
  Serial.println(F("Select from options what you wish to reset: "));
  Serial.println(F("  2.1. Ultrasonic Sensor's logs"));
  Serial.println(F("  2.2. LDR Sensor's logs"));
  Serial.println(F("  2.3. All logs"));
  Serial.println(F("  2.4. The Average distance"));
  Serial.println(F("  2.5. The Average luminosity"));
  Serial.println(F("  2.6. All average values"));
  Serial.println(F("  2.7. Back"));
}

void printConfirmationDialog() {
  Serial.println();
  Serial.println(F("Are you sure ?"));
  Serial.println(F("  1. YES "));
  Serial.println(F("  2. NO "));
}

void resetLoggerData(int option, int confirmationDialogDecision) {
  readOption = option;
  switch (option) {
    case 1:
      if (confirmationDialogDecision == 1) {
        for (int i = intBytes * numberOfSavedLogs; i < totalResetBytes; i++) { // basically, resets bytes from 20 to 59
          EEPROM.update(i, 0);
        }
        Serial.println(F("Logger data for Ultrasonic sensor has been reset !"));
        readOption = -1;
        printResetLoggerData();
      } else if (confirmationDialogDecision == 2) {
        Serial.println(F("Logger data has NOT been reset !"));
        readOption = -1;
        printResetLoggerData();
      } else {
        printConfirmationDialog();
      }
      break;
    case 2:
      if (confirmationDialogDecision == 1) {
        for (int i = 0; i < intBytes * numberOfSavedLogs; i++) {    //basically x2, resets bytes from 0 to 19
          EEPROM.update(i, 0);
        }
        Serial.println(F("Logger data for LDR sensor has been reset"));
        readOption = -1;
        printResetLoggerData();
      } else if (confirmationDialogDecision == 2) {
        Serial.println(F("Logger data has NOT been reset !"));
        readOption = -1;
        printResetLoggerData();
      } else {
        printConfirmationDialog();
      }
      break;
    case 3:
      if (confirmationDialogDecision == 1) {
        for (int i = 0; i < totalResetBytes; i++) {
          EEPROM.update(i, 0);
        }
        Serial.println(F("Logger data for all sensors has been reset"));
        readOption = -1;
        printResetLoggerData();
      } else if (confirmationDialogDecision == 2) {
        Serial.println(F("Logger data has NOT been reset !"));
        readOption = -1;
        printResetLoggerData();
      } else {
        printConfirmationDialog();
      }
      break;
    case 4:
        if (confirmationDialogDecision == 1) {
        for (int i = averageDistanceAdress; i < (averageDistanceAdress + doubleBytes) ; i++) {
          EEPROM.update(i, 0);
        }
        Serial.println(F("Average Distance has been reset"));
        readOption = -1;
        printResetLoggerData();
      } else if (confirmationDialogDecision == 2) {
        Serial.println(F("Average Distance has NOT been reset !"));
        readOption = -1;
        printResetLoggerData();
      } else {
        printConfirmationDialog();
      }
      break;
    case 5:
          if (confirmationDialogDecision == 1) {
        for (int i = averageLuminosityAdress; i < (averageLuminosityAdress + doubleBytes); i++) {
          EEPROM.update(i, 0);
        }
        Serial.println(F("Average luminosity has been reset"));
        readOption = -1;
        printResetLoggerData();
      } else if (confirmationDialogDecision == 2) {
        Serial.println(F("Average Luminosity has NOT been reset !"));
        readOption = -1;
        printResetLoggerData();
      } else {
        printConfirmationDialog();
      }
      break;
    case 6:
          if (confirmationDialogDecision == 1) {
        for (int i = averageLuminosityAdress ; i < (averageDistanceAdress + doubleBytes) ; i++) {
          EEPROM.update(i, 0);
        }
        Serial.println(F("All average values have been reset"));
        readOption = -1;
        printResetLoggerData();
      } else if (confirmationDialogDecision == 2) {
        Serial.println(F("All average values have NOT been reset"));
        readOption = -1;
        printResetLoggerData();
      } else {
        printConfirmationDialog();
      }
      break;
    case 7:
      inMainMenu = true;
      inResetLoggerData = false;
      readOption = -1;
      printMainMenu();
      break;
    default:
      handleError();
      readOption = -1;
      printResetLoggerData();
      break;
  }
}

void printSystemStatus() {
  Serial.println();
  Serial.println(F("3.System Status"));
  Serial.println(F("  3.1. Current Sensor Readings (press 6 to stop)"));
  Serial.println(F("  3.2. Current Sensor Settings"));
  Serial.println(F("  3.3. Display Logged Data"));
  Serial.println(F("  3.4. Display Average Readings"));
  Serial.println(F("  3.5. Back "));
}

void systemStatus(int option) {
  switch (option) {
    case 1:
      if (samplingInterval / 1000 >= minSampleRate && samplingInterval / 1000 <= maxSampleRate) {      //if sample rate is not between the set values as min and max then it will not toggle the flag to read
        Serial.println(F("| Ultrasonic Sensor Readings | LDR Sensor Readings |"));                     // I could just check if it is -1 (without to be divided) or 0 if divided, but safer, the better
        allowPrintSensorValues = true;
      } else {
        Serial.println(F("Warning ! Sample rate value not set. Please set a value for sample rate to read sensor values."));
        inSensorSettings = true;
        inSystemStatus = false;
        printSensorSettings();
      }
      break;
    case 2:
      Serial.print(F("Current sampling rate: "));
      Serial.println(samplingInterval / 1000);
      Serial.print(F("Current minimum threshold value for Ultrasonic sensor: "));
      Serial.println(ultrasonicAlertThreshold);
      Serial.print(F("Current minimum threshold value for LDR sensor: "));
      Serial.println(ldrAlertThreshold);
      printSystemStatus();
      break;
    case 3:
      Serial.println(F("| Ultrasonic Sensor Logs | LDR Sensor Logs |"));
      for (int i = 0; i < numberOfSavedLogs; i++) {
        long currentUltrasonicLog;
        EEPROM.get((i * longBytes) + 20, currentUltrasonicLog); //reads every 4 bytes, goes from 20 to 56
        Serial.print("              ");
        Serial.print(currentUltrasonicLog);
        Serial.print("                        ");
        int currentLdrLog;
        EEPROM.get(i * intBytes, currentLdrLog); //reads every 2 bytes goes from 0 to 18 (last log information is saved on 18 and 19 address)
        Serial.print(currentLdrLog);
        Serial.println("           ");
      }
      printSystemStatus();
      break;
    case 4:
      Serial.println();
      Serial.print(F("Average Luminosity: "));
      double averageLuminosity;
      EEPROM.get(averageLuminosityAdress, averageLuminosity);
      Serial.println(averageLuminosity);
      Serial.print(F("Average Distance: "));
      double averageDistance;
      EEPROM.get(averageDistanceAdress, averageDistance);
      Serial.println(averageDistance);
      printSystemStatus();
      break;
    case 5:
      inMainMenu = true;
      inSystemStatus = false;
      printMainMenu();
      break;
    case 6:
      allowPrintSensorValues = false;
      printSystemStatus();
      break;
    default:
      handleError();
      break;
  }
}

void printReadings() {
  if (allowPrintSensorValues == true && millis() - prevTimeSampleTaken >= samplingInterval) {           //print and saves the sensor's reading in EEPROM in first 60 addresses;
    Serial.print("              ");                         
    Serial.print(distance);
    Serial.print("                        ");
    Serial.print(photocellValue);
    Serial.println("           ");
    sumLuminosity = photocellValue + sumLuminosity;                                                       //calculates average and put it in EEPROM
    sumDistance = distance + sumDistance;
    counter++;
    EEPROM.put((currentSavedLog * longBytes) + (intBytes * numberOfSavedLogs), distance);
    EEPROM.put(currentSavedLog * intBytes, photocellValue);
    EEPROM.put(averageLuminosityAdress,sumLuminosity/counter);
    EEPROM.put(averageDistanceAdress,sumDistance/counter);
    currentSavedLog++;
    if (currentSavedLog >= numberOfSavedLogs) {
      currentSavedLog = 0;
    }
    prevTimeSampleTaken = millis();
  }
}

void printRgbLedControl() {
  Serial.println();
  Serial.println(F("4.RGB LED Control"));
  Serial.println(F("  4.1. Manual Color Control"));
  Serial.println(F("  4.2. LED: Toggle Automatic ON/OFF"));
  Serial.println(F("  4.3. Back"));
}

void rgbLedControl(int option, int colorValue) {
  readOption = option;
  switch (option) {
    case 1:
      if (colorValue > -1 && colorValue <= maxLedLuminosity && currentCollor < colors) { //improvised for, I need to put value for every rgb led, so it will enter 3 times in the function, so I have to read on a line 3 values, blue green red
        rgbColor[currentCollor] = colorValue;
        currentCollor++;
      } else {
        Serial.println();
        Serial.println(F("Give 3 valors (0-255) for Blue, Green and Red leds using space between them: "));
      }

      if (currentCollor >= colors) {  // if the iterator goes further than 3 iterations, the flag and iterator are reset
        readOption = -1;
        currentCollor = 0;
        printRgbLedControl();
      }
      break;
    case 2:
      if (ultrasonicAlertThreshold >= 0 && ldrAlertThreshold >= 0) { // I made so you can't switch to auto mode if you have the alert threshold not set
        ledAutomaticMode = !ledAutomaticMode;
        readOption = -1;
        Serial.print(F("Led's mode was switched to "));
        if (ledAutomaticMode == true) {
          Serial.println(F("automatic mode"));
        } else {
          Serial.println(F("manual mode"));
        }
        printRgbLedControl();
      } else {
        Serial.println(F("Warning ! Threshold alert not set for Ultrasonic sensor or LDR sensor. Please set the threshold values for both so you can enable the automatic mod.")); //also a warning and set the flags
        inSensorSettings = true;                                                                                                                                                   //to send you to sensor settings menu
        inRgbLedControl = false;
        readOption = -1;
        printSensorSettings();
      }
      break;
    case 3:
      inMainMenu = true;
      inRgbLedControl = false;
      printMainMenu();
      readOption = -1;
      break;
    default:
      handleError();
      readOption = -1;
      break;
  }
}

void ledBehavior() {
  if (ledAutomaticMode == true) {
    if (distance <= ultrasonicAlertThreshold || photocellValue <= ldrAlertThreshold) { //check if the distance is less than the threshold alert that I set, if it is, turns the led red
      analogWrite(ledPinRed, maxLedLuminosity);
      analogWrite(ledPinBlue, minLedLuminosity);
      analogWrite(ledpinGreen, minLedLuminosity);
    } else {                                                // else let stays green
      analogWrite(ledpinGreen, maxLedLuminosity);
      analogWrite(ledPinBlue, minLedLuminosity);
      analogWrite(ledPinRed, minLedLuminosity);
    }
  } else {
    for (int i = 0; i < colors; i++) {                      // if the auto mod is off, sets the led values to the values that were set in manual mod
      analogWrite(rgbColorPins[i], rgbColor[i]);
    }
  }
}



// void ultrasonicSensorReading() {                           // my try to change the delay to micros tell me in feedback if the code was good, chose to let it here and used
//   if (completeCycle == true) {                             // the code from lab to make sure if a problem appears is not from here :)
//     // Clears the trigPin
//     digitalWrite(trigPin, LOW);
//     if (micros() - prevTimeTrigger >= triggerDelayLOW) {
//       // Sets the trigPin on HIGH state for 10 micro seconds
//       digitalWrite(trigPin, HIGH);
//       completeCycle = !completeCycle;
//     }
//   }
//   if (micros() - prevTimeTrigger >= triggerDelayHIGH) {
//     digitalWrite(trigPin, LOW);
//     // Reads the echoPin, returns the sound wave travel time in microseconds
//     duration = pulseIn(echoPin, HIGH);
//     // Sound wave reflects from the obstacle, so to calculate the distance we
//     // consider half of the distance traveled.
//     distance = duration * 0.034 / 2;
//     completeCycle = !completeCycle;
//     prevTimeTrigger = micros();
//   }
// }

void ultrasonicSensorReading() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds

  duration = pulseIn(echoPin, HIGH);
  // Sound wave reflects from the obstacle, so to calculate the distance we
  // consider half of the distance traveled.
  distance = duration * 0.034 / 2;
}

void ldrSensorReading() {
  photocellValue = analogRead(photocellPin);
}