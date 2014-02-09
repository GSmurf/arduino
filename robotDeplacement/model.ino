/* ============================================= 
* Motor Code 
* http://whiley.org/2014/01/31/more-tracked-arduino-fun/
* ============================================= */
const int pwmA = 3; 
const int pwmB = 11; 
const int dirA = 12; 
const int dirB = 13; 
const int brakeA = 9; 
const int brakeB = 8; 

const int STOP = 0;
const int SLOW = 100;
const int MEDIUM = 200;
const int FAST = 255;

void setMotors(int left, int right) {
 analogWrite(pwmA, left);   
 analogWrite(pwmB, right);   
}

void setDirections(int left, int right) {
  digitalWrite(dirA,left);
  digitalWrite(dirB,right);
}

void brakesOff() {
 digitalWrite(brakeA,LOW);
 digitalWrite(brakeB,LOW); 
}

/* ============================================= */
/* Sensor Code */
/* ============================================= */
const int FRONT_SENSOR_PIN = 0;
const int UNDERNEATH_SENSOR_PIN = 1;
const int BACK_SENSOR_PIN = 2;

const int VERY_CLOSE = 0;
const int CLOSE = 1;
const int FAR = 2;

int frontSensor() {
 int x = analogRead(FRONT_SENSOR_PIN);
 
 if(x > 400) {
  return VERY_CLOSE;
 } else if(x > 300) {
  return CLOSE;
 } else {
  return FAR;
 } 
}

int backSensor() {
 int x = analogRead(BACK_SENSOR_PIN);
 
 if(x > 600) {
  return VERY_CLOSE;
 } else if(x > 400) {
  return CLOSE;
 } else {
  return FAR;
 } 
}

int underneathSensor() {
 int x = analogRead(UNDERNEATH_SENSOR_PIN);
 
 if(x < 300) {
  return CLOSE;
 } else {
  return FAR;
 } 
}

/* ============================================= */
/* Setup Code */
/* ============================================= */

void setup() {   
 pinMode(pwmA, OUTPUT);  //Set control pins to be outputs
 pinMode(pwmB, OUTPUT);
 pinMode(dirA, OUTPUT);
 pinMode(dirB, OUTPUT); 
 pinMode(FRONT_SENSOR_PIN, INPUT);
 pinMode(UNDERNEATH_SENSOR_PIN, INPUT);

 analogWrite(pwmA, STOP);        
 analogWrite(pwmB, STOP);
 
 setDirections(HIGH,HIGH);
 brakesOff();

 delay(3000);
}

/* ============================================= */
/* Controller Code */
/* ============================================= */

void loop() {  
  int front = frontSensor();
  int back = backSensor();  
  int underneath = underneathSensor();
  
  if(underneath == CLOSE) {
    // Something underneath, continue.
    switch(front) {
      case VERY_CLOSE:
        if(back != VERY_CLOSE) {
          // Only continue going backwards if 
          // there's nothing behind us.
          setDirections(LOW,LOW);
          setMotors(FAST,FAST);      
        }
        break;
      case CLOSE:
        setDirections(HIGH,HIGH);
        setMotors(STOP,STOP);     
        break;
      case FAR:
        setDirections(HIGH,HIGH);
        setMotors(FAST,FAST);
        break;
      }
  } else {
      // Nothing underneath, all ahead stop. 
        setDirections(HIGH,HIGH);
        setMotors(STOP,STOP);           
  }
  
  delay(100);
}

