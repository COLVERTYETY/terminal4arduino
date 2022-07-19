#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include <FreeDefaultFonts.h>

#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define WHITE   0xFFFF
#define GREY    0x8410


long linuxBaud = 250000;
boolean commandMode = false;

void setup(void)
{
  

  // init computer serial
  Serial.begin(115200);

  // init tft display
  uint16_t ID = tft.readID();
  Serial.print("found ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; //force ID if write-only display
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN, BLACK);
  tft.setTextSize(1);
  Serial.println("tft display initialized");

  // init linux serial
  Serial1.begin(linuxBaud);
}

void loop() {

  // copy from virtual serial line to uart and vice versa

  if (Serial.available()) {           // got anything from USB-Serial?

    char c = (char)Serial.read();     // read from USB-serial

    if (commandMode == false) {       // if we aren't in command mode...

      if (c == '~') {                 //    Tilde '~' key pressed?

        commandMode = true;           //       enter in command mode

      } else {

        Serial1.write(c);             //    otherwise write char to Linux

      }

    } else {                          // if we are in command mode...

      if (c == '0') {                 //     '0' key pressed?

        Serial1.begin(57600);         //        set speed to 57600

        Serial.println("Speed set to 57600");

      } else if (c == '1') {          //     '1' key pressed?

        Serial1.begin(115200);        //        set speed to 115200

        Serial.println("Speed set to 115200");

      } else if (c == '2') {          //     '2' key pressed?

        Serial1.begin(250000);        //        set speed to 250000

        Serial.println("Speed set to 250000");

      } else if (c == '3') {          //     '3' key pressed?

        Serial1.begin(500000);        //        set speed to 500000

        Serial.println("Speed set to 500000");

      } else if (c == '~') {

        Serial1.write((uint8_t *)"\xff\0\0\x05XXXXX\x0d\xaf", 11);

        Serial.println("Sending bridge's shutdown command");

      } else {                        //     any other key pressed?

        Serial1.write('~');           //        write '~' to Linux

        Serial1.write(c);             //        write char to Linux

      }

      commandMode = false;            //     in all cases exit from command mode

    }

  }

  if (Serial1.available()) {          // got anything from Linux?

    char c = (char)Serial1.read();    // read from Linux

    Serial.write(c);               // write to USB-serial
    // return cursor to top if out of bounds
    if (tft.getCursorY() > tft.height() - 10) {
      tft.setCursor(0, 0);
      //clear the screen
      tft.fillScreen(BLACK);
    }
    tft.print(c);


  }
}