#include <Adafruit_SSD1305.h>
#include <splash.h>
#include <Adafruit_GrayOLED.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Wire.h>
#include "config.h"
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>


#define LARGE_FONT &FreeSerifBold12pt7b
#define MED_FONT &FreeSerif9pt7b
#define TIMEOUT 3000
#define UPDATE 180000

#define LABEL 'D'

Adafruit_SSD1305 display(128, 32, &Wire1, -1);


AdafruitIO_Feed *feeda = io.feed("couples-a-op");
AdafruitIO_Feed *feedb = io.feed("couples-b-op");
AdafruitIO_Feed *feedc = io.feed("couples-c-op");
AdafruitIO_Feed *feedd = io.feed("couples-d-op");

AdafruitIO_Feed *feed1 = io.feed("couples-1st");
AdafruitIO_Feed *feed2 = io.feed("couples-2nd");

AdafruitIO_Feed* feed;
bool ACs = false;

char name[16];
char first[16];
char second[16];

unsigned long timesincelastrefresh = 0;

void setup() {

  Wire1.setPins(SDA1, SCL1);
  Wire1.begin();

  // start the serial connection
  Serial.begin(9600);

  // wait for serial monitor to open
  unsigned long long timeserialstarted = millis();
  while(!Serial && timeserialstarted + TIMEOUT > millis()) { delay(10); }
  Serial.println("Serial initialized or timed out.");


  while (!display.begin(SSD1305_I2C_ADDRESS, 0)) {
    Serial.println("Display failed to initialize");
    delay(100);
  } 

  Serial.println("Display initialized");
  display.clearDisplay();
  display.setContrast(0x88);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setRotation(2);
  display.setTextWrap(1);
  display.display();


  switch(LABEL) {
    case ('A'): {
      feed = feeda;
      break;
    }
    case ('B'): {
      feed = feedb;
      break;
    }
    case ('C'): {
      feed = feedc;
      break;
    }
    case ('D'): {
      feed = feedd;
      break;
    }
    case ('F'): {
      ACs = true;
      feed1->onMessage(handleChange);
      feed2->onMessage(handleChange);
      break;
    }
  }

  if (!ACs) {
    feed->onMessage(handleChange);
    Serial.print("Selected feed is ");
    Serial.println(feed->name);
  }


  io.connect();


  printToDisplay("Connecting to AIO");
  Serial.print("Connecting to Adafruit IO");

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {

    if (io.status() == 0) {
      io.connect();
    }
    Serial.println((char*)io.statusText());
    printToDisplay(".");
    delay(2000);
  }
  
  // we are connected
  Serial.println(io.statusText());
  printToDisplay((char*)io.statusText());


  if (ACs) {
    feed1->get();
    feed2->get();
  } else {
    feed->get();
  }

  timesincelastrefresh = millis();
}


void loop() {

  // process messages and keep connection alive
  io.run();

  if (millis() > timesincelastrefresh + UPDATE) {
    refreshDisplay();
    updateFeed();
  }

  if (millis() > 36000000) {
    display.clearDisplay();
    display.display();
    while(1);
  }

}

void printToDisplay(const char* str) {
  display.print(str);
  display.display();
}

void refreshDisplay() {
  
  static int xpos, ypos;
  xpos = random(0, 8);
  ypos = random(15, 23);

  display.setTextWrap(0);
  display.clearDisplay();
  if (ACs) {
    display.setCursor(0, 12);
    display.setFont(MED_FONT);
    display.print("1");
    display.setFont(NULL);
    display.print("st");
    display.setFont(MED_FONT);
    display.print(":");
    display.print(first);
    display.setCursor(0, 26);
    display.print("2");
    display.setFont(NULL);
    display.print("nd");
    display.setFont(MED_FONT);
    display.print(":");
    display.print(second);
  } else {
    display.setCursor(xpos, ypos);
    display.setFont(LARGE_FONT);
    display.print((char)LABEL);
    display.print(":");
    if (strlen(name) >= 8) display.setFont(MED_FONT);
    display.print(" ");
    display.print(name);
  }

  display.display();
  
}

void handleChange(AdafruitIO_Data *data) {

  Serial.print("Name of ");
  Serial.print(data->feedName());
  Serial.print(" is now: ");
  Serial.println(data->value());

  char* deststr;

  if (!strcmp(data->feedName(), "couples-1st")) {
    deststr = first;
    Serial.print("1st name is ");
  } else if (!strcmp(data->feedName(), "couples-2nd")) {
    deststr = second;
    Serial.print("2nd name is ");
  } else {
    deststr = name;
    Serial.print("name is ");
  }
  
  strncpy(deststr, data->value(), 16);

  Serial.println(deststr);

  refreshDisplay();
}

void updateFeed() {
  Serial.println("saving current name to keep feed active");
  if (ACs) {
    feed1->save(first);
    feed2->save(second);
  } else {
    feed->save(name);
  }

  timesincelastrefresh = millis();
}
