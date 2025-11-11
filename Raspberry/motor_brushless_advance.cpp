#include <wiringPi.h>
#include <iostream>
#include <chrono>
#include <string> 

constexpr int MOVE = 1;
constexpr int STOP = 2;
constexpr bool DEBUG = true;
constexpr int DEBUG_INTERVAL = 200;

constexpr int ESC_PIN_LEFT = 13;
constexpr int ESC_PIN_RIGHT = 12;
constexpr int BUTTON_PIN = 27;


// PWM 50 Hz: 19.2MHz / 1920 / 200 = 50
constexpr int PWM_CLOCK = 1920;
constexpr int PWM_RANGE = 200;

int state = STOP;

std::string inputString;       //String with the velocity of left motor and right motor ["vLeft", "vRight"] 

int currentVLeft = 1500;
int currentVRight = 1500;
int targetVLeft = 1500;
int targetVRight = 1500;

std::string targetVLeftString;
std::string targetVRightString;

unsigned long lastDebugTime = 0;
unsigned long lastMoveTime = 0;


int usToPwmValue(int micros) {
    if (micros < 1000) micros = 1000;
    if (micros > 2000) micros = 2000;
    return micros / 100; // 1000us -> 10, 1500us -> 15, 2000us -> 20
}

void moveMotors(){
    //pwmWrite(ESC_PIN_RIGHT, usToPwmValue(currentVRight));
    //pwmWrite(ESC_PIN_RIGHT, usToPwmValue(currentVRight));
}

template <typename T>
T constrain(T x, T minVal, T maxVal) {
    if (x < minVal) return minVal;
    if (x > maxVal) return maxVal;
    return x;
}

void displayDebug() {
  if (!DEBUG) return;
  unsigned long now = millis();
  if (now - lastDebugTime < DEBUG_INTERVAL) return;
  lastDebugTime = now;
  //std::cout << "Left: " << currentVLeft<<" | Right: "<<currentVRight << std::endl;

}

void smoothOperator(){
  if(currentVLeft!=targetVLeft || currentVRight!=targetVRight){
    unsigned long now = millis();
    std::cout << "Left: " << currentVLeft<<" | Right: "<<currentVRight << std::endl;
    if (now - lastMoveTime >= 10) {
      lastMoveTime = now;
      if (currentVLeft < targetVLeft) currentVLeft++;
      else if (currentVLeft > targetVLeft) currentVLeft--;
  
      if (currentVRight < targetVRight) currentVRight++;
      else if (currentVRight > targetVRight) currentVRight--;
      }
  }
  moveMotors();
}

void handleLineInput() {
  // Si hay datos pendientes en stdin, los leemos
    if (std::cin.rdbuf()->in_avail() > 0) {
        if (std::getline(std::cin, inputString)) {
            // parseo
            size_t spaceIndex = inputString.find(' ');
            if (spaceIndex == std::string::npos) {
                targetVLeftString  = inputString;
                targetVRightString = "";
            } else {
                targetVLeftString  = inputString.substr(0, spaceIndex);
                targetVRightString = inputString.substr(spaceIndex + 1);
            }

            // convertir
            targetVLeft  = std::stoi(targetVLeftString.empty()  ? "1500" : targetVLeftString);
            targetVRight = std::stoi(targetVRightString.empty() ? "1500" : targetVRightString);

            // normalizar
            if (targetVLeft == 0)  targetVLeft  = 1500;
            else                   targetVLeft  = constrain(targetVLeft,  1000, 2000);

            if (targetVRight == 0) targetVRight = 1500;
            else                   targetVRight = constrain(targetVRight, 1000, 2000);

            std::cout << "Values received: "
                      << targetVLeft << ", " << targetVRight << std::endl;
        }
    }
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
  std::cout << "Iniciando en 2 sec..." << std::endl;
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "Error al inicializar wiringPi\n";
        return;
    }

    pinMode(ESC_PIN_LEFT, PWM_OUTPUT);
    pinMode(ESC_PIN_RIGHT, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);  
    pwmSetClock(PWM_CLOCK);
    pwmSetRange(PWM_RANGE);
    currentVLeft  = targetVLeft  = 1500;
    currentVRight = targetVRight = 1500;
    moveMotors();
    pinMode(BUTTON_PIN, INPUT);  // Configurar el pin como entrada
	  pullUpDnControl(BUTTON_PIN,PUD_UP);
    delay(2000);

    std::cout << "Iniciado" << std::endl;
  }

void checkButton(){
  int value = digitalRead(BUTTON_PIN);
  if(value == LOW){
    targetVLeft+= 10;
    targetVRight+= 10;
    delay(200);
  }
}


void loopOnce() {
  switch (state){
    case MOVE:
      checkButton();
      checkData();
      //handleLineInput();
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


int main() {checkData
    setup();
    while(true){
        loopOnce();
        delay(1);
    }
    return 0;
}