// Rainbow cycle on the NeoPixel: 2024 ladyada for Adafruit Industries, MIT License

#include <WS2812.h>   // NeoPixel

// QT Py CH552 (8051) specific
#define NEOPIXEL_PIN        P1_0
#define NEOPIXEL_SHOW_FUNC  neopixel_show_P1_0
enum
{
  NUM_LEDS = 1,
  COLORS_PER_LEDS = 3,
  NUM_BYTES = NUM_LEDS * COLORS_PER_LEDS
};
#if NUM_BYTES > 255
#error "NUM_BYTES can not be larger than 255."
#endif
__xdata uint8_t ledData[NUM_BYTES];   // 8051 XRAM
void neopixel_show()
{
  NEOPIXEL_SHOW_FUNC(ledData, NUM_BYTES);
}


uint8_t neopixel_brightness = 255;


void neopixel_setPixelColorRgb(uint32_t c)
{
  uint16_t r, g, b;
  r = (((c >> 16) & 0xFF) * neopixel_brightness) >> 8;
  g = (((c >> 8) & 0xFF) * neopixel_brightness) >> 8;
  b = ((c & 0xFF) * neopixel_brightness) >> 8;

  set_pixel_for_GRB_LED(ledData, /* LED # */ 0, r, g, b);
}


uint32_t RgbWheel(uint8_t WheelPos) {
  uint8_t r, g, b;
  
  if (WheelPos < 85)
  {
    r = WheelPos * 3;
    g = 255 - WheelPos * 3 ;
    b = 0;
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    r = 255 - WheelPos * 3;
    g = 0;
    b = WheelPos * 3;
  }
  else
  {
    WheelPos -= 170;
    r = 0;
    g = WheelPos * 3;
    b = 255 - WheelPos * 3;
  }

  return ((uint32_t) r << 16) | ((uint32_t) g << 8) | (uint32_t) b;
}


void rainbowTick()
{
  for (uint8_t i = 0; i < 255; ++i)
  {
    neopixel_setPixelColorRgb(RgbWheel(i));
    neopixel_show();
    delay(20);
  }
}


void setup()
{
  pinMode(NEOPIXEL_PIN, OUTPUT);
  neopixel_brightness = 50;
}


void loop()
{
  rainbowTick();
}
