#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdint.h>
//#include <SD.h>
#include "lib/Touch_Screen_Driver-master/SeeedTouchScreen.cpp"
#include "lib/TFT_Touch_Shield_V2-master/TFTv2.cpp"
#include <SPI.h>

//Arduino uses RGB565 colors
#define BKG_COLOR  0x11C1

#define TIME_TO_COMPILE 10

unsigned long oldTimeRunning = 0;
unsigned char page = 1;
unsigned char numOfPages = 4;
unsigned char hours = (String(__TIME__).substring(0, 2)).toInt()%12;
unsigned char minutes = (String(__TIME__).substring(3, 5)).toInt();
unsigned char seconds = (String(__TIME__).substring(6, 8)).toInt()+TIME_TO_COMPILE;
boolean overBright = false;
boolean mouseEnabled = false;

char mouseSensitivity = 4;
short MAIN_COLOR = 0x07E0;

//10 char max
String Titles[] = {
  "Mouse", "Home", "Settings", "Overbright"
};

Point p;

TouchScreen ts = TouchScreen (XP, YP, XM, YM);
SoftwareSerial slave = SoftwareSerial(2, 3); //tx=3; rx=2. Used to talk to other chip


void setup() {

  Tft.TFTinit();
  Serial.begin(115200);
  slave.begin(115200);
  redrawScreen();
}//end setup



void loop() {

  p = ts.getPoint();
  if (p.z != 0) {
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
    drawScreen(p.x, p.y);
  }
  else if (Titles[page] == "Mouse") {

  }
  else if (Titles[page] == "Home") {
    if (updateTime()) {
      //update the space where the time is on the home screen
      Tft.fillRectangle(55, 240, 190, 60, BKG_COLOR);
      drawScreen(-1, -1);
    }
  }
  else if (Titles[page] == "Overbright") {

  }
  else {

  }

}//end loop




void drawScreen(int pX, int pY) {
  //_____________________________________________________________
  //                           Upper
  //_____________________________________________________________

  if (!overBright) {
    //seperation
    Tft.drawLine(0, 60, 240, 60, MAIN_COLOR);
    Tft.drawLine(0, 59, 240, 59, MAIN_COLOR);

    //Draw center text
    drawString(Titles[page], 60, 18, 2, MAIN_COLOR);

    //right triangle
    Tft.drawTriangle(200, 10, 200, 40, 230, 25, MAIN_COLOR);
    //left triangle
    Tft.drawTriangle(40, 10, 40, 40, 10, 25, MAIN_COLOR);

    //Check touchscreen
    //Left arrow
    if (pX < 60 && pX > 0 && pY < 60 && pY > 0) {
      if (page!=0) {
        page --;
      }
      redrawScreen();
    }
    //Right arrow
    if (pX < 240 && pX > 180 && pY < 60 && pY > 0) {
      if (page != numOfPages-1) {
        page++;
      }
      redrawScreen();
    }
  }


  
  //_____________________________________________________________
  //                           Lower
  //_____________________________________________________________
  if (Titles[page] == "Mouse") {
    Tft.drawLine(0, 280, 320, 280, MAIN_COLOR);
    drawString("Disabled", 20, 290, 2, MAIN_COLOR);
    if (pY > 280 && pY < 320) {
      mouseEnabled = !mouseEnabled;
      startMouse();
    }

  }

  else if (Titles[page] == "Home") {
    //draw Logo
    drawString("Pip-Boy", 15, 100, 5, MAIN_COLOR);
    drawString("2OOO Mini", 40, 150, 3, MAIN_COLOR);

    //draw Time
    drawString((String)(int)hours, 55, 240, 4, MAIN_COLOR);
    drawString(":", 105, 240, 4, MAIN_COLOR);
    drawString((String)(int)minutes, 125, 240, 4, MAIN_COLOR);

  }

  else if (Titles[page] == "Settings"){
    Tft.fillRectangle(70, 70, 100, 36, MAIN_COLOR);
    drawString("Color",80 ,75, 2, BKG_COLOR);
    if (pX < 60 && pX > 0 && pY < 60 && pY > 0) {
      drawScreen(-1, -1);
    }
  }

  else if (Titles[page] == "Overbright") {
    if (pY > 60) {
      if (overBright) {
        setOverBright(false);
      }
      else if (!overBright) {
        setOverBright(true);
      }

    }
    drawString("Tap screen to", 45, 150, 2, MAIN_COLOR);
    drawString("toggle OverBright", 20, 200, 2, MAIN_COLOR);
  }
  else {
    drawString("There has been no backend written for this page", 10, 100, 1, MAIN_COLOR);
  }


}


