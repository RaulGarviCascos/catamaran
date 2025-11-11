#include <wiringPi.h>
#include <iostream>
#include <unistd.h> // para sleep()


#define LED_PIN 17  // Usamos GPIO17 (pin físico 11)
#define BUTTON_PIN 27  // Usamos GPIO17 (pin físico 13)
		       
int main() {
    // Inicializar wiringPi con numeración GPIO (BCM)
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "Error al inicializar wiringPi" << std::endl;
        return 1;
    }
    bool enciendeLed = false;

    pinMode(LED_PIN, OUTPUT);  // Configurar el pin como salida
    pinMode(BUTTON_PIN, INPUT);  // Configurar el pin como entrada
	pullUpDnControl(BUTTON_PIN,PUD_UP);
	int value;
    while(true){
		value = digitalRead(BUTTON_PIN);
		if(value == LOW){
			enciendeLed = !enciendeLed;
			if(enciendeLed){
				std::cout << "Encendiendo LED..." << std::endl;
				digitalWrite(LED_PIN, HIGH);  // Encender
				delay(200);
			}else{
				std::cout << "Apago LED..." << std::endl;
				digitalWrite(LED_PIN, LOW);   // Apagar	
				delay(200);	
			}
		}
		delay(10);
    }
    std::cout << "Listo." << std::endl;
    return 0;
}
