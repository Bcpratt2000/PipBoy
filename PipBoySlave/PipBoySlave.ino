#include <Mouse.h>
#include <SoftwareSerial.h>
#include <SPI.h>

SoftwareSerial master = SoftwareSerial(8, 9); //use pins 8 and 9 to talk to other chip

int moveX, moveY, leftClick, rightClick;
int oldLeftClick = 0;
int oldRightClick = 0;
int mode;

void setup() {

  delay(1000);
  master.begin(9600);
  master.listen();
  master.setTimeout(3);
  Mouse.begin();
  Serial.begin(9600);


}

void loop() {

  checkMouse();

}



void checkMouse() {
  if (master.read() == 'M') {
    //get serial values
    moveX = master.parseInt();
    moveY = master.parseInt();
    leftClick = master.parseInt();
    rightClick = master.parseInt();

    //move mouse
    Mouse.move(moveX, moveY, 0);

    //Left Click
    if (leftClick == 1 && oldLeftClick == 0) {
      Mouse.click(MOUSE_LEFT);
    }


    //Right Click
    if (rightClick == 1 && oldRightClick == 0) {
      Mouse.click(MOUSE_RIGHT);
    }
    oldLeftClick = leftClick;
    oldRightClick = rightClick;

  }
}

