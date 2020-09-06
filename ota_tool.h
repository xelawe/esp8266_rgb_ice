#include "cy_ota.h"

int gv_ota_perc = 0;

void init_ota_local() {

  init_ota(gv_clientname);

  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
    //    gv_ota_perc = 0;
    //    oled.clear(PAGE);  // Clear the buffer
    //    oled.setCursor(0, 0); // points cursor to x=0 y=0
    //    oled.println("OTA start");
    //    oled.println("__________");
    //    oled.display();

    // stop fading
    fireticker.detach();

    // set all pixels to black


  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));

    //    int lv_ota_perc = ( progress / (total / 100));
    //
    //    if ( ((lv_ota_perc % 10) == 0) && ( lv_ota_perc != gv_ota_perc ) ){
    //      gv_ota_perc = lv_ota_perc;
    //      oled.print("#");
    //      oled.display();
    //    }
  });

  ArduinoOTA.onEnd([]() {
    //    oled.println(" ");
    //    oled.println("OTA end");
    //    oled.display();
  });
}
