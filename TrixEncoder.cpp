#include "TrixEncoder.h"

#define encKeyPressed() !bitRead(PINB, PINB0)

#define ENC_CWISE           0b00000011    // по часовой стрелке 
#define ENC_CCWISE          0b00000010    // против часовой стрелки
#define ENC_KCWISE          0b10000011    // по часовой стрелке, Key
#define ENC_KCCWISE         0b10000010    // против часовой стрелки, Key
#define ENC_FCWISE          0b01000011    // быстро по часовой стрелке 
#define ENC_FCCWISE         0b01000010    // быстро против часовой стрелки

uint16_t _step_value = 100;
uint16_t _fast_value = 10;
uint8_t enc_fast_tick_delay = 30;
uint16_t enc_event_counter_delay = 3000;
volatile int16_t _enc_value = 0;
volatile bool enc_irq = false;
bool enc_rotating = false;
uint32_t enc_key_on_time = 0;
uint32_t enc_key_off_time = 0;
uint16_t enc_key_duration = 0;
uint8_t key_press_number = 0;
uint16_t enc_key_double_click_ms = 380;
uint16_t enc_key_single_click_ms = 230;
uint8_t _enc_event = 0;
bool _enc_mltp_step;

extern "C" {

  bool enc_keyPressed() {
    return encKeyPressed();
  }

  uint16_t getTimeOffset(const uint32_t timestamp) {
    return (millis() - timestamp);
  }

  bool enc_tickEvent() {
    enc_irq = false;
    return bitRead(_enc_event, 7);
  }

  void enc_begin(const uint8_t pin_key, const uint8_t pin_s1, const uint8_t pin_s2) {
    pinMode(pin_s1, INPUT_PULLUP);
    pinMode(pin_s2, INPUT_PULLUP);
    pinMode(pin_key, INPUT_PULLUP);
    bitSet(PCICR, PCIE0); 
    // на пинах PIN_S1   и   PIN_KEY
    PCMSK0 |= _BV(PCINT1) | _BV(PCINT0);
    _enc_mltp_step = true;
  }

  void enc_reset_event() {
    _enc_event = UNDEFINED_STATE;
  }

  uint16_t enc_getClickDuration() {
    if (enc_tickEvent()) enc_key_duration = 0;
    return enc_key_duration;
  }

  void enc_setHoldDelay_ms(const uint16_t ms) {
    enc_event_counter_delay = ms; 
  }

  bool enc_eventOccurred() {
    return enc_irq;
  }
  

  uint8_t enc_event() {

    uint32_t t = millis();

    while (encKeyPressed()) {
      
      if (getTimeOffset(t) >= enc_event_counter_delay) {
        enc_key_duration = enc_event_counter_delay;
        _enc_event = LONG_CLICK;
        goto label_out;
      }

      if (enc_rotating) goto label_out;
    }
  
    t = millis();
    while (getTimeOffset(t) < enc_key_double_click_ms and !enc_rotating) {}
    
    enc_key_duration = enc_key_off_time - enc_key_on_time;
    
    if (key_press_number == DOUBLE_CLICK) {
      _enc_event = DOUBLE_CLICK;
      goto label_out;
    }

    if (enc_key_duration and enc_key_duration < enc_key_single_click_ms and !enc_rotating) {
      _enc_event = SINGLE_CLICK;
      goto label_out;
    }
    
  label_out:
    if (enc_tickEvent()) enc_key_duration = 0;
    enc_irq = false;
    return _enc_event;
  }

#ifdef TRIX_ENC_ISR_CALC

  int enc_value() {
    return _enc_value;
  }

  void enc_setValue(const int16_t newValue) {
    _enc_value = newValue;
  }

  void enc_bigStepMode(const bool mode_flag) {
    _enc_mltp_step = mode_flag;
  }

  void enc_setBigStepOffset(const uint16_t step_value) {
    _step_value = step_value;   
  }

  void enc_setFastStepOffset(const uint16_t fast_value) {
    _fast_value = fast_value;   
  }

  void enc_setFastTick_ms(const uint8_t ms) {
    enc_fast_tick_delay = ms;
  }

#endif

  void enc_setKeySingleClick_ms(const uint16_t ms) {
    enc_key_single_click_ms = ms;
  }

  void enc_setKeyDoubleClick_ms(const uint16_t ms) {
    enc_key_double_click_ms = ms;
  }

// время выполнения
// Key_on - 6.8 мкс
// Key_off - 7.4 мкс
// Tick - 8.25 мкс
// Tick + Key_on - 8.4 мкс
// =< 8.4  мкс (9.7 мкс максимум)
ISR (PCINT0_vect) {
  static uint8_t enc_s1_prev = 1;
  static uint8_t enc_key_prev = 0;
  static uint32_t enc_key_first_click_time = 0;
  static uint32_t enc_last_tick_change = 0;  

  if (encKeyPressed() ^ enc_key_prev) {

    enc_key_prev = encKeyPressed();
    
    // проверка таймер двойного клика
    if (enc_key_first_click_time and millis() - enc_key_first_click_time > enc_key_double_click_ms) {
      enc_key_first_click_time = 0;
      key_press_number = 0;
    }

    if (encKeyPressed()) {
      enc_key_on_time = millis();                               
      _enc_event = 0;
      enc_irq = true;
    } else {                                                    
      enc_key_off_time = millis();                              
      if (++key_press_number == SINGLE_CLICK and !enc_key_first_click_time) enc_key_first_click_time = millis(); // ++key_press_number должен выполняться первым, это важно
    }
    enc_rotating = false;
  }

  // обработка вращения с нажатием Key или без
  const uint8_t enc_s1 = bitRead(PINB, PINB1);
  
  if (enc_s1 ^ enc_s1_prev) {
    enc_s1_prev = enc_s1;
    uint8_t enc_state = (enc_s1 << 1) | bitRead(PINB, PINB2);
    const bool b = enc_last_tick_change and millis() - enc_last_tick_change < enc_fast_tick_delay and !encKeyPressed();
    if (b) bitSet(enc_state, 6); // бит 6 - быстрый тик, всегда без нажатой кнопки
    if (encKeyPressed()) bitSet(enc_state, 7);
#ifdef TRIX_ENC_ISR_CALC
    switch (enc_state) {
      case ENC_CWISE  : ++_enc_value; _enc_event = CWISE; break;
      case ENC_CCWISE : --_enc_value; _enc_event = CCWISE; break;
      case ENC_FCWISE : _enc_value += _fast_value; _enc_event = FCWISE; break;
      case ENC_FCCWISE: _enc_value -= _fast_value; _enc_event = FCCWISE; break;
      case ENC_KCWISE : {_enc_mltp_step ? _enc_value += _step_value : ++_enc_value; _enc_event = KCWISE; break;}
      case ENC_KCCWISE: {_enc_mltp_step ? _enc_value -= _step_value : --_enc_value; _enc_event = KCCWISE;}
    }
#else
    switch (enc_state) {
      case ENC_CWISE  : _enc_event = CWISE; break;
      case ENC_CCWISE : _enc_event = CCWISE; break;
      case ENC_FCWISE : _enc_event = FCWISE; break;
      case ENC_FCCWISE: _enc_event = FCCWISE; break;
      case ENC_KCWISE : _enc_event = KCWISE; break;
      case ENC_KCCWISE: _enc_event = KCCWISE;
    }
#endif

    if (bitRead(enc_state, 1)) {
      enc_last_tick_change = millis();
      key_press_number = 0;
      enc_rotating = true;
      enc_irq = true;
    }
  }
}

}
