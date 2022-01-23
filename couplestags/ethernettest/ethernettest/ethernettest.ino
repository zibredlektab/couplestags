#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

AdafruitIO_Feed* feeds[4] = {io.feed("couples-a-op"), io.feed("couples-b-op"), io.feed("couples-c-op"), io.feed("couples-d-op")}; 
Adafruit_SH1107 oled(64, 128, &Wire);

char names[4][16];

void setup() {

  oled.begin(0x3C, true);
  oled.setCursor(0, 20);
  oled.clearDisplay();
  oled.setRotation(1);
  oled.setTextColor(SH110X_WHITE);
  oled.println("Starting up...");
  oled.display();

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
  Serial.begin(115200);
  while(!Serial && millis() < 3000) {};
  Serial.println("Serial initialized or timed out.");

  
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

 // mux.setPort(port);
  oled.clearDisplay();
  oled.setCursor(0,20);
  oled.print((char)toupper(cam));
  oled.print(": ");
  oled.print(names[port]);
  oled.display();
  delay(500);
  
}
