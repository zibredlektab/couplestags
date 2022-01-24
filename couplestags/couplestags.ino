#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include <Adafruit_SSD1305.h>
#include <splash.h>
#include <Adafruit_GrayOLED.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include "config.h"

Adafruit_SSD1305 oleds[4] {{128, 32}, {128, 32}, {128, 32}, {128, 32}};

QWIICMUX mux;

AdafruitIO_Feed* feeds[4] = {io.feed("couples-a-op"), io.feed("couples-b-op"), io.feed("couples-c-op"), io.feed("couples-d-op")}; 

char names[4][16];

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  if (!mux.begin()) {
    Serial.println("Mux not detected.");
    while (1);
  }

  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  // we are connected
  Serial.println();
  Serial.println(io.statusText());


  for (int i = 0; i < 4; i++) {
    mux.setPort(i);
    if (!oleds[i].begin(0x3c, 0)) {
      Serial.print("Oled on port ");
      Serial.print(i+1);
      Serial.println(" failed to initialize");
      while(1);
    } else {
      Serial.print("Oled on port ");
      Serial.print(i);
      Serial.println(" initialized");
    }

    feeds[i]->onMessage(handleChange);
    feeds[i]->get();
  }
}

void loop() {

  // process messages and keep connection alive
  io.run();

}

void handleChange(AdafruitIO_Data *data) {

  Serial.print("Name of ");
  Serial.print(data->feedName());
  Serial.print(" is now: ");
  Serial.println(data->value());

  if (strcmp(data->feedName(), "couples-a-op") == 0) {
    updateOled('A', data->value());
  } else if (strcmp(data->feedName(), "couples-b-op") == 0) {
    updateOled('B', data->value());
  } else if (strcmp(data->feedName(), "couples-c-op") == 0) {
    updateOled('C', data->value());
  } else if (strcmp(data->feedName(), "couples-d-op") == 0) {
    updateOled('D', data->value());
  }
  
}

void updateOled(const char cam, char* newname) {

  int port = toupper(cam) - 65;

  for (int i = 0; i < 16; i++) {
    names[port][i] = newname[i];
    if (newname[i] == '\0') {
      break;
    }
  }

  names[port][15] = '\0';

  mux.setPort(port);
  oleds[port].clearDisplay();
  oleds[port].setCursor(0,10);
  oleds[port].print(toupper(cam));
  oleds[port].print(": ");
  oleds[port].print(names[port]);
  oleds[port].display();
  
}
