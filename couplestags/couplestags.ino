#include <SparkFun_I2C_Mux_Arduino_Library.h>

#include <Adafruit_SSD1305.h>
#include <splash.h>
#include <Adafruit_GrayOLED.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

// Adafruit IO Multiple Feed Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

Adafruit_SSD1305 aoled(128, 32);
Adafruit_SSD1305 boled(128, 32);
Adafruit_SSD1305 coled(128, 32);
Adafruit_SSD1305 doled(128, 32);

QWIICMUX mux;

AdafruitIO_Feed *AOp = io.feed("couples-a-op");
AdafruitIO_Feed *BOp = io.feed("couples-b-op");
AdafruitIO_Feed *COp = io.feed("couples-c-op");
AdafruitIO_Feed *DOp = io.feed("couples-d-op");

char aname[16];
char bname[16];
char cname[16];
char dname[16];

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  if (mux.begin() == false) {
    Serial.println("Mux not detected.");
    while (1);
  }

  mux.setPort(1);
  if(!aoled.begin(0x3C, 0)) {
    Serial.println(F("A SSD1305 allocation failed"));
    while(1);
  } else {
    Serial.println(F("A SSD1305 allocation succeeded"));
  }
  
  mux.setPort(2);
  if(!boled.begin(0x3C, 0)) {
    Serial.println(F("B SSD1305 allocation failed"));
    while(1);
  } else {
    Serial.println(F("B SSD1305 allocation succeeded"));
  }

  mux.setPort(3);
  if(!coled.begin(0x3C, 0)) {
    Serial.println(F("C SSD1305 allocation failed"));
    while(1);
  } else {
    Serial.println(F("C SSD1305 allocation succeeded"));
  }
  
  mux.setPort(4);
  if(!doled.begin(0x3C, 0)) {
    Serial.println(F("D SSD1305 allocation failed"));
    while(1);
  } else {
    Serial.println(F("D SSD1305 allocation succeeded"));
  }

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // attach message handler
  AOp->onMessage(handleChange);
  BOp->onMessage(handleChange);
  COp->onMessage(handleChange);
  DOp->onMessage(handleChange);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  // make sure all feeds get their current values right away
  AOp->get();
  BOp->get();
  COp->get();
  DOp->get();

}

void loop() {

  // process messages and keep connection alive
  io.run();

}

void handleChange(AdafruitIO_Data *data) {

  Serial.print("Name of ");

  // since we are using the same function to handle
  // messages for two feeds, we can use feedName() in
  // order to find out which feed the message came from.
  Serial.print(data->feedName());
  Serial.print(" is now: ");

  Serial.println(data->value());

  char* nametochange;

  if (strcmp(data->feedName(), "couples-a-op") == 0) {
    nametochange = aname;
  } else if (strcmp(data->feedName(), "couples-b-op") == 0) {
    nametochange = bname;
  } else if (strcmp(data->feedName(), "couples-c-op") == 0) {
    nametochange = cname;
  } else if (strcmp(data->feedName(), "couples-d-op") == 0) {
    nametochange = dname;
  }


  for (int i = 0; i < 16; i++) {
    nametochange[i] = data->value()[i];
    if (data->value()[i] == '\0') {
      break;
    }
  }

}
