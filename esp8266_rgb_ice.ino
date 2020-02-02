#include <cy_serdebug.h>
#include <cy_serial.h>

const char *gc_hostname = "esprgbice";

#include "cy_wifi.h"
#include "cy_ota.h"
#include "cy_mqtt.h"

#include "fire_tools.h"

int gv_temp;


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

  init_fire();

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
