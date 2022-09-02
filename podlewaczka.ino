/*********************************************************************
This is an example sketch for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <Encoder.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
//Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

struct MenuButton {
  int id;
  char* text;
};

const MenuButton buttons[] = {
  {
    id: 0,
    text: "Humidity lvl."
  },
  {
    id: 1,
    text: "Target hum."
  },
  {
    id: 2,
    text: "que?"
  },
  {
    id: 3,
    text: "duppa."
  }
};

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

int selectedButton = 0;
int oldSelectedButton = 0;

Encoder myEnc(2, 7);

long oldPosition  = -999;

void setup () {
  display.clearDisplay();
  display.begin();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setContrast(30);
  display.display();

  Serial.begin(9600);
}

void loop () {
  if (millis() % 100) {
    if (oldSelectedButton != selectedButton) {
      display.clearDisplay();
      oldSelectedButton = selectedButton;
    }
    for (int i = 0; i < sizeof(buttons) / sizeof(MenuButton); i++) {
      if (selectedButton == buttons[i].id) {
        display.setTextColor(WHITE, BLACK);
      }
      display.setCursor(14 - (sizeof(buttons[i].text) * 6), (i * 10) + 3);
      display.write(buttons[i].text);
      if (selectedButton == buttons[i].id) {
        display.setTextColor(BLACK);
      }
    }
    display.display();
  }

  long newPosition = myEnc.read();
  if (newPosition < oldPosition - 3 || newPosition > oldPosition + 3) {
    bool right = oldPosition > newPosition;
    oldPosition = newPosition;
    Serial.println(newPosition);

    int maxSize = sizeof(buttons) / sizeof(MenuButton);

    if (right) {
      Serial.println("right");
      if (selectedButton != maxSize - 1) {
        selectedButton += 1;
      } else {
        selectedButton = 0;
      }
    } else {
      if (selectedButton != 0) {
        selectedButton -= 1;
      } else {
        selectedButton = maxSize - 1;
      }
    }
  }
}
