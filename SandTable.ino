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
  for (int i = 0; i < rMax; i++) {
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
    //Serial.println("rVal = "+rVal);
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

int goToPolar(int rValEnd, double thetaValEnd) {
  Serial.println("Starting at coords: (" + (String)rVal + ", " + (String)thetaVal + ")");
  Serial.println("Ending at coords:   (" + (String)rValEnd + ", " + (String)thetaValEnd + ")");
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
  /*//This ensures the theta motor chooses the shortest direction to the endpoint
    if(distanceToTheta>(thetaMax/2)){
    distanceToTheta = distanceToTheta-(thetaMax/2);
    thetaValDir ^= thetaValDir;
    }*/
  //This attempts to alternate the motors so they appear to move at the same time
  if (distanceToR > distanceToTheta) {
    int turnRatio = distanceToR / distanceToTheta;    
    for (int i = 0; i < (distanceToR / turnRatio); i++) {
      stepR(turnRatio, rValDir);
      stepTheta(1, thetaValDir);
    }
  } else {
    int turnRatio = distanceToTheta / distanceToR;
    for (int i = 0; i < (distanceToTheta / turnRatio); i++) {
      stepTheta(turnRatio, thetaValDir);
      stepR(1, rValDir);
    }
  }
  Serial.println("Actual ending coords:   (" + (String)rVal + ", " + (String)thetaVal + ") (Before adjusting)");
  //Below is to catch if we didn't make it all the way to both coords due to rounding
  distanceToR = abs(rVal - rValEnd);
  if (rValEnd > rVal) {
    rValDir = 1;
  } else {
    rValDir = 0;
  }
  distanceToTheta = abs(thetaVal - thetaValEnd);
  if (thetaValEnd > thetaVal) {
    thetaValDir = 1;
  } else {
    thetaValDir = 0;
  }
  stepR(distanceToR, rValDir);
  stepTheta(distanceToTheta, thetaValDir);
  Serial.println("Actual ending coords:   (" + (String)rVal + ", " + (String)thetaVal + ") (After adjusting)\n");
  return;
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


void loop() {
  calibrateR();

  goToPolar(0, 0);
  goToPolar(rMax / 2, thetaMax * 0.25);
  //goToPolar(0,0);
  goToPolar(rMax / 2, thetaMax * 0.50);
  //goToPolar(0,0);
  goToPolar(rMax / 2, thetaMax * 0.75);
  //goToPolar(0,0);
  goToPolar(rMax / 2, 0);
  //goToPolar(0,0);
  goToPolar(rMax / 2, 0);
  //spiral(0);
  //spiral(1);

  while (1) {}
}
