# TrixEncoder

Encoder handling library for Atmega328-based/Arduino Nano projects

/* TrixEncoder
 *  
 * Lightweight library for rotary encoder with key (Rotary Encoder Module for reference https://ali.onl/1TNu)  
 * 
 * Written for 16 MHz Atmega328p based projects (Arduino Nano), however can be used in other AVR-based platform
 *  
 * Version 1.0, initial publish date 17/09/2021
 * Author Dmitry V e-mail: gem.ini@rambler.ru
 * To say Thanks and support me: PayPal (gem.ini@rambler.ru)  
 *   
 * Events support: rotation, key pressed rotation, fast rotation, single click, double click, key holding, custom click event
 *  
 * Features:
 * 1. No-class library, saves a bit SRAM
 * 2. Based on PIN CHANGE INTERRUPT which allows waking up from sleep mode, valuable external INT0/INT1 remain available for other project purpose
 * 3. The main code implemented in the ISR section, hence the value and state preserved until being processed in your code, even with delay() etc
 * 4. ISR processing time takes up to 8.5 us or less for any encoder event
 * 5. The code quiet simple, can be changed easily by your own to meet your specific requirements 
 * 
 * 
 * Arbitrary pin assignment automation unavailable. This is the only connection so far:
 * 
 * Encoder side       Arduno Nano side
 *    Vcc ------------------ +5V
 *    Key ------------------ D8
 *    S2  ------------------ D10
 *    S1  ------------------ D9
 *    GND ------------------ GND
 *    
 * Of course you may change the library code, pins assignment, IRQ type and source etc
 */


Methods and constants description.

void enc_begin(const uint8_t  pin_key, const uint8_t pin_s1, const uint8_t pin_s2)
  - initializes PINs, PCINT, mandatory method, called once in SETUP section
     
bool enc_eventOccurred()
  - returns TRUE on any encoder event
  
bool enc_tickEvent()
  - returns TRUE on tick-type event

void enc_bigStepMode(const bool mode_flag)
  - enable (mode_flag = true) or disable (mode_flag = false) enlarged step on tick with key pressed, enabled by default, default value 100

int enc_value()
  - returns current encoder value

void enc_setValue(const int16_t newValue)
  - set encoder value, set 0 to reset value
  
void enc_setKeySingleClick_ms(const uint16_t ms = 230)
  - set duration for SINGLE CLICK, ms
  
void enc_setBigStepOffset(const uint16_t step_value)
  - set offset for enlarged step, default value 100
  
void enc_setFastStepOffset(const uint16_t fast_value)
  - set offset for fast tick, default value 10

void enc_setFastTick_ms(const uint8_t ms)
  - set time interval between two consequent ticks for fast-tick event definition

uint8_t enc_event()
  - returns one of these constant:
                      CUSTOM_CLICK    - Click duration longer than SINGLE_CLICK but less than LONG_CLICK 
                      SINGLE_CLICK    - Single click, default duration 230 ms
                      DOUBLE_CLICK    - Double click, two consequent clicks done in 380 ms interval
                      LONG_CLICK      - Key been held longer than 3 s (default value)
                      UNDEFINED_STATE - Undefined state
                      CWISE           - Clockwise tick
                      CCWISE          - Counterclockwise tick
                      FCWISE          - Fast clockwise tick
                      FCCWISE         - Fast counterclockwise tick
                      KCWISE          - Clockwise tick with key pressed
                      KCCWISE         - Counterclockwise tick with key pressed
void enc_reset_event()
  - set encoder event state to UNDEFINED_STATE
 
void enc_setHoldDelay_ms(const uint16_t ms)
  - set hold time for LONG_CLICK, default value 3000 ms

uint16_t enc_getClickDuration()
  - define single click duration, default value 230 ms
  
void enc_setKeyDoubleClick_ms(const uint16_t ms = 380);
  - define double click interval, default value 380 ms


ISR performance on 16 MHz, encoder value calculation enabled
  Key ON        - 6.8 us
  Key OFF       - 7.4 us
  Tick          - 8.25 us
  Tick + Key_on - 8.4 us
  For unknown reasons rare ISRs perform time may reach 9.7 us
