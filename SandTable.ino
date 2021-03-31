int xLimit = 9;
int yLimit = 8;
int yEnable = 6;
int xEnable = 7;
int xStep = 3;
int xDir = 2;
int yStep = 5;
int yDir = 4;

int delayAmount = 1;
int yVal = 0;
int xVal = 0;

//TODO: Find x and y max steps
int yMax = 20850;
int xMax = 21300;

void setup() { 
  pinMode(yEnable, OUTPUT);
  pinMode(xEnable, OUTPUT);
  pinMode(xStep, OUTPUT);
  pinMode(xDir, OUTPUT);
  pinMode(yStep, OUTPUT);
  pinMode(yDir, OUTPUT);
  pinMode(yLimit, INPUT_PULLUP);
  pinMode(xLimit, INPUT_PULLUP);
  
  Serial.begin(9600);
  Serial.println("Starting Serial...");
  
  digitalWrite(yEnable, 0);
  digitalWrite(xEnable, 0);
  delayAmount = 1;
  Serial.println("Starting Calibration:");
  delayAmount = 1;
}

int calibrateY(){
  for(int i = 0; i<yMax; i++){
    if (digitalRead(yLimit) == 0){
      delay(3); //This delay and double check, filters out noise from stepper motors
      if (digitalRead(yLimit) == 0){
        Serial.println("Y Calibration Successful!");
      yVal = 0;
      stepY(yMax/2, 1);
      return 1;
      }
      
    }
    Serial.println(digitalRead(yLimit));
    stepY(1, 0);
  }
  Serial.println("Calibration Failed...");
  while(1){};
  return 0;
}

int calibrateX(){
  for(int i = 0; i<xMax; i++){
    if (digitalRead(xLimit) == 0){
      Serial.println("X Calibration Successful!");
      xVal = 0;
      stepX(xMax/2, 1);
      return 1;
    }
    stepX(1, 0);
  }
  Serial.println("Calibration Failed...");
  while(1){};
  return 0;
}

int stepY(int steps, int dir){
  digitalWrite(yDir, dir);
  for(int i = 0; i<steps; i++){
        digitalWrite(yStep, 1);
        delay(delayAmount);
        digitalWrite(yStep, 0);
        delay(delayAmount);
        if(dir){
          yVal++;
        } else {
          yVal--;
        }
      }
}

int stepX(int steps, int dir){
  digitalWrite(xDir, dir);
  for(int i = 0; i<steps; i++){
        digitalWrite(xStep, 1);
        delay(delayAmount);
        digitalWrite(xStep, 0);
        delay(delayAmount);
        if(dir){
          xVal++;
        } else {
          xVal--;
        }
      }
}

int goToCoords(int endxVal, int endyVal){
  Serial.println("Starting at coords: ("+(String)xVal + ", "+(String)yVal+")");
  Serial.println("Ending at coords:   ("+(String)endxVal + ", "+(String)endyVal+")");
  int xDelta = endxVal - xVal;
  int yDelta = endyVal - yVal;
  
  if (xDelta > 0){
    stepX(xDelta, 1);
  } else{
    stepX(xDelta, 0);
  }
  
  if (yDelta > 0){
    stepY(yDelta, 1);
  } else{
    stepY(yDelta, 0);
  }
  Serial.println("Done moving!!");
}

void loop() {
  calibrateX();
  calibrateY();
  //stepX(1800, 1);
  //stepY(1800, 0);
  //stepX(1800, 0);
  //stepY(1800, 1);
  //stepX(212500,1);
  
  while(1){}
}
