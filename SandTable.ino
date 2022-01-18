int rLimit = 9;
int rEnable = 6;
int thetaEnable = 7;
int thetaStep = 3;
int thetaDir = 2;
int rStep = 5;
int rDir = 4;

int delayAmount;
int rVal = 0;
int thetaVal = 0;

int rMax = 11000;
int thetaMax = 13050;

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

int calibrateR() {
  for (int i = 0; i < rMax * 2; i++) {
    if (digitalRead(rLimit) == 0) {
      delay(3); //This delay and double check, filters out noise from stepper motors
      if (digitalRead(rLimit) == 0) {
        Serial.println("R Calibration Successful!");
        stepR(rMax, 0);
        rVal = 0;
        return 1;
      }

    }
    stepR(1, 1);
    //Serial.println(digitalRead(rLimit));
  }
  Serial.println("Calibration Failed...");
  while (1) {};
  return 0;
}

int stepR(int steps, int dir) {
  digitalWrite(rDir, dir);
  for (int i = 0; i < steps; i++) {
    if (rVal >= rMax || abs(rVal) >= rMax) {
      Serial.println("Out of bounds");
      return;
    }
    digitalWrite(rStep, 1);
    delay(delayAmount);
    digitalWrite(rStep, 0);
    delay(delayAmount);
    if (dir) {
      rVal++;
    } else {
      rVal--;
    }
  }
}

int stepTheta(int steps, int dir) {
  digitalWrite(thetaDir, dir);
  for (int i = 0; i < steps; i++) {
    digitalWrite(thetaStep, 1);
    delay(delayAmount);
    digitalWrite(thetaStep, 0);
    delay(delayAmount);
    if (dir) {
      thetaVal++;
    } else {
      thetaVal--;
    }
  }
}

//Moves to coords turning R first, then Theta
int goToPolarUnsync(int rValEnd, double thetaValEnd){
  //Serial.println("Starting at coords: (" + (String)rVal + ", " + (String)thetaVal + ")");
  //Serial.println("Ending at coords:   (" + (String)rValEnd + ", " + (String)thetaValEnd + ")");
  int rValDir;
  int distanceToR = abs(rVal - rValEnd);
  if (rValEnd > rVal) {
    rValDir = 1;
  } else {
    rValDir = 0;
  }
  int thetaValDir;
  int distanceToTheta = abs(thetaVal - thetaValEnd);
  if (thetaValEnd > thetaVal) {
    thetaValDir = 1;
  } else {
    thetaValDir = 0;
  }
  //This ensures the theta motor chooses the shortest direction to the endpoint
  if (distanceToTheta > (thetaMax / 2)) {
    distanceToTheta = distanceToTheta - (thetaMax / 2);
    if(thetaValDir == 0){
      thetaValDir = 1;
    } else{
      thetaValDir = 0; 
    }
  }
  stepR(distanceToR, rValDir);
  stepTheta(distanceToTheta, thetaValDir);
}

int goToPolar(int rValEnd, double thetaValEnd) {
  int distanceToR = 0;
  int distanceToTheta = 0;
  int rValDir;
  int thetaValDir;
  //Serial.println("Starting at coords: (" + (String)rVal + ", " + (String)thetaVal + ")");
  //Serial.println("Ending at coords:   (" + (String)rValEnd + ", " + (String)thetaValEnd + ")");

  distanceToR = abs(rVal - rValEnd);
  if (rValEnd > rVal) {
    rValDir = 1;
  } else {
    rValDir = 0;
  }
  distanceToTheta = abs(thetaVal - thetaValEnd);
  if (thetaValEnd > thetaVal) {
    thetaValDir = 0;
  } else {
    thetaValDir = 1;
  }
  //This ensures the theta motor chooses the shortest direction to the endpoint
  if (distanceToTheta > (thetaMax / 2)) {
    distanceToTheta = distanceToTheta - (thetaMax / 2);
    if(thetaValDir == 0){
      thetaValDir = 1;
    } else{
      thetaValDir = 0; 
    }
  }
  //This attempts to alternate the motors so they appear to move at the same time
  int remainder = 0;
  if (distanceToR > distanceToTheta) {
    int turnRatio = distanceToR / distanceToTheta;
    for (int i = 0; i < distanceToTheta; i++) {
      stepR(turnRatio, rValDir);
      if (distanceToTheta > 0) {
        stepTheta(1, thetaValDir);
      }
    }
    remainder = distanceToR % distanceToTheta;
    //Serial.println("Remainder: "+(String)remainder);
    stepR(remainder, rValDir);
  } else {
    int turnRatio = distanceToTheta / distanceToR;
    for (int i = 0; i < distanceToR; i++) {
      stepTheta(turnRatio, thetaValDir);
      if (distanceToR > 0) {
        stepR(1, rValDir);
      }
    }
    remainder = distanceToTheta % distanceToR;
    //Serial.println("Remainder: "+(String)remainder);
    stepTheta(remainder, thetaValDir);
  }
  if(thetaVal < 0){
    thetaVal += thetaMax;
  }
  if(thetaVal > thetaMax){
    thetaVal -= thetaMax;
  }
  //Serial.println("Distance R:     "+(String)distanceToR+" in Dir: "+(String)rValDir);
  //Serial.println("Distance Theta: "+(String)distanceToTheta+" in Dir: "+(String)thetaValDir);
  //Serial.println("Actual ending coords:   (" + (String)rVal + ", " + (String)thetaVal + ")\n");
  //Call the goTo function again to correct for any rounding problems
  goToPolarUnsync(rValEnd, thetaValEnd);

}


int spiral(int cw) {
  while (rVal < (rMax - 50)) {
    stepTheta(10, cw);
    stepR(1, 1);
  }
  while (rVal > 0) {
    stepTheta(10, cw);
    stepR(1, 0);
  }
}

int goToCartesian(double xVal, double yVal){
  Serial.println("Cartesian coords: (" + (String)xVal + ", " + (String)yVal + ")");
  int thetaValEnd =(atan(yVal/xVal))*((thetaMax/2)/3.141592653);
  int rValEnd = sqrt(sq(xVal)+sq(yVal));
  if(thetaValEnd<0){
    thetaValEnd += thetaMax;
  }
  Serial.println("Polar Coords: (" + (String)rValEnd + ", " + (String)thetaValEnd + ")\n");
  goToPolar(rValEnd, thetaValEnd);  
}

void loop() {
  calibrateR();
  goToPolarUnsync(-rMax,0);
  delay(2000000000);
  goToPolarUnsync(rMax/2, thetaMax*0.25);
  delay(2000);
  goToPolarUnsync(0,0);
  delay(2000);
  goToPolarUnsync(rMax/2, thetaMax*0.50);
  delay(2000);
  goToPolarUnsync(0,0);
  delay(2000);
  goToPolarUnsync(rMax/2, thetaMax*0.75);
  delay(200000);


  goToPolar(rMax / 2, thetaMax * 0.75);
  delay(2000);
  goToPolar(rMax / 2, 0);
  delay(2000);
  goToPolar(rMax / 2, thetaMax * 0.75);
  delay(2000);
  //goToPolar(0,0);
  goToPolar(rMax / 2, 0);
  delay(2000);
  goToPolar(rMax / 2, thetaMax * 0.75);
  delay(2000);
  //goToPolar(0,0);
  goToPolar(rMax / 2, 0);
  delay(2000);
  //spiral(0);
  //spiral(1);

  while (1) {}
}
