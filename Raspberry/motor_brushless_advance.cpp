#include <wiringPi.h>
#include <iostream>
#include <chrono>
#include <string> 
#include <softPwm.h>
#include <thread>


constexpr int MOVE = 1;
constexpr int STOP = 2;
constexpr bool DEBUG = true;
constexpr int DEBUG_INTERVAL = 200;

constexpr int ESC_PIN_LEFT = 18;
constexpr int ESC_PIN_RIGHT = 12;

constexpr int BUTTON_PIN_POS = 27;
constexpr int BUTTON_PIN_NEG = 22;

int state = STOP;


int currentVLeft = 1500;
int currentVRight = 1500;
int targetVLeft = 1500;
int targetVRight = 1500;



unsigned long lastDebugTime = 0;
unsigned long lastMoveTime = 0;


int usToPwmValue(int micros) {
    if(micros == 0) micros = 1500; 
    if (micros < 1000) micros = 1000;
    if (micros > 2000) micros = 2000;
    return micros / 100; // 1000us -> 10, 1500us -> 15, 2000us -> 20
}

void moveMotors(){
    softPwmWrite(ESC_PIN_LEFT, usToPwmValue(currentVLeft));
    //softPwmWrite(ESC_PIN_RIGHT, usToPwmValue(currentVRight));
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

void escCalibrate() {
    std::cout << "Starting ESC calibration..." << std::endl;
    softPwmWrite(ESC_PIN_LEFT, usToPwmValue(2000));
    softPwmWrite(ESC_PIN_RIGHT, usToPwmValue(2000));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    softPwmWrite(ESC_PIN_LEFT, usToPwmValue(1000));
    softPwmWrite(ESC_PIN_RIGHT, usToPwmValue(1000));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Calibration done!" << std::endl;
}

void setup() {
  std::cout << "Iniciando en 4 sec..." << std::endl;
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "Error al inicializar wiringPi\n";
        return;
    }

 
    softPwmCreate(ESC_PIN_LEFT, 0, 100);
    softPwmCreate(ESC_PIN_RIGHT, 0, 100);

    escCalibrate();
    pinMode(BUTTON_PIN_POS, INPUT);  // Configurar el pin como entrada
    pinMode(BUTTON_PIN_NEG, INPUT);  // Configurar el pin como entrada
	  pullUpDnControl(BUTTON_PIN_POS,PUD_UP);
	  pullUpDnControl(BUTTON_PIN_NEG,PUD_UP);
    std::cout << "Iniciado" << std::endl;
  }

void checkButtons(){
  int value_pos = digitalRead(BUTTON_PIN_POS);
  if(value_pos == LOW){
    targetVLeft = constrain(targetVLeft + 10, 1000, 2000);
    targetVRight = constrain(targetVRight + 10, 1000, 2000);
    delay(200);
  }
  int value_neg = digitalRead(BUTTON_PIN_NEG);
  if(value_neg == LOW){
    targetVLeft = constrain(targetVLeft - 10, 1000, 2000);
    targetVRight = constrain(targetVRight - 10, 1000, 2000);
	delay(200);
  }
}


void loopOnce() {
  switch (state){
    case MOVE:
      checkButtons();
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


int main() {
    setup();
    while(true){
        loopOnce();
        delay(1);
    }
    return 0;
}
