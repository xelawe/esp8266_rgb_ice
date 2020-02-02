

/**
  Arduino Uno - NeoPixel Fire
  v. 1.0
  Copyright (C) 2015 Robert Ulbricht

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cy_serdebug.h>
#include <cy_serial.h>

const char *gc_hostname = "esprgbice";

#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include "cy_wifi.h"
#include "cy_ota.h"
#include "cy_mqtt.h"


// data pin
#define PIN 5
// led count
#define CNT 37

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT, PIN, NEO_GRB + NEO_KHZ800);

uint32_t active_color;
uint32_t fire_color   = strip.Color ( 80,  35,  0);
//uint32_t ice_color   = strip.Color ( 0,  216,  255);
uint32_t ice_color   = strip.Color ( 0,  80,  90);
uint32_t off_color    = strip.Color (  0,  0,  0);

int fader_pos;
#define fader_steps 60


int gv_temp;

Ticker drawticker;
Ticker showticker;
Ticker fireticker;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Fire simulator
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class NeoFire
{
    Adafruit_NeoPixel &strip;
  public:

    NeoFire(Adafruit_NeoPixel&);
    uint32_t fire_colors[CNT];
    int fire_ticks[CNT];
    void Draw();
    void Clear();
    void AddColor(uint8_t position, uint32_t color);
    void SubstractColor(uint8_t position, uint32_t color);
    uint32_t Blend(uint32_t color1, uint32_t color2);
    uint32_t Substract(uint32_t color1, uint32_t color2);
    uint32_t Fade(uint32_t color1, uint32_t color2, uint8_t position, int led);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Constructor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NeoFire::NeoFire(Adafruit_NeoPixel& n_strip) : strip (n_strip)
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set all colors
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::Draw()
{
  Clear();

  //  if ( gv_temp < 20 ) {
  //    active_color = fire_color;
  //  } else {
  //    active_color = ice_color;
  //  }
  //
  //  ++gv_temp;
  //  if ( gv_temp > 30 ) {
  //    gv_temp = 0;
  //  }
  //
  //
  //    Serial.print("Temp: ");
  //  Serial.println(gv_temp );


  active_color = ice_color;

  for (int i = 0; i < CNT; i++)
  {

    AddColor(i, active_color);


    //int r = random(80);
    int b = random(255);
    uint32_t diff_color = strip.Color ( b / 2, b, b);
    SubstractColor(i, diff_color);
  }

  //strip.show();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set color of LED
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::AddColor(uint8_t position, uint32_t color)
{
  //  uint32_t blended_color = Blend(strip.getPixelColor(position), color);
  uint32_t blended_color = Blend(fire_colors[position], color);
  //  strip.setPixelColor(position, blended_color);
  fire_colors[position] = blended_color;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set color of LED
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::SubstractColor(uint8_t position, uint32_t color)
{
  // uint32_t blended_color = Substract(strip.getPixelColor(position), color);
  uint32_t blended_color = Substract(fire_colors[position], color);
  //strip.setPixelColor(position, blended_color);
  fire_colors[position] = blended_color;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Color blending
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t NeoFire::Blend(uint32_t color1, uint32_t color2)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  return strip.Color(constrain(r1 + r2, 0, 255), constrain(g1 + g2, 0, 255), constrain(b1 + b2, 0, 255));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Color blending
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t NeoFire::Substract(uint32_t color1, uint32_t color2)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;
  int16_t r, g, b;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  r = (int16_t)r1 - (int16_t)r2;
  g = (int16_t)g1 - (int16_t)g2;
  b = (int16_t)b1 - (int16_t)b2;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  return strip.Color(r, g, b);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// fade from color1 to color2 with certain fader position
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t NeoFire::Fade(uint32_t color1, uint32_t color2, uint8_t position, int led)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;
  int16_t r, g, b;
  uint8_t diff;
  float incf_r, incf_g, incf_b;
  int16_t inci_r, inci_g, inci_b;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  if ( position == 1) {
    fire_ticks[led] = 0;
  }

  if ( led < 5 ) {
    Serial.print("LED: ");
    Serial.print(led);
    Serial.print(" ");
  }

  //  Serial.print("Rot:");
  //  Serial.print(r1);
  //  Serial.print(" -> ");
  //  Serial.print(r2);
  //  Serial.print("Grün:");
  //  Serial.print(g1);
  //  Serial.print(" -> ");
  //  Serial.print(g2);
  //  Serial.print("Blau:");
  //  Serial.print(b1);
  //  Serial.print(" -> ");
  //  Serial.println(b2);

  // int divisor = fader_steps - position;
  int divisor = fader_steps - position + fire_ticks[led];
  if (divisor <= 0) {
    divisor = 1;
  }

  if (r1 < r2) {
    uint8_t diff = r2 - r1;
    incf_r = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_r = (int16_t)round( incf_r );
    r = (int16_t)r1 + inci_r;
  } else {
    uint8_t diff = r1 - r2;
    incf_r = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_r = (int16_t)round( incf_r );
    r = (int16_t)r1 - inci_r;
  }

  if (g1 < g2) {
    uint8_t diff = g2 - g1;
    incf_g = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_g = (int16_t)round( incf_g );
    g = (int16_t)g1 + inci_g;
  } else {
    uint8_t diff = g1 - g2;
    incf_g = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_g = (int16_t)round( incf_g );
    g = (int16_t)g1 - inci_g;
  }

  if (b1 < b2) {
    uint8_t diff = b2 - b1;
    incf_b = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_b = (int16_t)round( incf_b );
    b = (int16_t)b1 + inci_b;
  } else {
    uint8_t diff = b1 - b2;
    incf_b = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_b = (int16_t)round( incf_b );
    b = (int16_t)b1 - inci_b;
  }

  if ( inci_r == 0 && inci_g == 0 && inci_b == 0 ) {
    fire_ticks[led]++;
  } else {
    fire_ticks[led] = 0;
  }

  if ( led < 5 ) {
    Serial.print("Tick: ");
    Serial.print(fire_ticks[led] );
    Serial.print(", ");

    Serial.print("R: ");
    Serial.print(inci_r);
    Serial.print(" ");
    Serial.print("G: ");
    Serial.print(inci_g);
    Serial.print(" ");
    Serial.print("B: ");
    Serial.print(inci_b);
    Serial.print(" ");
  }

  return strip.Color(r, g, b);
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Every LED to black
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::Clear()
{
  for (uint16_t i = 0; i < strip.numPixels (); i++)
    //strip.setPixelColor(i, off_color);
    fire_colors[i] = off_color;
}

NeoFire fire(strip);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void firedraw() {
  fader_pos = 0;
  fire.Draw();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fireshow() {
  fader_pos++;

  Serial.print("Fader: ");
  Serial.print(fader_pos );

  for (int i = 0; i < CNT; i++)
  {
    strip.setPixelColor(i, fire.Fade(strip.getPixelColor(i), fire.fire_colors[i], fader_pos, i) );
  }
  strip.show();

  Serial.println();

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Setup
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup()
{
  cy_serial::start(__FILE__);

  gv_temp = 0;

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  wifi_init(gc_hostname);

  init_ota(gv_clientname);

  firedraw();
  //drawticker.attach(fader_steps, firedraw);
  //showticker.attach(1, fireshow);
  drawticker.attach(6, firedraw);
  showticker.attach(0.1, fireshow);
  //fireticker.attach(0.1, firetick);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Main loop
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop()
{

  check_ota();

  delay(20);

}
