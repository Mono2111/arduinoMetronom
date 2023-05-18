#include <Arduino.h>

const int ledPin = 3;
const int inputPin = 2;

int buttonState = 0;
int lastButtonState = 0;
int currentButtonState = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

long buttonPressIntervals[3] = {0, 0, 0}; //First value is the start point of the measurement; The others indicate the time intervals.
int buttonPressCount = 0; //How often was the button pressed 

int bpm = 120; //Start BPM
long lastLEDBlinkTime = 0; 
boolean ledIsOn = false; //Defines if the led is currently on
int blinkDuration = 200; //Defines how long the led is on

/**
 * @brief Checks whether the button state was changed 
 */
void checkButtonState();
/**
 * @brief Checks whether the led should now flash
 */
void checkLEDBlink();
/**
 * @brief Checks whether the measurement should be aborted
 */
void checkForResetBPMMeasurement();
/**
 * @brief Resets the button measurement values.
 */
void resetButtonPressValues();

void setup() {
  pinMode(inputPin, INPUT);
  pinMode(ledPin, OUTPUT);
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
          }
          else if(buttonPressCount == 1) 
          {
            buttonPressIntervals[1] = millis() - buttonPressIntervals[0];
            ++buttonPressCount;
          }
          else if(buttonPressCount == 2) 
          {
            buttonPressIntervals[2] = millis() - (buttonPressIntervals[1]+buttonPressIntervals[0]);
            ++buttonPressCount;
          }
          else { //calculate new bpm 
            bpm = 60000 / ((buttonPressIntervals[1] + buttonPressIntervals[2]) / 2);

            resetButtonPressValues();
          }
      }
    }
  }
  lastButtonState = currentButtonState;
}

void checkForResetBPMMeasurement()
{
  if(millis() - buttonPressIntervals[0] > 1200 * 3) //1200ms = 50 bpm 
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