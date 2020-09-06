#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

// data pin
#define PIN 5
// led count
#define CNT 21 //37

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT, PIN, NEO_GRB + NEO_KHZ800);


uint32_t active_color;
//uint32_t fire_color   = strip.Color ( 80,  35,  0);
uint32_t fire_color   = strip.Color ( 255,  80,  0);
//uint32_t ice_color   = strip.Color ( 0,  216,  255);
uint32_t ice_color   = strip.Color ( 0,  80,  94);


int fader_pos;
#define fader_steps 60

#include "NeoFire.h"

NeoFire fire(strip);

Ticker fireticker;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void init_fire() {
  fader_pos = 0;

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
 //active_color = fire_color;

  fire.Draw( active_color );
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void firetick() {

  if (fader_pos >= fader_steps ) {
    init_fire();
  }

  fader_pos++;

  //Serial.print("Fader: ");
  //Serial.print(fader_pos );

  for (int i = 0; i < CNT; i++)
  {
    strip.setPixelColor(i, fire.Fade(strip.getPixelColor(i), fire.fire_colors[i], fader_pos, i) );
  }
  strip.show();

  //Serial.println();
}

