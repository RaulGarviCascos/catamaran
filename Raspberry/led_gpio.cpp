#include <wiringPi.h>
#include <iostream>
#include <unistd.h> // para sleep()

#define LED_PIN 17  // Usamos GPIO17 (pin físico 11)

int main() {
    // Inicializar wiringPi con numeración GPIO (BCM)
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "Error al inicializar wiringPi" << std::endl;
        return 1;
    }

    pinMode(LED_PIN, OUTPUT);  // Configurar el pin como salida

    std::cout << "Encendiendo LED..." << std::endl;
    digitalWrite(LED_PIN, HIGH);  // Encender
    sleep(1);

    std::cout << "Apagando LED..." << std::endl;
    digitalWrite(LED_PIN, LOW);   // Apagar
    sleep(1);

    std::cout << "Listo." << std::endl;
    return 0;
}
