/* TrixEndcoder 1.0 example
 *  
 * This example demonstrates SINGLE CLICK and TICK events handling. The example uses built-in LED on Arduino Nano board
 * 
 */

#include <TrixEncoder.h>

// >>> Don't change this assignment, there is no pre-processor handler implemented for arbitrary pins
#define S1_PIN  9
#define S2_PIN  10
#define KEY_PIN 8

void setup() {
  // put your setup code here, to run once:

  // indicates bigStepOffset enable mode
  pinMode(LED_BUILTIN, OUTPUT);
 
  // initialize encoder instance, enable PCINT
  enc_begin(KEY_PIN, S1_PIN, S2_PIN);

  // ensure Global Interrupt Enabled
  // sei(); // actually no need, 'coz Global Interrupt enabled at least during millis() background initialization

}

void loop() {
  // put your main code here, to run repeatedly:

  static bool state = false;
  
  // enc_eventOccurred() returns TRUE if encoder state changed
  if (enc_eventOccurred()) {
    
    // enc_tickEvent() returns TRUE if event is tick-type
    // inverse LED state on each tick
    if (enc_tickEvent()) digitalWrite(LED_BUILTIN, state = !state);

    // LED blinks on SINGLE CLICK
    if (enc_event() == SINGLE_CLICK) {
      
      for (byte i = 0; i < 10; i++) {
        digitalWrite(LED_BUILTIN, state = !state);
        delay(50);
      }
      
    }
    
  }
   
  
}
