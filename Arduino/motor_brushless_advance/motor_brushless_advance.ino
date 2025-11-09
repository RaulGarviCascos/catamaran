
#include <Servo.h>

#define MOVE 1
#define STOP 2
#define DEBUG true
#define DEBUG_INTERVAL 200

Servo escRight;
Servo escLeft;

int state = STOP;

String inputString = "";       //String with the velocity of left motor and right motor ["vLeft", "vRight"] 

int currentVLeft = 1500;
int currentVRight = 1500;
int targetVLeft = 1500;
int targetVRight = 1500;

String targetVLeftString;
String targetVRightString;

unsigned long lastDebugTime = 0;
unsigned long lastMoveTime = 0;


void moveMotors(){
  //escLeft.writeMicroseconds(currentVLeft);
  //escRight.writeMicroseconds(currentVRight);

  
  /*Serial.print("muevo motor izquierdo:");
  Serial.println(currentVLeft);
  Serial.print("muevo motor derecho");
  Serial.println(currentVRight);*/
}

void displayDebug() {
  if (!DEBUG) return;
  unsigned long now = millis();
  if (now - lastDebugTime < DEBUG_INTERVAL) return;
  lastDebugTime = now;

  Serial.print("Left:");
  Serial.print(currentLeft);
  Serial.print(" Right:");
  Serial.println(currentRight);
}


void smoothOperator(){
  if(currentVLeft!=targetVLeft || currentVRight!=targetVRight){
    unsigned long now = millis();
    if (now - lastMoveTime >= 10) {
      lastMoveTime = now;
      if (currentLeft < targetLeft) currentLeft++;
      else if (currentLeft > targetLeft) currentLeft--;
  
      if (currentRight < targetRight) currentRight++;
      else if (currentRight > targetRight) currentRight--;
      }
  }
  moveMotors();
}

void handleSerialCommand() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n');
    getValues();
    targetVLeft = targetVLeftString;
    targetVRight = targetVRightString;
    Serial.print("values received: ");
    Serial.print(targetVLeft);
    Serial.print(", ");
    Serial.println(targetVRight);
    checkData();
  }
}

void getValues(){
   // --- Split command and value ---
  int spaceIndex = inputString.indexOf(' ');
  targetVLeftString = (spaceIndex == -1) ? inputString : inputString.substring(0, spaceIndex);
  targetVRightString = (spaceIndex == -1) ? "" : inputString.substring(spaceIndex + 1);

}

void checkData(){
  if(targetVLeft == 0){
    targetVLeft = 1500;
  }else {
     targetVLeft = constrain(targetVLeft, 1000, 2000);
  }
  if(targetVRight==0){
    targetVRight = 1500;
  }else{
     targetVRight = constrain(targetVRight, 1000, 2000);
  }
}

void setup() {
  /*
  
  escRight.attach(9); // ESC signal on pin 9
  escLeft.attach(11);
  
  Serial.begin(9600);
  Serial.println("Starting ESC calibration...");
  delay(2000);
  // ESC calibration (optional but recommended)
  escRight.writeMicroseconds(2000);  // max throttle
  escLeft.writeMicroseconds(2000);
  delay(2000);
  escRight.writeMicroseconds(1000);  // min throttle
  escLeft.writeMicroseconds(1000);
  delay(2000);
  Serial.println("Calibration done!");
  
  Serial.println("initial value: 1500");
  */
  Serial.begin(9600);
  Serial.println("Iniciado...");
  delay(2000);

}

void loop() {
  switch (state){
    case MOVE:
      handleSerialCommand();
      smoothOperator();
      displayDebug();
      break;
    case STOP:
      targetVLeft = 1500;
      targetVRight = 1500;
      state = MOVE;
      displayDebug();
      break;
  }
}
