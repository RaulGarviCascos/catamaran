
#include <Servo.h>

#define TURN_LEFT 1
#define TURN_RIGHT 2
#define FORWARD 3
#define BACKWARD 4
#define DEFAULT_STATE 5
#define STOP 6

Servo escRight;
Servo escLeft;

int state;

String newString;
int vNew;

int vOldLeft;
int vOldRight;

void moveMotors(int vLeft,int vRight){
  //escLeft.writeMicroseconds(vLeft);
  //escRight.writeMicroseconds(vRight);
  Serial.print("muevo motor izquierdo:");
  Serial.println(vLeft);
  Serial.print("muevo motor derecho");
  Serial.println(vRight);
}

//iguala las velocidades, da igual en que sentido sea, eso ya se maneja desde las otras funciones
void progressMove(int vOldLeft,int vOldRight,int vNewLeft,int vNewRight,int currentState,int* newValues){   
  
  if(vOldLeft!=vNewLeft || vOldRight!=vNewRight){
    if(vOldLeft!=vNewLeft){
      vOldLeft+=(vOldLeft<vNewLeft)?1:-1;
    }
    if(vOldRight!=vNewRight){
      vOldRight+=(vOldRight<vNewRight)?1:-1;
    }
  }else if(vOldLeft==vNewLeft && vOldRight==vNewRight){
    currentState = DEFAULT_STATE;
  }
  newValues [3]={vOldLeft,vOldRight,currentState};
  
}


void getValues(String inputString, String* values){
  int pos=0;
  for(char caracter:inputString){
    if(caracter != ' '){
      values[pos] += caracter;
    }else{
      pos+=1;
    }
  }
 
}

int changeState(String* values){
  int state;
   if(values[0] == "l" || values[0] == "left"|| values[0] == "izquierda"|| values[0] == "a"){
      state = TURN_LEFT;
    }else if(values[0] == "r" || values[0] == "right"|| values[0] == "derecha"|| values[0] == "d"){
      state = TURN_RIGHT;
    }else if(values[0] == "forward"|| values[0] == "adelante"|| values[0] == "f"|| values[0] == "w"){
      state = FORWARD;
    }else if(values[0] == "backward"|| values[0] == "detras"|| values[0] == "b"|| values[0] == "s"){
      state = FORWARD;
    }else if(values[0] == "stop"|| values[0] == "parar"|| values[0] == "q"){
      state = STOP;
    }
  return state;
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
  vOldLeft = 1500;
  vOldRight = 1500;
  Serial.println("initial value: 1500");
  
}

void loop() {
  
  if(Serial.available()){
    newString = Serial.readString();
    newString.trim();
    String values [2];
    getValues(newString,values);
    vNew = values[1].toInt();
    Serial.print("values received: ");
    Serial.print(values[0]);
    Serial.print(", ");
    Serial.println(values[1]);
    if(vNew<1000){
      vNew = 1000;
    }else if(vNew>2000){
      vNew = 2000;
    }
    state = changeState(values);
    Serial.print("status has changed to: ");
    Serial.println(state);
  }


  switch (state){
    case TURN_LEFT:
      int currentValues [3] = {};
      progressMove(vOldLeft,vOldRight,1500,vNew,state,currentValues);
      vOldLeft = currentValues[0];
      vOldRight = currentValues[1];
      moveMotors(vOldLeft,vOldRight);
      state = currentValues[2];
      break;
    case TURN_RIGHT:
      int currentValues [3] {};
      progressMove(vOldLeft,vOldRight,vNew,1500,state,currentValues);
      vOldLeft = currentValues[0];
      vOldRight = currentValues[1];
      moveMotors(vOldLeft,vOldRight);
      state = currentValues[2];
      break;
    case FORWARD:
      int currentValues [3] {};
      progressMove(vOldLeft,vOldRight,vNew,1500,state,currentValues);
      vOldLeft = currentValues[0];
      vOldRight = currentValues[1];
      moveMotors(currentValues[0],currentValues[1]);
      state = currentValues[2];
      break;
    case BACKWARD:
      int currentValues [3] {};
      progressMove(vOldLeft,vOldRight,vNew,1500,state,currentValues);
      vOldLeft = currentValues[0];
      vOldRight = currentValues[1];
      moveMotors(vOldLeft,vOldRight);
      state = currentValues[2];
      break;
    case STOP:
      vOldLeft = 1500;
      vOldRight = 1500;
      moveMotors(vOldLeft,vOldRight);
      state = DEFAULT_STATE;
      break;
    case DEFAULT_STATE:
      moveMotors(vOldLeft,vOldRight);
      break;
      
  }
  


}
