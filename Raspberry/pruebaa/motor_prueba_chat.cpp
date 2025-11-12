#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#define MOVE 1
#define STOP 2
#define DEBUG true
#define DEBUG_INTERVAL 200  // ms

// Pines en modo WiringPi (ajusta según tu conexión)
#define ESC_LEFT_PIN 0   // GPIO17
#define ESC_RIGHT_PIN 1  // GPIO18

// Puerto serie (ajusta según tu conexión)
#define SERIAL_PORT "/dev/ttyAMA0"  // o "/dev/ttyUSB0" si usas adaptador USB–TTL

// Variables globales
int state = STOP;

int currentVLeft = 1500;
int currentVRight = 1500;
int targetVLeft = 1500;
int targetVRight = 1500;

unsigned long lastDebugTime = 0;
unsigned long lastMoveTime = 0;

// Obtener tiempo actual (milisegundos)
unsigned long millis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

// Convertir microsegundos (1000–2000) → rango PWM (0–100)
int usToPwm(int us) {
    if (us < 1000) us = 1000;
    if (us > 2000) us = 2000;
    return (us - 1000) / 10;
}

// Mover motores
void moveMotors() {
    softPwmWrite(ESC_LEFT_PIN, usToPwm(currentVLeft));
    softPwmWrite(ESC_RIGHT_PIN, usToPwm(currentVRight));
}

void displayDebug() {
    if (!DEBUG) return;
    unsigned long now = millis();
    if (now - lastDebugTime < DEBUG_INTERVAL) return;
    lastDebugTime = now;

    std::cout << "[DEBUG] Left: " << currentVLeft
              << "  Right: " << currentVRight << std::endl;
}

void smoothOperator() {
    if (currentVLeft != targetVLeft || currentVRight != targetVRight) {
        unsigned long now = millis();
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

void checkData() {
    if (targetVLeft == 0) targetVLeft = 1500;
    else if (targetVLeft < 1000) targetVLeft = 1000;
    else if (targetVLeft > 2000) targetVLeft = 2000;

    if (targetVRight == 0) targetVRight = 1500;
    else if (targetVRight < 1000) targetVRight = 1000;
    else if (targetVRight > 2000) targetVRight = 2000;
}

bool readSerialCommand(std::ifstream &serial) {
    std::string line;
    if (std::getline(serial, line)) {
        size_t space = line.find(' ');
        if (space != std::string::npos) {
            targetVLeft = std::stoi(line.substr(0, space));
            targetVRight = std::stoi(line.substr(space + 1));
        } else {
            targetVLeft = 1500;
            targetVRight = 1500;
        }
        checkData();
        std::cout << "Received: " << targetVLeft << " " << targetVRight << std::endl;
        return true;
    }
    return false;
}

void escCalibrate() {
    std::cout << "Starting ESC calibration..." << std::endl;
    softPwmWrite(ESC_RIGHT_PIN, usToPwm(2000));
    softPwmWrite(ESC_LEFT_PIN, usToPwm(2000));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    softPwmWrite(ESC_RIGHT_PIN, usToPwm(1000));
    softPwmWrite(ESC_LEFT_PIN, usToPwm(1000));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Calibration done!" << std::endl;
}

int main() {
    wiringPiSetup();
    softPwmCreate(ESC_LEFT_PIN, 0, 100);
    softPwmCreate(ESC_RIGHT_PIN, 0, 100);

    escCalibrate();
    std::cout << "Initial value: 1500" << std::endl;

    std::ifstream serial(SERIAL_PORT);
    if (!serial.is_open()) {
        std::cerr << "Error opening serial port: " << SERIAL_PORT << std::endl;
        return 1;
    }

    while (true) {
        switch (state) {
            case MOVE:
                readSerialCommand(serial);
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
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return 0;
}
