
#include <Servo.h>

#define TURN_LEFT 1
#define TURN_RIGHT 2
#define FORWARD 3
#define BACKWARD 4

Servo escRight;
Servo escLeft;

byte state;



String newString;
int value;

int vOldLeft;
int vOldRight;

void move(int vLeft,int vRight){
  escLeft.writeMicroseconds(vLeft);
  escRight.writeMicroseconds(vRight);
}

int [] progressMove(int vOldLeft,int vOldRight,int vNewLeft,int vNewRight){   //iguala las velocidades, da igual en que sentido sea, eso ya se maneja desde las otras funciones
  
  
  if(vOldLeft!=vNewLeft || vOldRoght!=vNewRight){
    if(vOldLeft!=vNewLeft){
      vOldLeft+=(vOldLeft<vNewLeft)?1:-1;
    }
    if(vOldRight!=vNewRight){
      vOldRight+=(vOldRight<vNewRight)?1:-1;
    }
  }
  int newValues [2]={vOldLeft,vOldRight};
  return newValues;
  
}


void turnLeft(int vOld,int vNew){
  progressMove(vOld,vOld,1500,vNew);
}

void turnRight(int vOld){
  progressMove(vOld,vOld,vNew,1500);
}

String [] getValues(String inputString){
  String values [2];
  int pos=0;
  for(char caracter:inputString){
    if(caracter != " "){
      values[pos] += caracter;
    }else{
      pos+=1;
    }
  }
  return values;
}



void setup() {
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
  oldLeft = 1500;
  oldRight = 1500;
  Serial.println("initial value: 1500");
  
}

void loop() {
  
  if(Serial.available()){
    
    newString = Serial.readString();
    newString.trim();
    String values [2];
    values = getValues(newString);
    vNew = values[1];
    String message;
    
    if(vNew<1000){
      vNew = 1000;
    }else if(vNew>2000){
      vNew = 2000;
    }
    
    if(values[0] == "l" || values[0] == "left"|| values[0] == "izquierda"|| values[0] == "a"){
      message = "Im moving left";
      turnLeft(vOld,vNew);
    }else if(values[0] == "r" || values[0] == "right"|| values[0] == "derecha"|| values[0] == "d"){
       message = "Im moving right";
      turnRight(vOld,vNew);
    }else if(values[0] == "forward"|| values[0] == "adelante"|| values[0] == "f"|| values[0] == "w"){
      message = "Im moving forward";
      progressiveMove(vOld,vOld,vNew,vNew);
    }else if(values[0] == "backward"|| values[0] == "detras"|| values[0] == "b"|| values[0] == "s"){
      message = "Im moving backward";
      progressiveMove(vOld,vOld,nNew,vNew); 
    }

    Serial.println("Value has changed");
    Serial.println(message);
  }


  switch (state){
    case TURN_LEFT:
      break;
    case TURN_RIGHT:
      break;
    case FORWARD:
      break;
    case BACKWARD:
      break;
    case STOP:
      break;
      
  }
  
  esc.writeMicroseconds(value);
  delay(50);



}
