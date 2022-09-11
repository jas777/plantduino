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
    text: "Manual mode"
  }
};

int selectedButton = 0;
int oldSelectedButton = 0;
int menuPage = -1;

int targetHumidity = 1000;

bool buttonPress = false;

Encoder myEnc = Encoder(2, 7);

long oldPosition  = -999;

void setup () {
  display.clearDisplay();
  display.begin();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setContrast(30);
  display.display();

  pinMode(8, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop () {
  if (millis() % 100) {
    display.clearDisplay();
    switch (menuPage) {
      case -1:
        display.setTextSize(1);
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
        break;
      case 1:
        display.setTextSize(2);
        display.setCursor(3, 10);
        int tens = ((targetHumidity % 1000) / 100);
        int hundreds = targetHumidity / 1000;
        if (targetHumidity == 1000) display.write(hundreds + 48);
        if (!(tens == 0 && hundreds == 0)) display.write(tens + 48);
        int mappedVal = map(targetHumidity, 0, 1000, 0, 77);
        display.write((int) ((targetHumidity % 100) / 10) + 48);
        display.write(".");
        display.write((int) (targetHumidity % 10) + 48);
        display.write("%");
        display.drawRect(3, 30, 78, 10, BLACK);
        display.fillRect(3, 31, mappedVal, 8, BLACK);
    }
    display.display();
  }

  if (digitalRead(8) == LOW && !buttonPress) {
    buttonPress = true;
    if (menuPage != -1) {
      menuPage = -1;
    } else {
      menuPage = selectedButton;
    }
  }

  if (digitalRead(8) == HIGH && buttonPress) {
    buttonPress = false;
  }

  long newPosition = myEnc.read();
  if (newPosition < oldPosition - 3 || newPosition > oldPosition + 3) {
    bool right = oldPosition > newPosition;
    oldPosition = newPosition;

    int maxSize = sizeof(buttons) / sizeof(MenuButton);

    if (right) {
      switch (menuPage) {
        case -1:
          if (selectedButton != maxSize - 1) {
            selectedButton += 1;
          } else {
            selectedButton = 0;
          }
          break;
        case 1:
          if (targetHumidity != 0) {
            targetHumidity -= 5;
          }
          break;
      }
    } else {
      switch (menuPage) {
        case -1:
          if (selectedButton != 0) {
            selectedButton -= 1;
          } else {
            selectedButton = maxSize - 1;
          }
          break;
        case 1:
          if (targetHumidity != 1000) {
            targetHumidity += 5;
          }
          break;
      }
    }
  }
}
