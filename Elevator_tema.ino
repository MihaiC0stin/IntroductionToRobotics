const int ButtonPinFloor_1 = 8;
const int ButtonPinFloor_2 = 10;
const int ButtonPinFloor_3 = 12;

const int LedPinFloor_1 = 9;
const int LedPinFloor_2 = 11;
const int LedPinFloor_3 = 13;

const int FloorButtonPins[] = {ButtonPinFloor_1, ButtonPinFloor_2, ButtonPinFloor_3};
const int FloorLedPins[] = {LedPinFloor_1, LedPinFloor_2, LedPinFloor_3};

const int BuzzerPin = 3;
const int LedPinStatusElevator = 2;

byte ButtonStateFloor_1 = LOW;
byte ButtonStateFloor_2 = LOW;
byte ButtonStateFloor_3 = LOW;

byte LedStateFloor_1 = LOW;
byte LedStateFloor_2 = LOW;
byte LedStateFloor_3 = LOW;

byte ButtonFloorStates[] = {ButtonStateFloor_1, ButtonStateFloor_2, ButtonStateFloor_3};
byte LedFloorStates[] = {LedStateFloor_1, LedStateFloor_2, LedStateFloor_3};

byte reading[] = {HIGH, HIGH, HIGH};
byte lastreading[] = {HIGH, HIGH, HIGH};

unsigned long LastDebounceTime = 0;
int DebounceDelay = 50;

int CurrentFloor = 0;
int FloorToGo = 0;

int CloseDoorTime = 2000;
int MoveBetweenFloorsTime = 6000;
unsigned long TimeAfterButtonPressed = 0;
unsigned long PreviousTime = 0;

int buzzerToneCloseDoor = 4350;
int buzzerToneElevatorMoving = 3500;

bool ElevatorStatus = false;


void setup() {
pinMode(ButtonPinFloor_1, INPUT_PULLUP);
pinMode(ButtonPinFloor_2, INPUT_PULLUP);
pinMode(ButtonPinFloor_3, INPUT_PULLUP);
pinMode(LedPinFloor_1, OUTPUT);
pinMode(LedPinFloor_2, OUTPUT);
pinMode(LedPinFloor_3, OUTPUT);
pinMode(BuzzerPin, OUTPUT);
pinMode(LedPinStatusElevator, OUTPUT);
Serial.begin(9600);
}

void loop() { 

  Serial.print("Current Floor: ");
  Serial.print(CurrentFloor);
  Serial.print("\t");
  Serial.print("Floor to Go to: ");
  Serial.println(FloorToGo);

  for(int i = 0; i < sizeof(FloorButtonPins)/sizeof(int); i++){
      
      reading[i] = digitalRead(FloorButtonPins[i]);

      if(reading[i] != lastreading[i]){
        LastDebounceTime = millis();
      }
      if((millis() - LastDebounceTime) > DebounceDelay){

        if(reading[i] != ButtonFloorStates[i]){
          ButtonFloorStates[i] = reading[i];
          if((ButtonFloorStates[i] == LOW) && (CurrentFloor != i)){
            FloorToGo = i;
            ElevatorStatus = true;
            tone(BuzzerPin, buzzerToneCloseDoor, CloseDoorTime);
            TimeAfterButtonPressed = millis();
          }
        }  
      } 
    digitalWrite(FloorLedPins[i], LedFloorStates[i]);
    lastreading[i] = reading[i];
   }


    if((millis() - TimeAfterButtonPressed > CloseDoorTime) && (millis() - PreviousTime) > MoveBetweenFloorsTime) {
      

      if(FloorToGo - CurrentFloor > 0){
        tone(BuzzerPin, buzzerToneElevatorMoving, MoveBetweenFloorsTime);
        LedFloorStates[CurrentFloor] = LOW;
        CurrentFloor ++;
        LedFloorStates[CurrentFloor] = HIGH;
        
      }else if(FloorToGo - CurrentFloor < 0){
        tone(BuzzerPin, buzzerToneElevatorMoving, MoveBetweenFloorsTime);
        LedFloorStates[CurrentFloor] = LOW;
        CurrentFloor --;
         LedFloorStates[CurrentFloor] = HIGH;

      }else{
            if(ElevatorStatus == true){
              tone(BuzzerPin, buzzerToneCloseDoor, CloseDoorTime);
            }
            LedFloorStates[CurrentFloor] = HIGH;
            ElevatorStatus = false;
      }

      PreviousTime = millis();
    }

   if(ElevatorStatus == true){
      if(millis() % 200 > 100){
          digitalWrite(LedPinStatusElevator,HIGH);
       }
      else{
          digitalWrite(LedPinStatusElevator,LOW);
      }        
   }

   if(ElevatorStatus == false)
   {
    digitalWrite(LedPinStatusElevator, LOW);
   }
}