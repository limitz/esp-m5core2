/* Created 19 Nov 2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * This is a driver for the WS2812 RGB LEDs using the RMT peripheral on the ESP32.
 *
 * This code is placed in the public domain (or CC0 licensed, at your option).
 */

#ifndef WS2812_DRIVER_H
#define WS2812_DRIVER_H

#include <stdint.h>
#include <stddef.h>

typedef union {
  struct __attribute__ ((packed)) {
    uint8_t r, g, b;
  };
  uint32_t num;
} rgbVal;

void ws2812_init();
void ws2812_setColors(unsigned int length, rgbVal *array);

inline void ws2812_update(rgbVal *array, size_t len)
{
	ws2812_setColors(len, array);
}

inline rgbVal makeRGBVal(uint8_t r, uint8_t g, uint8_t b)
{
  rgbVal v;


  v.r = r;
  v.g = g;
  v.b = b;
  return v;
}
#define WS2812_PIN 25
#define RGB makeRGBVal
#endif /* WS2812_DRIVER_H */
