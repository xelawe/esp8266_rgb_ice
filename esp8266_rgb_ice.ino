#include <cy_serdebug.h>
#include <cy_serial.h>

const char *gc_hostname = "esprgbice";

#include "cy_wifi.h"

//#include "cy_mqtt.h"

#include "fire_tools.h"
#include "ota_tool.h"

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

  gv_noconn_reset = false;
  gv_portal_timeout = 60;
  wifi_init(gc_hostname);

  init_ota_local();

  init_fire();

  fireticker.attach(0.05, firetick);

  DebugPrintln("Setup done.");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Main loop
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop()
{

  check_ota();

  delay(20);

}
