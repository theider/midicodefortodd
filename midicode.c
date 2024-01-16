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

void loop() {
  unsigned long currentTime = millis();

  // Button 1
  if (digitalRead(BUTTON_PIN_1) == LOW) {
    if (buttonState1 == Idle) {
      buttonState1 = Pressed;
      buttonPressedTime1 = currentTime;
    } else if (buttonState1 == Pressed && currentTime - buttonPressedTime1 > 1000) {
      buttonState1 = LongPress;
      digitalWrite(PIN_5, LOW); // Send low signal to pin 5
      digitalWrite(LED_PIN_1, HIGH);
      digitalWrite(LED_PIN_2, LOW);
    }
  } else {
    if (buttonState1 == Pressed) {
      if (currentTime - buttonPressedTime1 < 500) {
        if (patchNum == 24) {
          patchNum = 0;
        } else {
          patchNum++;
        }
        MIDI.sendProgramChange(patchNum, 1);
        delay(200);
      }
      buttonState1 = Idle;
    } else if (buttonState1 == LongPress && currentTime - buttonPressedTime1 > 500) {
      buttonState1 = Idle;
    }
  }

  // Button 2
  if (digitalRead(BUTTON_PIN_2) == LOW) {
    if (buttonState2 == Idle) {
      buttonState2 = Pressed;
      buttonPressedTime2 = currentTime;
    } else if (buttonState2 == Pressed && currentTime - buttonPressedTime2 > 1000) {
      buttonState2 = LongPress;
      digitalWrite(PIN_5, HIGH); // Send high signal to pin 5
      digitalWrite(LED_PIN_2, HIGH);
      digitalWrite(LED_PIN_1, LOW);
    }
  } else {
    if (buttonState2 == Pressed) {
      if (currentTime - buttonPressedTime2 < 500) {
        if (patchNum == 0) {
          patchNum = 24;
        } else {
          patchNum--;
        }
        MIDI.sendProgramChange(patchNum, 1);
        delay(200);
      }
      buttonState2 = Idle;
    } else if (buttonState2 == LongPress && currentTime - buttonPressedTime2 > 500) {
      buttonState2 = Idle;
    }
  }
}
