#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
byte patchNum = 0;
#define BUTTON_PIN_1 2
#define BUTTON_PIN_2 3
#define LED_PIN_1 12
#define LED_PIN_2 11
#define PIN_5 5

enum ButtonState {
  Idle,
  Pressed,
  LongPress
};

ButtonState buttonState1 = Idle;
ButtonState buttonState2 = Idle;
unsigned long buttonPressedTime1 = 0;
unsigned long buttonPressedTime2 = 0;
bool longPressHandled1 = false;
bool longPressHandled2 = false;
unsigned long debounceDelay = 50; // Adjust this value as needed
const unsigned long minPressTime = 50; // Minimum press time in milliseconds
const unsigned long minRetriggerTime = 1250; // Minimum re-trigger time in milliseconds

void setup() {
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(PIN_5, OUTPUT);
  digitalWrite(PIN_5, HIGH); // Simulate long press initially on BUTTON_PIN_1
  digitalWrite(LED_PIN_1, HIGH);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void handleButton(ButtonState &buttonState, unsigned long &buttonPressedTime, bool &longPressHandled, int buttonPin, int ledPin, int oppositeLedPin) {
  unsigned long currentTime = millis();

  switch (buttonState) {
    case Idle:
      if (digitalRead(buttonPin) == LOW) {
        buttonState = Pressed;
        buttonPressedTime = currentTime;
        longPressHandled = false;
      }
      break;

    case Pressed:
      if (digitalRead(buttonPin) == HIGH) {
        if (currentTime - buttonPressedTime < debounceDelay) {
          // Ignore noise during debouncing
        } else if (currentTime - buttonPressedTime < 500) {
          if (patchNum == (buttonPin == BUTTON_PIN_1 ? 24 : 0)) {
            patchNum = buttonPin == BUTTON_PIN_1 ? 0 : 24;
          } else {
            patchNum += (buttonPin == BUTTON_PIN_1) ? 1 : -1;
          }
          MIDI.sendProgramChange(patchNum, 1);
          delay(200);
        } else if (currentTime - buttonPressedTime > minRetriggerTime) {
          buttonState = LongPress;
          digitalWrite(PIN_5, buttonPin == BUTTON_PIN_1 ? LOW : HIGH); // Send signal to pin 5
          digitalWrite(ledPin, HIGH);
          digitalWrite(oppositeLedPin, LOW);
        }
      }
      break;

    case LongPress:
      if (digitalRead(buttonPin) == HIGH && !longPressHandled) {
        longPressHandled = true;
        buttonState = Idle;
      }
      break;
  }
}

void loop() {
  handleButton(buttonState1, buttonPressedTime1, longPressHandled1, BUTTON_PIN_1, LED_PIN_1, LED_PIN_2);
  handleButton(buttonState2, buttonPressedTime2, longPressHandled2, BUTTON_PIN_2, LED_PIN_2, LED_PIN_1);
}
