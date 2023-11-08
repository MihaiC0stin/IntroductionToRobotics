// declare all the segments pins
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

const int segSize = 8;  //we have 7 segments and 1 decimal point that is a dot.


int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};  // array of pins

byte ledState[segSize] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW };                    // array of led states for every pin in order
bool ledStateON[segSize] = { false, false, false, false, false, false, false, false };  // array of booleans for pins. This will be used to fix if a led should be LOW or HIGH for a segment.

const int blinkTime = 1000;
const int resetTime = 3000;
unsigned long prevTimeBlink = 0;
unsigned long prevTimeReset = 0;
bool buttonPressed = false;


// declare all the pins
const int pinSW = 2;  // digital pin connected to switch output
const int pinX = A0;  // A0 - analog pin connected to X output
const int pinY = A1;  // A1 - analog pin connected to Y output
// byte swState = LOW;
int xValue = 0;
int yValue = 0;
bool joystickMoved = false;
const int minThreshold = 400;
const int maxThreshold = 700;

byte reading = HIGH;
byte lastReading = HIGH;
unsigned int lastDebounceTime = 0;
unsigned int debounceDelay = 50;
byte buttonState = HIGH;
const int numberOfSegments = 8;
const int numberOfDirections = 4;

const int availableMovementsForCurrentPin[numberOfSegments][numberOfDirections]{
  // this array shows where the current pin can go for every direction.
  // UP   DOWN   LEFT   RIGHT
  { pinA, pinG, pinF, pinB },    // a   0
  { pinA, pinG, pinF, pinB },    // b   1
  { pinG, pinD, pinE, pinDP },   // c   2
  { pinG, pinD, pinE, pinC },    // d   3
  { pinG, pinD, pinE, pinC },    // e   4
  { pinA, pinG, pinF, pinB },    // f   5
  { pinA, pinD, pinG, pinG },    // g   6
  { pinDP, pinDP, pinC, pinDP }  // dp  7
};

const int numberOfRows = 2;  // needed to make a matrix with 2 rows(is exactly under this line of code)

const int rowCurrentPin[numberOfRows][numberOfSegments]{ //array used to assign a position (it can also be seen as a key or id) for every pin. These positions will be used everywhere.
                                                         { pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP },
                                                         { 0, 1, 2, 3, 4, 5, 6, 7 }
};

int currentPin = pinDP;   // we start from pinDP
int rowOfCurrentPin = 7;  // also assign the row for dp pin. This is not really needed, but good to have for better code and less errors that may appear.

volatile bool possiblePress = false;       // Indicates that a button press might have occurred
volatile bool shouldStartCounter = false;  // Indicates the need to start debounce counter

void setup() {

  for (int i = 0; i < segSize; i++) {
    // Set the pin mode for each segment
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);  // activate pull-up resistor on the // push-button pin

  // Start the serial communication.
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, CHANGE);  //We attach the interrupt to pinSW (2)
}

void loop() {

  xValue = analogRead(pinX);  // read position of joystick on X axis
  yValue = analogRead(pinY);  // read position of joystick on Y axis


  // Serial.print("Switch: ");
  // Serial.print(reading);
  // Serial.print(" | ");
  // Serial.print("X-axis: ");
  // Serial.print(xValue);
  // Serial.print(" | ");             //here we have my testing madness XD
  // Serial.print("Y-axis: ");
  // Serial.print(yValue);
  // Serial.print(" | ");
  // Serial.print("Current Pin: ");
  // Serial.print(currentPin);
  // Serial.print(" | ");
  // Serial.print("Row : ");
  // Serial.print(rowOfCurrentPin);
  // Serial.print(" | ");
  // Serial.print("LedState: ");
  // Serial.print(ledState[rowOfCurrentPin]);
  // Serial.print(" | ");
  // Serial.print("LedStateON: ");
  // Serial.print(ledStateON[rowOfCurrentPin]);
  // Serial.print(" | ");
  // Serial.print("ButtonPressed ");
  // Serial.print(buttonPressed);
  // Serial.print(" | ");
  // Serial.print("possible press: ");
  // Serial.print(possiblePress);
  // Serial.print(" | ");
  // Serial.print("shouldStartCounter ");
  // Serial.print(shouldStartCounter);
  // Serial.println(" | ");


  //here we have 4 main functions
  debounce();
  blink();
  joistickMovement();
  reset();
}


