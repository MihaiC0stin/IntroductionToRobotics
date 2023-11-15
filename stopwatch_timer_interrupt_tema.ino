//Define pins for buttons
const int pinStartPause = 2;
const int pinReset = 8;
const int pinSaveLap = 3;
// Define connections to the shift register
const int latchPin = 11;  // Connects to STCP (latch pin) on the shift register
const int clockPin = 10;  // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;   // Connects to DS (data pin) on the shift register
// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
// Store the digits in an array for easy access
int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int displayCount = 4;  // Number of digits in the display
// Define the number of unique encodings (0-9, A-F for hexadecimal)
const int encodingsNumber = 16;
// Define byte encodings for the hexadecimal characters 0-F
byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};
// Variables for controlling the display update timing
unsigned long lastIncrement = 0;
unsigned long delayCount = 100;  // Delay between updates (milliseconds)
unsigned long number = 0;        // The number being displayed

bool pause = true;
bool reset = false;
bool savedLapsViewMode = false;

byte readingReset = HIGH;
byte lastReadingReset = HIGH;
byte buttonStateReset = HIGH;
unsigned int lastDebounceTimeReset = 0;
unsigned int debounceDelay = 50;

const int laps = 4;
int savedLaps[laps] = { 0, 0, 0, 0 };
int currentLap = 0;
int iteratorSavedLaps = 0;

// Flag to indicate a valid button press event
volatile bool buttonPressedStartPause = false;
volatile bool buttonPressedSaveLap = false;
// Timestamp for when the last interrupt occurred, used for debouncing
volatile unsigned long lastInterruptTimeStartPause = 0;
volatile unsigned long lastInterruptTimeSaveLap = 0;
// Debounce time in milliseconds
const unsigned long interruptDebounceDelay = 200;


void setup() {
  // Initialize the pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  // Initialize digit control pins and set them to LOW (off)
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  // Initialize the pins for buttons
  pinMode(pinStartPause, INPUT_PULLUP);
  pinMode(pinReset, INPUT_PULLUP);
  pinMode(pinSaveLap, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinStartPause), handleInterruptStartPause, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinSaveLap), handleInterruptSaveLap, FALLING);
  // Begin serial communication for debugging purposes
  Serial.begin(9600);
}
void loop() {

  for (int i = 0; i < laps; i++) {
    Serial.print("|");
    Serial.print(savedLaps[i]);
  }
  Serial.println("|");

  debounceStartPause();
  debounceReset();
  debounceSaveLap();
  // DONE: Check if the current time is greater than 'lastIncrement' plus 'delayCount'
  if (millis() - lastIncrement > delayCount && pause == false) {
    // DONE: Increment the number and reset 'lastIncrement'
    number++;
    // DONE: Ensure that 'number' wraps around after reaching 9999 to start over from 0
    number %= 10000;  // Wrap around after 9999
    lastIncrement = millis();
  }
  // DONE: Display the incremented number on the 7-segment display using multiplexing
  writeNumber(number);
}

void handleInterruptStartPause() {
  static unsigned long interruptTime = 0;  // Timestamp for the current interrupt, retains its value between ISR calls
  interruptTime = micros();                // Capture the current time in microseconds
  // Debounce logic: If interrupts come in faster succession than the debounce delay, they're ignored
  if (interruptTime - lastInterruptTimeStartPause > interruptDebounceDelay * 1000) {  // Convert debounceDelay to microseconds for comparison
    buttonPressedStartPause = true;
  }
  // Update the last interrupt timestamp
  lastInterruptTimeStartPause = interruptTime;
}

void debounceStartPause() {
  if (buttonPressedStartPause == true) {
    if (savedLapsViewMode == false) {
      pause = !pause;
      reset = false;
    }
    buttonPressedStartPause = false;
  }
}

void debounceReset() {
  readingReset = digitalRead(pinReset);
  if (readingReset != lastReadingReset) {
    lastDebounceTimeReset = millis();
  }
  if ((millis() - lastDebounceTimeReset) > debounceDelay) {
    if (readingReset != buttonStateReset) {
      buttonStateReset = readingReset;
      if (buttonStateReset == LOW && pause == true && savedLapsViewMode == false) {
        number = 0;
        reset = true;
      } else if (buttonStateReset == LOW && pause == true && savedLapsViewMode == true) {
        number = 0;
        savedLapsViewMode = false;
        for (currentLap = 0; currentLap < laps; currentLap++) {
          savedLaps[currentLap] = 0;
        }
        currentLap = 0;
      }
    }
  }
  lastReadingReset = readingReset;
}

void handleInterruptSaveLap() {
  static unsigned long interruptTime = 0;  // Timestamp for the current interrupt, retains its value between ISR calls
  interruptTime = micros();                // Capture the current time in microseconds
  // Debounce logic: If interrupts come in faster succession than the debounce delay, they're ignored
  if (interruptTime - lastInterruptTimeSaveLap > interruptDebounceDelay * 1000) {  // Convert debounceDelay to microseconds for comparison
    buttonPressedSaveLap = true;
  }
  // Update the last interrupt timestamp
  lastInterruptTimeSaveLap = interruptTime;
}

void debounceSaveLap() {
  if (buttonPressedSaveLap == true) {
    if (pause == false) {
      savedLaps[currentLap] = number;
      currentLap++;
      if (currentLap > 3) {
        currentLap = 0;
      }
    } else if (pause == true && reset == true) {
      savedLapsViewMode = true;
      number = savedLaps[iteratorSavedLaps];
      iteratorSavedLaps++;
      if (iteratorSavedLaps > 3) {
        iteratorSavedLaps = 0;
      }
    }
    buttonPressedSaveLap = false;
  }
}


void writeReg(int digit) {
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber) {
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);

  if (displayNumber == 2) {
    writeReg(B00000001);
  }
}

void writeNumber(int number) {
  // DONE: Initialize necessary variables for tracking the current number and digit position
  int currentNumber = number;
  int displayDigit;
  int lastDigit = 0;
  // DONE: Loop through each digit of the current number
  for (displayDigit = 3; displayDigit >= 0; displayDigit--) {
    // DONE: Extract the last digit of the current number
    lastDigit = currentNumber % 10;
    // DONE: Activate the current digit on the display
    activateDisplay(displayDigit);
    // DONE: Output the byte encoding for the last digit to the display
    writeReg(byteEncodings[lastDigit]);
    // DONE: Update 'currentNumber' by removing the last digit
    currentNumber /= 10;
    // DONE: Clear the display to prevent ghosting between digit activations
    writeReg(B00000000);  // Clear the register to avoid ghosting
  }
}