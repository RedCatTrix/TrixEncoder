/* TrixEncoder
 *  
 * Lightweight library for rotary encoder with key (Rotary Encoder Module for reference https://ali.onl/1TNu)  
 * 
 * Version 1.1
 * 
 * Initial publish date 17/09/2021
 * Distributed under GNU General Public License v3.0
 * Author Dmitry V (RedCatTrix@yahoo.com), to say Thanks and support me: PayPal (gem.ini@rambler.ru)  
 *  
 * Written for 16 MHz Atmega328p based projects (Arduino Nano), however it can be used in other AVR-based platform
 *  
 * Events support: rotation, key pressed rotation, fast rotation, single click, double click, key holding, custom click event
 * 
 * Features:
 * 1. No-class library, saves a bit SRAM
 * 2. Based on PIN CHANGE INTERRUPT which allows waking up from sleep mode, valuable external INT0/INT1 remain available for other project purpose
 * 3. The main code implemented in the ISR section, hence the value and state preserved until being processed in your code, even with delay() etc
 * 4. ISR processing time takes up to 8.5 us or less for any encoder event
 * 5. The code quiet simple, can be change it easily by your own to meet your specific requirements 
 * 
 * 
 * This is the only connection so far:
 * 
 * Encoder side       Arduno Nano side
 *    Vcc ------------------ +5V
 *    Key ------------------ D8
 *    S2  ------------------ D10
 *    S1  ------------------ D9
 *    GND ------------------ GND
 *    
 * Of course you may change the library code, pins assignment, IRQ type and source etc
 * 
 * Release history
 * 
 * v.1.0 - intitial issue
 * v.1.1:
 *  - PCINT initialization in enc_begin() method minor change
 *  - License type noted in the header
 */
 
#ifndef TrixEncoder_h
#define TrixEncoder_h

#include "Arduino.h"

// Enable encoder value from the library, calculation of value performed in ISR
// commented definition will disable value counting, the only event type returns
#define TRIX_ENC_ISR_CALC

#define CUSTOM_CLICK        0x00
#define SINGLE_CLICK        0x01
#define DOUBLE_CLICK        0x02
#define LONG_CLICK          0x03
#define UNDEFINED_STATE     0xFF
#define CWISE               0x84
#define CCWISE              0x85
#define FCWISE              0x86
#define FCCWISE             0x87
#define KCWISE              0x88
#define KCCWISE             0x89

extern "C" {

  extern void enc_begin(const uint8_t  pin_key, const uint8_t pin_s1, const uint8_t pin_s2);
    
  extern bool enc_eventOccurred();
  
  extern bool enc_tickEvent();

  extern void enc_bigStepMode(const bool mode_flag = false);
 
  extern bool enc_keyPressed();

#ifdef TRIX_ENC_ISR_CALC

  extern int enc_value();

  extern void enc_setValue(const int16_t newValue);
  
  extern void enc_setKeySingleClick_ms(const uint16_t ms = 230);
  
  extern void enc_setBigStepOffset(const uint16_t step_value);
  
  extern void enc_setFastStepOffset(const uint16_t fast_value);

  extern void enc_setFastTick_ms(const uint8_t ms);

#endif

  extern uint8_t enc_event();

  extern void enc_reset_event();
 
  extern void enc_setHoldDelay_ms(const uint16_t ms);

  extern uint16_t enc_getClickDuration();
  
  extern void enc_setKeyDoubleClick_ms(const uint16_t ms = 380);

}

#endif
