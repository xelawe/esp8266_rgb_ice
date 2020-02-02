

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

#include "NeoFire.h"


int gv_temp;

Ticker drawticker;
//Ticker showticker;
Ticker fireticker;

NeoFire fire(strip);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void firedraw() {
  fader_pos = 0;
  fire.Draw();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fireshow() {


}

void firetick() {


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
  //showticker.attach(0.1, fireshow);
  fireticker.attach(0.1, firetick);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Main loop
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop()
{

  check_ota();

  delay(20);

}
