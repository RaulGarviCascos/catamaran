#include <Servo.h>

#define STATE_CALIBRATING  1
#define STATE_RUNNING      2
#define STATE_STOP         3

#define DEBUG true
#define DEBUG_INTERVAL 200

Servo escRight;
Servo escLeft;

unsigned long lastDebugTime = 0;
unsigned long lastMoveTime = 0;

int state = STATE_RUNNING;

String inputString;
int vNew = 1500;

int targetLeft = 1500;
int targetRight = 1500;
int currentLeft = 1500;
int currentRight = 1500;

void moveMotors() {
  unsigned long now = millis();
  if (now - lastMoveTime >= 10) {
    lastMoveTime = now;

    if (currentLeft < targetLeft) currentLeft++;
    else if (currentLeft > targetLeft) currentLeft--;

    if (currentRight < targetRight) currentRight++;
    else if (currentRight > targetRight) currentRight--;

    //escLeft.writeMicroseconds(currentLeft);
    //escRight.writeMicroseconds(currentRight);
  }
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


void handleSerialCommand() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n');
    parseCommand();
  }
}

void parseCommand() {
  inputString.trim();
  if (inputString.isEmpty()) return;

  // --- Split command and value ---
  int spaceIndex = inputString.indexOf(' ');
  String cmd = (spaceIndex == -1) ? inputString : inputString.substring(0, spaceIndex);
  String value = (spaceIndex == -1) ? "" : inputString.substring(spaceIndex + 1);

  // --- Normalize command ---
  cmd.toLowerCase();

  // --- Parse and constrain numeric value ---
  if (value.length() > 0) {
    vNew = constrain(value.toInt(), 1000, 2000);
  }

  // --- Apply motor targets based on command ---
  if (cmd == "l" || cmd == "left" || cmd == "a") {
    targetLeft = vNew;
    targetRight = 1500;

  } else if (cmd == "r" || cmd == "right" || cmd == "d") {
    targetLeft = 1500;
    targetRight = vNew;

  } else if (cmd == "f" || cmd == "forward" || cmd == "w") {
    targetLeft = vNew;
    targetRight = vNew;

  } else if (cmd == "b" || cmd == "backward" || cmd == "s") {
    targetLeft = 3000 - vNew;
    targetRight = 3000 - vNew;

  } else if (cmd == "stop" || cmd == "q") {
    targetLeft = 1500;
    targetRight = 1500;
    currentLeft = 1500;
    currentRight = 1500;
    state = STATE_STOP;

  } else {
    Serial.println(F("Unknown command."));
    return;
  }

  // --- Serial feedback ---
  Serial.print(F("Command: "));
  Serial.print(cmd);
  Serial.print(F(" | Value: "));
  Serial.println(vNew);
}



void setup() {
  escRight.attach(9);
  escLeft.attach(11);

  Serial.begin(115200);
  Serial.println("Calibrating ESCs...");
  delay(2000);

  escRight.writeMicroseconds(2000);
  escLeft.writeMicroseconds(2000);
  delay(2000);
  escRight.writeMicroseconds(1000);
  escLeft.writeMicroseconds(1000);
  delay(2000);

  Serial.println("Calibration complete");
  Serial.println("Ready");
  state = STATE_RUNNING;
}

void loop() {
  switch (state) {
    case STATE_CALIBRATING:
      break;

    case STATE_RUNNING:
      handleSerialCommand();
      moveMotors();
      displayDebug();
      break;

    case STATE_STOP:
      //escLeft.writeMicroseconds(1500);
      //escRight.writeMicroseconds(1500);
      displayDebug();
      break;
  }
}
