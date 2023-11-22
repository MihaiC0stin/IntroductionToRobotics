#include <EEPROM.h>

int samplingInterval = -1;
int ultrasonicAlertThreshold = -1;
int ldrAlertThreshold = -1;
bool inMainMenu = true;
bool inSensorSettings = false;
bool inResetLoggerData = false;
bool inSystemStatus = false;
bool inRgbLedControl = false;
int readOption = -1;
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
unsigned long int prevTimeTrigger = 0;

bool allowPrintSensorValues = false;

const int triggerDelayHIGH = 12;
const int triggerDelayLOW = 2;


const int numberOfSavedLogs = 10;

bool completeCycle = true;

int currentSavedLog = 0;

bool ledAutomaticMode = false;

const int colors = 3;
int rgbColor[colors] = { 0, 0, 0 };
int rgbColorPins[colors] = { 3, 5, 6 };
int currentCollor = 0;

const int minSampleRate = 1;
const int maxSampleRate = 10;
const int minimumAlertThreshold = 1;

const int longBytes = 4;
const int intBytes = 2;
const int maxLedLuminosity = 255;
const int minLedLuminosity = 0;
const int totalResetBytes = (longBytes + intBytes) * numberOfSavedLogs;




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
  handleInputAndMenuOperations();
  ultrasonicSensorReading();
  ldrSensorReading();
  printReadings();
  ledBehavior();
}

void handleInputAndMenuOperations() {
  if (Serial.available()) {
    int read = Serial.parseInt();
    if (inMainMenu == true) {
      mainMenu(read);
    } else {
      if (inSensorSettings == true && readOption == -1 && readOption2 == -1) {
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
      } else if (inSystemStatus == true && allowPrintSensorValues == true) {
        if (read == 5) {
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
      inMainMenu = false;
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
  readOption = option;
  switch (option) {
    case 1:
      if (serialInput <= maxSampleRate && serialInput >= minSampleRate) {
        samplingInterval = serialInput * 1000;
        Serial.print(F("Sensor's sampling rate set to: "));
        Serial.print(samplingInterval / 1000);
        Serial.println(F(" seconds"));
        readOption = -1;
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
      readOption2 = serialInput;
      switch (serialInput) {
        case 1:
          if (confirmationDialogDecision == 1) {
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
          readOption = -1;
          readOption2 = -1;
          printSensorSettings();
          break;
        default:
          readOption2 = -1;
          printSensorSettingsResetMenu();
          break;
      }
      break;
    case 5:
      inMainMenu = true;
      inSensorSettings = false;
      readOption = -1;
      printMainMenu();
      break;
    default:
      handleError();
      readOption = -1;
      printSensorSettings();
      break;
  }
}


void printResetLoggerData() {
  Serial.println();
  Serial.println(F("Select which device's logs to be reset: "));
  Serial.println(F("  2.1. Ultrasonic sensor"));
  Serial.println(F("  2.2. LDR sensor"));
  Serial.println(F("  2.3. All sensors available"));
  Serial.println(F("  2.4. Back "));
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
        for (int i = intBytes * numberOfSavedLogs; i < totalResetBytes; i++) {
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
        for (int i = 0; i < intBytes * numberOfSavedLogs; i++) {
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
  Serial.println(F("  3.1. Current Sensor Readings (press 5 to stop)"));
  Serial.println(F("  3.2. Current Sensor Settings"));
  Serial.println(F("  3.3. Display Logged Data"));
  Serial.println(F("  3.4. Back "));
}

void systemStatus(int option) {
  switch (option) {
    case 1:
      if (samplingInterval / 1000 >= minSampleRate && samplingInterval / 1000 <= maxSampleRate) {
        Serial.println(F("| Ultrasonic Sensor Readings | LDR Sensor Readings |"));
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
        EEPROM.get((i * longBytes) + 20, currentUltrasonicLog);
        Serial.print("              ");
        Serial.print(currentUltrasonicLog);
        Serial.print("                        ");
        int currentLdrLog;
        EEPROM.get(i * intBytes, currentLdrLog);
        Serial.print(currentLdrLog);
        Serial.println("           ");
      }
      printSystemStatus();
      break;
    case 4:
      inMainMenu = true;
      inSystemStatus = false;
      printMainMenu();
      break;
    case 5:
      allowPrintSensorValues = false;
      printSystemStatus();
      break;
    default:
      handleError();
      break;
  }
}

void printReadings() {
  if (allowPrintSensorValues == true && millis() - prevTimeSampleTaken >= samplingInterval) {
    Serial.print("              ");
    Serial.print(distance);
    Serial.print("                        ");
    Serial.print(photocellValue);
    Serial.println("           ");
    EEPROM.put((currentSavedLog * longBytes) + 20, distance);
    EEPROM.put(currentSavedLog * intBytes, photocellValue);
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
      if (colorValue > -1 && colorValue <= maxLedLuminosity && currentCollor < colors) {
        rgbColor[currentCollor] = colorValue;
        currentCollor++;
      } else {
        Serial.println();
        Serial.println(F("Give 3 valors (0-255) for Blue, Green and Red leds using space between them: "));
      }

      if (currentCollor >= colors) {
        readOption = -1;
        currentCollor = 0;
        printRgbLedControl();
      }
      break;
    case 2:
      if (ultrasonicAlertThreshold >= 0 && ldrAlertThreshold >= 0) {
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
        Serial.println(F("Warning ! Threshold alert not set for Ultrasonic sensor or LDR sensor. Please set the threshold values for both so you can enable the automatic mod."));
        inSensorSettings = true;
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
    if (distance <= ultrasonicAlertThreshold || photocellValue <= ldrAlertThreshold) {
      analogWrite(ledPinRed, maxLedLuminosity);
      analogWrite(ledPinBlue, minLedLuminosity);
      analogWrite(ledpinGreen, minLedLuminosity);
    } else {
      analogWrite(ledpinGreen, maxLedLuminosity);
      analogWrite(ledPinBlue, minLedLuminosity);
      analogWrite(ledPinRed, minLedLuminosity);
    }
  } else {
    for (int i = 0; i < colors; i++) {
      analogWrite(rgbColorPins[i], rgbColor[i]);
    }
  }
}



// void ultrasonicSensorReading() {
//   if (completeCycle == true) {
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