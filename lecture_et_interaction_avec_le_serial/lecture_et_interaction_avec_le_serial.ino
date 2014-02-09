int lastByte;
char possibles[] = "a - allumage\n"
" z - extincion\n"
" e - affichage du satut\n"
" r - toggle de l'etat\n"
" t - mode blink\n";
int pin = 13;
volatile int state = LOW;
long previousMillis = 0;
boolean blinking = false;
long interval = 500;           // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(9600);
  pinMode(pin, OUTPUT);
//  attachInterrupt(0, blink, CHANGE);
}

void loop() {
  if (Serial.available() > 0)
  {
    lastByte = Serial.read();
    reception(lastByte);
  }
  if (blinking){
    blinkMode();
  }
}

void envoiStatut(int nouveauStatut)
{
  state = nouveauStatut;
  digitalWrite(pin, state);
}

void reception(int test){
  switch(test){
    case 'a':
      blinking = false;
      Serial.println("Vous avez envoye un a, allumage de la LED !");
      envoiStatut(HIGH);
      break;
    case 'z':
      blinking = false;
      Serial.println("Vous avez envoye un z, extinction de la LED !!");
      envoiStatut(LOW);
      break;
    case 'r':
      blinking = false;
      Serial.println("Vous avez envoye un r, toggle de l'etat !");
      changeStatut();
      break;     
    case 'e':
      Serial.print("state est egale a : ");
      Serial.println(state);
      break;       
    case 't':
      blinking = true;
      Serial.println("Mode blink ON !!!");
      break;       
    default:
      Serial.println("Vos posibilites sont : ");
      Serial.println(possibles);
      break;  
  }    
}

void changeStatut()
{
  envoiStatut(!state);
}

void blinkMode()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (state == LOW)
      state = HIGH;
    else
      state = LOW;

    // set the LED with the ledState of the variable:
    envoiStatut(state);
  }
}
