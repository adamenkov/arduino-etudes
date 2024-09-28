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


uint8_t neopixel_brightness = 150;


void neopixel_setPixelColor(uint32_t c)
{
  uint8_t r, g, b;
  r = ((c >> 16) & 0xFF) * neopixel_brightness >> 8;
  g = ((c >> 8) & 0xFF) * neopixel_brightness >> 8;
  b = (c & 0xFF) * neopixel_brightness >> 8;

  set_pixel_for_GRB_LED(ledData, /* LED # */ 0, r, g, b);
}


typedef struct
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RGB;


void neopixel_setPixelColorRGB(RGB rgb)
{
  set_pixel_for_GRB_LED(
    ledData,
    /* LED # */ 0,
    (uint16_t) rgb.r * neopixel_brightness >> 8,
    (uint16_t) rgb.g * neopixel_brightness >> 8,
    (uint16_t) rgb.b * neopixel_brightness >> 8
  );
}


// Red -> Blue -> Green -> Red -> ...
// (Original rainbow cycle on the NeoPixel: 2024 ladyada for Adafruit Industries, MIT License)
uint32_t Wheel(uint8_t WheelPos) {
  uint8_t r, g, b;
  
  if (WheelPos < 85)
  {
    r = 255 - WheelPos * 3;
    g = 0;
    b = WheelPos * 3;
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    r = 0;
    g = WheelPos * 3;;
    b = 255 - WheelPos * 3;
  }
  else
  {
    WheelPos -= 170;
    r = WheelPos * 3;
    g = 255 - WheelPos * 3;
    b = 0;
  }

  return ((uint32_t) r << 16) | ((uint32_t) g << 8) | (uint32_t) b;
}


void rainbowTick(void)
{
  for (uint8_t i = 0; i < 255; ++i)
  {
    neopixel_setPixelColor(Wheel(i));
    neopixel_show();
    delay(20);
  }
}


void LerpRGB(RGB a, RGB b, float t, RGB* out)
{
  out->r = (1 - t) * a.r + t * b.r;
  out->g = (1 - t) * a.g + t * b.g;
  out->b = (1 - t) * a.b + t * b.b;
}


void setup(void)
{
  pinMode(NEOPIXEL_PIN, OUTPUT);
  neopixel_brightness = 50;
}


void loop(void)
{
  const uint32_t POMODORO_MS = 1000UL * 60 * 25;

  RGB red     = { 0xFF, 0x00, 0x00 };
  RGB yellow  = { 0xFF, 0xFF, 0x00 };
  RGB green   = { 0x00, 0xFF, 0x00 };
  
  float t;
  RGB rgb;

  uint32_t time_elapsed_ms;
  while ((time_elapsed_ms = millis()) < POMODORO_MS)
  {
    if (time_elapsed_ms % 1000 < 500)
    {
      t = (float) time_elapsed_ms / POMODORO_MS;
      LerpRGB(green, red, t, &rgb);
      neopixel_setPixelColorRGB(rgb);
    }
    else
    {
      neopixel_setPixelColor(0x000000);
    }

    neopixel_show();
    delay(40);
  }

  for (;;)
  {
    rainbowTick();
  }
}
