int thirdSensor = 0;    // digital sensor

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  
  pinMode(2, INPUT);   // digital sensor is on digital pin 2
}

void loop()
{
  thirdSensor = map(digitalRead(2), 0, 1, 0, 255);
  // Quand un son est entendu alors l'écrit ;)
  if (thirdSensor == 0) {
    Serial.print("Son detecte (");Serial.print(thirdSensor);Serial.print(") :\t");Serial.println(millis());
    delay(80);
  }
}
