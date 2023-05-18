#include <Arduino.h>

const int ledPin = 3;
const int inputPin = 2;

int buttonState = 0;
int lastButtonState = 0;
int currentButtonState = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

long buttonPressIntervals[3] = {0, 0, 0};
int buttonPressCount = 0;

int bpm = 120;
long lastLEDBlinkTime = 0;
boolean ledIsOn = false;
int blinkDuration = 200; //ms => max BPM = 300

void checkButtonState();
void checkLEDBlink();
void checkForResetBPMMeasurement();
void resetButtonPressValues();

void setup() {
  pinMode(inputPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  checkButtonState();
  checkLEDBlink();

  checkForResetBPMMeasurement();
}

void checkLEDBlink()
{
  if(ledIsOn && millis()-lastLEDBlinkTime >= blinkDuration)
  {
    digitalWrite(ledPin, LOW);
    ledIsOn = false;
  }

  if(!ledIsOn && millis()-lastLEDBlinkTime >= 60000/bpm)
  {
    digitalWrite(ledPin, HIGH);
    lastLEDBlinkTime = millis();
    ledIsOn = true;
  }
}

void checkButtonState()
{
  currentButtonState = digitalRead(inputPin);

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentButtonState != buttonState) {
      buttonState = currentButtonState;
      if (buttonState == LOW) {

          if(buttonPressCount == 0) //start measurement
          {
            buttonPressIntervals[0] = millis();
            ++buttonPressCount;

            Serial.println("PRESSED ONE");
          }
          else if(buttonPressCount == 1) //take two points 
          {
            buttonPressIntervals[1] = millis() - buttonPressIntervals[0]; //10500 - 10000 = 500
            ++buttonPressCount;
            Serial.println("PRESSED TWO");
          }
          else if(buttonPressCount == 2) //take two points 
          {
            buttonPressIntervals[2] = millis() - (buttonPressIntervals[1]+buttonPressIntervals[0]); // 110000 - (500+10000) = 500
            ++buttonPressCount;
            Serial.println("PRESSED THREE");
          }
          else { //only if PressCount = 3  => calculate new bpm 
            bpm = 60000 / ((buttonPressIntervals[1] + buttonPressIntervals[2]) / 2);

            Serial.print("PRESSED FOUR ");
            Serial.print(buttonPressIntervals[1]);
            Serial.print(" / ");
            Serial.println(buttonPressIntervals[2]);

            resetButtonPressValues();

            Serial.print("New BPM: ");
            Serial.println(bpm);
          }
      }
    }
  }
  lastButtonState = currentButtonState;
}

void checkForResetBPMMeasurement()
{
  if(millis() -buttonPressIntervals[0] > 1200*3) //3 pressed for 50 bpm
  {
    resetButtonPressValues();
  }
}

void resetButtonPressValues()
{
  buttonPressCount = 0;
  buttonPressIntervals[0] = 0;
  buttonPressIntervals[1] = 0;
  buttonPressIntervals[2] = 0;
}