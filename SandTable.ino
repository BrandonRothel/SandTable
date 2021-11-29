int rLimit = 9;
int rEnable = 6;
int thetaEnable = 7;
int thetaStep = 3;
int thetaDir = 2;
int rStep = 5;
int rDir = 4;

int delayAmount = 1;
int rVal = 0;
int thetaVal = 0;

int rMax = 22000;

void setup() { 
  pinMode(rEnable, OUTPUT);
  pinMode(thetaEnable, OUTPUT);
  pinMode(thetaStep, OUTPUT);
  pinMode(thetaDir, OUTPUT);
  pinMode(rStep, OUTPUT);
  pinMode(rDir, OUTPUT);
  pinMode(rLimit, INPUT_PULLUP);
  
  Serial.begin(9600);
  Serial.println("Starting Serial...");
  
  digitalWrite(rEnable, 0);
  digitalWrite(thetaEnable, 0);
  delayAmount = 1;
  Serial.println("Starting Calibration:");
}

int calibrateR(){
  for(int i = 0; i<rMax; i++){
    if (digitalRead(rLimit) == 0){
      delay(3); //This delay and double check, filters out noise from stepper motors
      if (digitalRead(rLimit) == 0){
        Serial.println("R Calibration Successful!");
      rVal = 0;
      stepR(rMax/2, 0);
      return 1;
      }
      
    }
    stepR(1, 1);
    Serial.println(digitalRead(rLimit));
  }
  Serial.println("Calibration Failed...");
  while(1){};
  return 0;
}

int stepR(int steps, int dir){
  digitalWrite(rDir, dir);
  for(int i = 0; i<steps; i++){
        digitalWrite(rStep, 1);
        delay(delayAmount);
        digitalWrite(rStep, 0);
        delay(delayAmount);
        if(dir){
          rVal++;
        } else {
          rVal--;
        }
      }
}

int stepTheta(int steps, int dir){
  digitalWrite(thetaDir, dir);
  for(int i = 0; i<steps; i++){
        digitalWrite(thetaStep, 1);
        delay(delayAmount);
        digitalWrite(thetaStep, 0);
        delay(delayAmount);
        if(dir){
          thetaVal++;
        } else {
          thetaVal--;
        }
      }
}


int spiral(int cw){
  while(1){
    
  }
}

void loop() {
  calibrateR();
  //spiral(1);
  
  while(1){}
}
