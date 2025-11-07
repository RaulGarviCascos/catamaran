
#include <Servo.h>

#define MOVE 1
#define PROGRESSIVE_MOVE 2
#define STOP 3


Servo escRight;
Servo escLeft;

int state = STOP;

String command = "";       //String with the velocity of left motor and right motor

String valuesComand[2];  //["vLeft", "vRight"] 


int currentVLeft = 1500;
int currentVRight = 1500;
int targetVLeft = 1500;
int targetVRight = 1500;

void moveMotors(){
  //escLeft.writeMicroseconds(currentVLeft);
  //escRight.writeMicroseconds(currentVRight);

  
  /*Serial.print("muevo motor izquierdo:");
  Serial.println(currentVLeft);
  Serial.print("muevo motor derecho");
  Serial.println(currentVRight);*/
}

//iguala las velocidades, da igual en que sentido sea, eso ya se maneja desde las otras funciones
void progressMove(){   

  if(currentVLeft!=targetVLeft || currentVRight!=targetVRight){
    if(currentVLeft!=targetVLeft){
      currentVLeft+=(currentVLeft<targetVLeft)?1:-1;
    }
    if(currentVRight!=targetVRight){
      currentVRight+=(currentVRight<targetVRight)?1:-1;
    }
  }else if(currentVLeft==targetVLeft && currentVRight==targetVRight){
    state = MOVE;
    Serial.print("He alcanzado el objetivo de las velocidades: ");
    Serial.print(currentVLeft);
    Serial.print(" : ");
    Serial.println(currentVRight);
  }
  delay(20);
  
}

void getValues(){
  int pos=0;
  for(char caracter:command){
    if(caracter != ' '){
      valuesComand[pos] += caracter;
    }else{
      pos+=1;
    }
  }
}

void checkData(){
 
  if(targetVLeft == 0){
    targetVLeft = 1500;
  }else if(targetVLeft<1000){
    targetVLeft = 1000;
  }else if(targetVLeft>2000){
    targetVLeft = 2000;
  }

  if(targetVRight==0){
    targetVRight = 1500;
  }else if(targetVRight<1000){
    targetVRight = 1000;
  }else if(targetVRight>2000){
    targetVRight = 2000;
  }
  state = PROGRESSIVE_MOVE;
}

void receiveCommand(){
  command = Serial.readString();
  command.trim();
  getValues();
  targetVLeft = valuesComand[0].toInt();
  targetVRight = valuesComand[1].toInt();
  Serial.print("values received: ");
  Serial.print(valuesComand[0]);
  Serial.print(", ");
  Serial.println(valuesComand[1]);
  checkData();
  valuesComand[0] = "";
  valuesComand[1] = "";
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
  
  if(Serial.available()){
   receiveCommand();
  }

  switch (state){
    case MOVE:
      moveMotors();
      break;
    case PROGRESSIVE_MOVE:
      progressMove();
      moveMotors();
      break;
    case STOP:
      targetVLeft = 1500;
      targetVRight = 1500;
      state = PROGRESSIVE_MOVE;
      break;
   
      
  }
  


}
