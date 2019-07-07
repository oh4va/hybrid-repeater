/** @file mmi.ino
    @brief

    This file contains the main functionality
    of the mixed mode repeater interface code.

    @author Veijo Arponen, OH4VA <vepa@iki.fi>
    @bug No known bugs.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "mmi.h"

/**
  Sets up the Arduino board.
*/
void setup() {

  SetPinModes();

  // Glow ANA/DIGI Leds for three seconds
  digitalWrite(GPIO_SVXLINK_PIN, HIGH);
  digitalWrite(GPIO_MMDVM_PIN, HIGH);
  delay(TIME_START);

#ifdef ARDUINO_UNO
  Serial.begin(9600);
  Serial.print("Multi-Mode Interface ");
  Serial.print(VERSION);
  Serial.print('\n');
#endif
}

/**
  Actively controls the Arduino board.
*/
void loop() {

  byte input = 0;           // FSM input
  byte output = 0;          // FSM output
  unsigned long wait_time;  // FSM state delay

#ifdef ARDUINO_UNO
  Serial.print("State: ");
  Serial.print(state);
  Serial.print(", ");
#endif

  // Reset TOT ON flag
  if (state == MMI_IDLE) {
    ptt_tot_on = 0;
  }

  // Select state output
  output = FSM[state].Out;

  // Set output, toggle only if TX has been ON too long
  SetOutput(output, ptt_tot_on);

#ifdef ARDUINO_UNO
  Serial.print("Out: ");
  Serial.print(output, BIN);
  Serial.print(", ");

  Serial.print("TOT: ");
  Serial.print(ptt_tot_on);
  Serial.print(", ");
#endif

  // Set wait time
  wait_time = ptt_tot_on ? TIME_LOOP_TOT : TIME_LOOP;

  // Wait
  delay(wait_time);

  // Increment PTT-ON timer if TX is ON
  if (output & OUT_PTT_BIT) {
    ptt_on_timer++;
  }
  else {
    ptt_on_timer = 0;
  }

  // Decrement mode hang timer
  if (mode_hang_timer > 0 ) {
    mode_hang_timer--;
  }

  // Set mode hang timer
  if (ptt_on_timer > MODE_HANG_THRESHOLD * 1000UL / TIME_LOOP) {
    mode_hang_timer = MODE_HANG_TIME * 1000UL / TIME_LOOP;
  }

  // Set mode hang ON
  if (mode_hang_timer > 0 && !ptt_tot_on) {
    mode_hang_on = 1;
  }
  else {
    mode_hang_on = 0;
  }

  // Set TOT ON flag if TX has been ON too long
  if (ptt_on_timer > PTT_ON_MAX * 1000UL / TIME_LOOP) {
    ptt_tot_on = 1;
  }

  // Read inputs
  input = ReadInput();

#ifdef ARDUINO_UNO
  Serial.print("Input: ");
  Serial.print(input, BIN);
  Serial.print('\n');
#endif

  // Select next state based on the input byte
  state = FSM[state].Next[input];
}


/**
  Set digital pin modes (input/output)

  @param none
  @return none
*/
void SetPinModes() {

  // Digital inputs
  pinMode(GPIO_DMKURI_PTT_PIN, INPUT_PULLUP);
  pinMode(GPIO_MMDVM_PTT_PIN, INPUT_PULLUP);

  // Digital outputs
  pinMode(GPIO_SVXLINK_PIN, OUTPUT);
  pinMode(GPIO_MMDVM_PIN, OUTPUT);
  pinMode(GPIO_PTT_PIN, OUTPUT);
}

/**
  Read input

  @param none
  @return Input value for the FSM packed in one byte
*/

byte ReadInput() {

  byte input = 0;

  // Copy mode hang state to the FSM input byte
  input |= mode_hang_on;
  input = input << 1;

  // Read PTT state from MMDVM
  input |= (!digitalRead(GPIO_MMDVM_PTT_PIN));
  input = input << 1;

  // Read PTT state from DMK Engineering URI or RB RIM Lite
  input |= (!digitalRead(GPIO_DMKURI_PTT_PIN));

  return input;
}

/**
  Write output to GPIO

  @param output Output bits
  @param toggle Toggle LEDs on every call
  @return none
*/

inline void SetOutput(byte output, bool toggle) {

  // Only toggle LEDs
  if (toggle) {
    // Set PTT OFF
    digitalWrite(GPIO_PTT_PIN, LOW);
    // Blink SVXLINK LED
    if (output & OUT_SVX_BIT) {
      digitalWrite(GPIO_SVXLINK_PIN, !digitalRead(GPIO_SVXLINK_PIN));
    }
    // Blink MMDVM LED
    if (output & OUT_DVM_BIT) {
      digitalWrite(GPIO_MMDVM_PIN, !digitalRead(GPIO_MMDVM_PIN));
    }
  }
  else {
    // Set SVXLINK ON/OFF
    digitalWrite(GPIO_SVXLINK_PIN, output & OUT_SVX_BIT);
    // Set MMDVM ON/OFF
    digitalWrite(GPIO_MMDVM_PIN, output & OUT_DVM_BIT);
    // Set PTT ON/OFF
    digitalWrite(GPIO_PTT_PIN, output & OUT_PTT_BIT);
  }
}
