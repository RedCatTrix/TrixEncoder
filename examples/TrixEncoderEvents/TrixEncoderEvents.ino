/* TrixEndcoder 1.0 example
 *  
 * This example demonstrates how TrixEndcoder works, the captured event sends to Serial Monitor port
 * 
 * Uncomment delay() to check that the event not lost
 */

#include <TrixEncoder.h>

// >>> Don't change this assignment, there is no pre-processor handler implemented for arbitrary pins
#define S1_PIN  9
#define S2_PIN  10
#define KEY_PIN 8

bool mode = true;

void setup() {
  // indicates bigStepOffset enable mode
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, mode);
  
  // initialize Serial Monitor, port speed 115200 
  Serial.begin(115200);
  Serial.println("Serial monitor started \n");   
  Serial.println("TrixEncoder v.1.0 Demo \n");   
      
  // initialize encoder instance, enable PCINT
  enc_begin(KEY_PIN, S1_PIN, S2_PIN);

  // ensure Global Interrupt Enabled
  // sei(); // actually no need, 'coz Global Interrupt enabled at least during millis() background initialization
}

void loop() {

  // enc_eventOccurred() returns TRUE if encoder state changed
  if (enc_eventOccurred()) {

    const char * event_desc;                        // buffer for event description
    const byte encoder_event = enc_event();         // temp var for encoder event type
    word click_duration = enc_getClickDuration();   // get click duration

    switch (encoder_event) {
      case SINGLE_CLICK:  event_desc =  "Single click";     break;
      case DOUBLE_CLICK:  event_desc =  "Double click"; {
          // toggle increment mode for keyed rotation (step 100 by default, enabled, to change the offset use enc_setBigStepOffset() )
          enc_bigStepMode(mode = !mode);
          // LED on if bigStepMode enabled
          digitalWrite(LED_BUILTIN, mode);
          break;
        }             
      case LONG_CLICK:    event_desc = "Key hold";          enc_setValue(0); break;  // reset encoder value 
      case CWISE:         event_desc = "Right tick";        break;
      case CCWISE:        event_desc = "Left tick";         break;
      case FCWISE:        event_desc = "Fast right tick";   break;
      case FCCWISE:       event_desc = "Fast left tick";    break;
      case KCWISE:        event_desc = "Key + right tick";  break;
      case KCCWISE:       event_desc = "Key + left tick";   break;
      case CUSTOM_CLICK:  event_desc = click_duration > 1000 and click_duration < 2500 ? "Custom action" : "Undefined action"; // from 1 to 2.5 s click considered here as a custom action
   }
   
    // print event type
    Serial.print("Event: " + String(event_desc) + '\t');
    
    // and fetch current encoder value for tick-type event
    if (enc_tickEvent() or encoder_event == LONG_CLICK) {
      Serial.print("\t value = ");
      Serial.print(enc_value());
    }
    Serial.print('\t');
    
    // print SINGLE CLICK duration
    if (encoder_event == SINGLE_CLICK) {
      Serial.print("Duration, ms: ");
      Serial.print(click_duration);  
    }
    
    Serial.println();
  }

  // delay(2000); // uncomment to check the value and event are not lost
}
