#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include <Fonts/FreeSansBold12pt7b.h>

#include <Wire.h>

#define LARGE_FONT &FreeSansBold12pt7b

AdafruitIO_Feed* feeds[4] = {io.feed("couples-a-op"), io.feed("couples-b-op"), io.feed("couples-c-op"), io.feed("couples-d-op")}; 
Adafruit_SSD1305 oled(128, 32, &Wire1, -1);

char names[4][16];
int xpos, ypos;

QWIICMUX mux;

unsigned long long timesincelastrefresh = 0;

void setup() {
  
  
  delay(1000);
  Serial.begin(115200);
  while(!Serial && millis() < 3000) {};
  Serial.println("Serial initialized or timed out.");


  Wire1.setPins(SDA1, SCL1);
  Wire1.begin();
  
  if (!mux.begin(0x70, Wire1)) {
    Serial.println("Mux not detected.");
    while (1);
  }

  mux.setPort(0);

  if (!oled.begin(SSD1305_I2C_ADDRESS, 0)) {
    Serial.println("failed to start oled");
    while(1);
  }
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.setTextColor(WHITE);
  oled.setRotation(2);
  oled.setContrast(0x0);
  oled.println("Starting up...");
  oled.display();
  oled.setFont(LARGE_FONT);

  Serial.print("Connecting to Adafruit IO");
  io.connect();
  Serial.print("...");
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  for (int i = 0; i < 4; i++) {
    feeds[i]->onMessage(handleChange);
    feeds[i]->get();
  }
}

void loop() {
  io.run();
  if (millis() > timesincelastrefresh + 300000) {
    refreshOled();
  }

  if (millis() > 36000000) {
    oled.clearDisplay();
    oled.display();
    while(1);
  }
}

void refreshOled() {
  xpos = random(0, 10);
  ypos = random(18, 28);
  oled.clearDisplay();
  oled.setCursor(xpos, ypos);
  for (int i = 0; i < 4; i++) {
    mux.setPort(i);
    oled.print((char)(i + 65));
    oled.print(": ");
    oled.print(names[i]);
    oled.setCursor(xpos, ypos + 10*(i+1));
    oled.display();
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

  Serial.print("saved name as '");
  Serial.print(names[port]);
  Serial.println("'");
  refreshOled();
}