void joistickMovement() { //function used to control the 7 segment display using joystick

  for (int i = 0; i < numberOfSegments; i++) {                   
    if (currentPin == rowCurrentPin[numberOfRows - 2][i]) {// we check for the position of the current pin.
      rowOfCurrentPin = rowCurrentPin[numberOfRows - 1][i]; // and we assign the position of current pin to a variable that will be used in more cases.
    }
  }

  if ((yValue < minThreshold) && (xValue > minThreshold) && (xValue < maxThreshold) && joystickMoved == false) { //check if joystick moved up and if joystick was in idle position
    if (ledStateON[rowOfCurrentPin] == true && ledState[rowOfCurrentPin] == LOW) { //this if and the other else if are used to correct the led state if needed because of blink
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);
    } else if (ledStateON[rowOfCurrentPin] == false && ledState[rowOfCurrentPin] == HIGH) {
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);
    }
    currentPin = availableMovementsForCurrentPin[rowOfCurrentPin][numberOfDirections - 4]; //current pin change to the pin that is in matrix at position of current pin and direction (in our case here the direction is UP)
    joystickMoved = true; // used to stop the movement of current pin on segments when joystick is held in a position.
  }

  if ((yValue > maxThreshold) && (xValue > minThreshold) && (xValue < maxThreshold) && joystickMoved == false) {
    if (ledStateON[rowOfCurrentPin] == true && ledState[rowOfCurrentPin] == LOW) {
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);
    } else if (ledStateON[rowOfCurrentPin] == false && ledState[rowOfCurrentPin] == HIGH) {
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);
    }
    currentPin = availableMovementsForCurrentPin[rowOfCurrentPin][numberOfDirections - 3];
    joystickMoved = true;
  }

  if ((xValue < minThreshold) && (yValue > minThreshold) && (yValue < maxThreshold) && joystickMoved == false) {
    if (ledStateON[rowOfCurrentPin] == true && ledState[rowOfCurrentPin] == LOW) {
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);

    } else if (ledStateON[rowOfCurrentPin] == false && ledState[rowOfCurrentPin] == HIGH) {
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);
    }
    currentPin = availableMovementsForCurrentPin[rowOfCurrentPin][numberOfDirections - 2];
    joystickMoved = true;
  }

  if ((xValue > maxThreshold) && (yValue > minThreshold) && (yValue < maxThreshold) && joystickMoved == false) {
    if (ledStateON[rowOfCurrentPin] == true && ledState[rowOfCurrentPin] == LOW) {
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);
    } else if (ledStateON[rowOfCurrentPin] == false && ledState[rowOfCurrentPin] == HIGH) {
      ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
      digitalWrite(currentPin, ledState[rowOfCurrentPin]);
    }
    currentPin = availableMovementsForCurrentPin[rowOfCurrentPin][numberOfDirections - 1];
    joystickMoved = true;
  }

  if ((xValue > minThreshold) && (xValue < maxThreshold) && (yValue > minThreshold) && (yValue < maxThreshold)) { // if joystick returns to idle position then it can enter again in ifs
    joystickMoved = false;
  }
}

void blink() {      // blink effect on led. Change the ledstate at a set amount of time
  if (millis() - prevTimeBlink >= blinkTime) {
    ledState[rowOfCurrentPin] = !ledState[rowOfCurrentPin];
    prevTimeBlink = millis();
  }
}


void debounce() {

  // Check if a possible button press was detected
  if (possiblePress) {
    // Start the debounce counter if needed
    if (shouldStartCounter) {
      lastDebounceTime = millis();
      shouldStartCounter = false;
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      reading = digitalRead(pinSW);
      if (reading != buttonState) {
        buttonState = reading;
        buttonPressed = !buttonPressed; // if button is pressed (FALLING), it changes to true, after the button is not pressed anymore (RISING) it goes back to false
        if (buttonState == LOW) {
          ledStateON[rowOfCurrentPin] = !ledStateON[rowOfCurrentPin];
          prevTimeReset = millis(); //start the timer for reset
        }
      }
      possiblePress = false;
    }
  }
  digitalWrite(currentPin, ledState[rowOfCurrentPin]);
}
// Interrupt service routine (ISR) executed when the button is pressed
void handleInterrupt() {
  possiblePress = true;       // Indicate that a button press might have occurred
  shouldStartCounter = true;  // Indicate the need to start the debounce counter
}

void reset() {
  if ((millis() - prevTimeReset > resetTime) && (buttonPressed == true)) { //After an amount of time if the button is still pressed starts the reset
    for (int i = 0; i < segSize; i++) {
      ledState[i] = LOW;
      ledStateON[i] = false;
      digitalWrite(segments[i], ledState[i]);
      currentPin = pinDP;
    }
  }
}