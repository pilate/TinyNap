#include "TinyNap.h"

#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


void watchdog_setup(uint8_t wdp)
{
  // reset status register
  MCUSR = 0;

  // WDP3 is 6th bit? cool
  if (wdp & 0b00001000)
  {
    wdp |= bit(WDP3);
  }

  // interrupt flag, interupt enable, change enable, enable
  wdp |= bit(WDIF) | bit(WDIE) | bit(WDCE) | bit(WDE);

  WDTCR = wdp;
}

void nap(uint32_t nap_time)
{
  int i;
  uint8_t s;

  // cli();

  uint8_t prev_sra = ADCSRA;
  ADCSRA = 0;

  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  for (i = 8192, s = 9; i >= 16; i /= 2, s--)
  {
    while (nap_time > i)
    {
      watchdog_setup(s);
      // sei();
      sleep_cpu();
      nap_time -= i;
    }
  }

  sleep_disable();

  if (nap_time)
  {
    delay(nap_time);
  }
  ADCSRA = prev_sra;

}

ISR(WDT_vect)
{
  wdt_disable();
}