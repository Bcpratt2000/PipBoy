#include <Mouse.h>
#include <SoftwareSerial.h>
#include <SPI.h>

int moveX, moveY, leftClick, rightClick;
int oldLeftClick = 0;
int oldRightClick = 0;

void setup() {

  delay(1000);
  Serial1.begin(115200);
  Mouse.begin();
  Serial.begin(9600);


}
void loop() {
  if(Serial1.read() == 'M'){
    checkMouse();
  }else{
    while(Serial1.available() > 0) {
      char t = Serial1.read();
    }
  }
  
  
//String temp = Serial1.readString();
//if(temp != -1)
//Serial.println(temp);
}



void checkMouse() {

    //get serial values
    moveX = Serial1.parseInt();
    moveY = Serial1.parseInt();
    leftClick = Serial1.parseInt();
    rightClick = Serial1.parseInt();

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
//    oldLeftClick = leftClick;
    oldRightClick = rightClick;

  
  
}

