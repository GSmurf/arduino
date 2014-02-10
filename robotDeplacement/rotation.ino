// site : http://goo.gl/IetFQU
// attach red wire to 5 volts
// attach black wire to  ground
// attach the yellow wire pin 8
const byte servopin = 8;

// to variables the will make the 
// servo turn to zero degree every
// restart/reset
int currentPosition = 500;  
int commandPosition = 500;

// larger rotateSpeed value means
// slower and smaller value means
// faster
byte rotateSpeed = 1;

// changing the increment value
// will also change the servo speed
// larger value increases speed
// lower value lowers speed
int increment = 2;

void setup(){ //void setup start
    Serial.begin(9600);
    pinMode(servopin,OUTPUT);
}// void setup end

void rotate(){//void rotate start
    /******************************************
    Use the variable currentPosition to 
    turn the servo motor
    
    The next four lines of codes are the basic 
    to run the servo motor. The delayMicroseconds
    can hold value from 500(0 degrees) to 
    2500(180 degrees)
    ******************************************/
    digitalWrite(servopin,HIGH);
    delayMicroseconds(currentPosition);
    digitalWrite(servopin,LOW);
    delayMicroseconds(20);
}//void rotate end

void loop(){// void loop start
    
    /******************************************
    Set of command to assign value to variable
    commandPosition. Open the serial monitor and
    letter a,b,c,d,e consecutively.
    
    The commandPosition variable can be assigned
    with any value from analog sensor such as 
    temperature and potentiometer. 
    ******************************************/
     if (Serial.available() > 0) { //if serial available start
        int data = Serial.read();
    
        if (data =='a'){
          commandPosition = 500; 
        }
        if (data =='b'){
          commandPosition = 1000; 
        }
        if (data =='c'){
          commandPosition = 1500; 
        }
        if (data =='d'){
          commandPosition = 2000; 
        }
        if (data =='e'){
          commandPosition = 2500; 
        }
    }// if serial.available end
    
    /******************************************
    Set of command to assign value from
    commandPosition to currentPosition
    ******************************************/
    if (commandPosition > currentPosition){
      currentPosition = currentPosition + increment;
      rotate();  
      }
    if (commandPosition < currentPosition){
      currentPosition = currentPosition - increment;
      rotate();
      }
    if (commandPosition == currentPosition){
      currentPosition = commandPosition; 
      digitalWrite(servopin,LOW); // stop sending pulses
      }          // as soon as the desired position is 
                // reached
    
    // Rotation speed
    delay(rotateSpeed);

}//void loop end
