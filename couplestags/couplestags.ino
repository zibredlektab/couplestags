#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include <Adafruit_SSD1305.h>
#include <splash.h>
#include <Adafruit_GrayOLED.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include "config.h"
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>


#define LARGE_FONT &FreeSansBold12pt7b
#define MED_FONT &FreeSans9pt7b

Adafruit_SSD1305 oleds[4] {{128, 32, &Wire1, -1}, {128, 32, &Wire1, -1}, {128, 32, &Wire1, -1}, {128, 32, &Wire1, -1}};

#include <Wire.h>
QWIICMUX mux;

AdafruitIO_Feed* feeds[4] = {io.feed("couples-a-op"), io.feed("couples-b-op"), io.feed("couples-c-op"), io.feed("couples-d-op")}; 

char names[4][16];
int xpos, ypos;

unsigned long long timesincelastrefresh = 0;

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(!Serial && millis() < 3000) {};
  Serial.println("Serial initialized or timed out.");
  
  Wire1.setPins(SDA1, SCL1);
  Wire1.begin();

  if (!mux.begin(QWIIC_MUX_DEFAULT_ADDRESS, Wire1)) {
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
    if (!oleds[i].begin(SSD1305_I2C_ADDRESS, 0)) {
      Serial.print("Oled on port ");
      Serial.print(i);
      Serial.println(" failed to initialize");
      while(1) yield();
    } else {
      Serial.print("Oled on port ");
      Serial.print(i);
      Serial.println(" initialized");
      oleds[i].clearDisplay();
      oleds[i].setContrast(0x0);
      oleds[i].setCursor(0,0);
      oleds[i].setTextColor(WHITE);
      oleds[i].setRotation(2);
      oleds[i].println("Starting up...");
      oleds[i].display();
      oleds[i].setFont(LARGE_FONT);
    }

    feeds[i]->onMessage(handleChange);
    feeds[i]->get();

  }
}


void loop() {

  // process messages and keep connection alive
  io.run();
  if (millis() > timesincelastrefresh + 300000) {
    refreshOled();
  }

  if (millis() > 36000000) {
    for (int i = 0; i < 4; i++) {
      oleds[i].clearDisplay();
      oleds[i].display();
    }
    while(1);
  }

}

void refreshOled() {
  xpos = random(0, 10);
  ypos = random(18, 28);
  for (int i = 0; i < 4; i++) {
    mux.setPort(i);
    oleds[i].clearDisplay();
    oleds[i].setFont(LARGE_FONT);
    oleds[i].setCursor(xpos, ypos);
    oleds[i].print((char)(i + 65));
    oleds[i].print(":");
    if (strlen(names[i]) >= 7) {
      oleds[i].setFont(MED_FONT);
    }
    oleds[i].print(" ");
    oleds[i].print(names[i]);
    oleds[i].setCursor(xpos, ypos + 10*(i+1));
    oleds[i].display();
  }
  
  timesincelastrefresh = millis();
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

  refreshOled();
  
}
