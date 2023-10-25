const int input_blue = A0;
const int input_green = A1;
const int input_red = A2;

const int output_blue = 9;
const int output_green = 10;
const int output_red = 11;

const int from_low = 0;
const int from_high = 1023;
const int to_low = 0;
const int to_high = 255;

const int baud = 9600;

// global variables are initialized to 0 by default
int input_value_blue = 0;
int output_value_blue = 0;
int input_value_green = 0;
int output_value_green = 0;
int input_value_red = 0;
int output_value_red = 0;

void setup() {
  Serial.begin(baud);
  pinMode(input_blue, INPUT);
  pinMode(output_blue, OUTPUT);
  pinMode(input_green, INPUT);
  pinMode(output_green, OUTPUT);
  pinMode(input_red, INPUT);
  pinMode(output_red, OUTPUT);
}

void loop() {
  input_value_blue = analogRead(input_blue);
  input_value_green = analogRead(input_green);
  input_value_red = analogRead(input_red);
  output_value_blue = map(input_value_blue, from_low, from_high, to_low, to_high);
  output_value_green = map(input_value_green, from_low, from_high, to_low, to_high);
  output_value_red = map(input_value_red, from_low, from_high, to_low, to_high);

  Serial.print("(");
  Serial.print(output_value_blue);
  Serial.print(" ");
  Serial.print(output_value_green);
  Serial.print(" ");
  Serial.print(output_value_red);
  Serial.println(")");


  analogWrite(output_blue, output_value_blue);
  analogWrite(output_green, output_value_green);
  analogWrite(output_red, output_value_red);
}