//This is here because the TFT.drawString does not accept variables
void drawString(String string, int poX, int poY, int size, int fgcolor) {
  for (int index = 0; index < string.length(); index++) {
    Tft.drawChar(string[index], poX + (6 * size * index), poY, size, fgcolor);
  }
}



void redrawScreen() {
  Tft.fillRectangle(0, 0, 240, 320, BKG_COLOR);
  drawScreen(-1, -1);
}
void redrawScreen(int pX, int pY) {
  Tft.fillRectangle(0, 0, 240, 320, BKG_COLOR);
  drawScreen(pX, pY);
}
void redrawLower() {
  Tft.fillRectangle(0, 61, 240, 320, BKG_COLOR);
  drawScreen(-1, -1);
}
void redrawUpper() {
  Tft.fillRectangle(0, 0, 240, 60, BKG_COLOR);
  drawScreen(-1, -1);
}

void drawCRTLines() { //blanks out every other line
  for (int index; index < 160; index++) {
    Tft.drawLine(0, index * 2, 240, index * 2, BKG_COLOR);

  }
}

//refresh system time vatiables
boolean updateTime() {
  boolean timeUpdated = false;
  while (millis() - 1000 >= oldTimeRunning) {

    seconds = seconds + 1;

    if (seconds >= 60) {
      minutes = minutes + 1;
      seconds = 0;
      timeUpdated = true;
    }
    if (minutes >= 60) {
      hours = hours + 1;
      minutes = 0;
    }
    if (hours >= 13) {
      hours = 1;
      minutes = 0;
    }
    oldTimeRunning += 1000;
  }
  return timeUpdated;
}

void setOverBright(boolean state) {
  overBright = state;
  if (state) {
    Tft.fillRectangle(0, 0, 240, 320, WHITE);
  }
  else{
    redrawScreen();
  }
}

//start the service that makes the arduino act as a mouse for the computer;
void startMouse() {
  int leftMouse;
  int rightMouse;
  int oldX;
  int oldY;
  int moveX;
  int moveY;
  boolean isFirst = true;

  //blank screen
  Tft.fillRectangle(0, 0, 240, 320, BKG_COLOR);

  //draw the button to disable the mouse and resume loop()
  Tft.fillRectangle(0, 280, 240, 320, MAIN_COLOR);
  drawString("Enabled", 20, 290, 2, BKG_COLOR);

  //draw button(s) to click
  //Left
  Tft.drawRectangle(0, 220, 120, 280, MAIN_COLOR);
  drawString("Left", 20, 230, 2, MAIN_COLOR);
  //Right
  Tft.drawRectangle(120, 220, 240, 280, MAIN_COLOR);
  drawString("Right", 140, 230, 2, MAIN_COLOR);

  //main loop
  while (mouseEnabled) {
    p = ts.getPoint();
    if (p.z != 0) {
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
      //disable if the disable button is pressed
      if (p.y > 280 && p.x < 320) {
        mouseEnabled = false;
        slave.println("modeReset");
        redrawScreen();
      }

      //check button area

      //dont move mouse if buttons are being pressed
      moveX = 0;
      moveY = 0;

      //check left mouse button
      if (p.x > 0 && p.x < 120 && p.y > 220 && p.y < 280) {
        leftMouse = 1;
      }
      else {
        leftMouse = 0;
      }

      //chect right mouse button
      if (p.x > 120 && p.x < 240 && p.y > 220 && p.y < 280) {
        rightMouse = 1;
      }
      else {
        rightMouse = 0;
      }

      //if touchpad area is being pressed
      if (p.x > 0 && p.x < 240 && p.y > 0 && p.y < 220) {
        if (isFirst) {
          moveX = 0;
          moveY = 0;
        }
        else {
          moveX = p.x - oldX;
          moveY = p.y - oldY;

          if(abs(moveX * mouseSensitivity) <=3)
            moveX=0;
          if(abs(moveY * mouseSensitivity) <=3)
            moveY=0;
        }
        oldX = p.x;
        oldY = p.y;

      }
      slave.println('M');
      //change in x
      slave.println((int)(moveX * mouseSensitivity));
      //change in y
      slave.println((int)(moveY * mouseSensitivity));
      //leftMouse
      slave.println((int)leftMouse);
      //rightMouse
      slave.println((int)rightMouse);

      isFirst = false;

    }
    else {
      isFirst = true;
    }
  }
  delay(20);
}
