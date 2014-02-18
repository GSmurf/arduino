/*
* IA de déplacement du robot
*/
#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
const int PIN_LECTURE_DISTANCE = 0;
const int PIN_SERVO_MOTEUR = 9;

// DEBUG MODE
const boolean DEBUG = true;
const int TEMPS_CYCLE = 4; // EN SECONDES

/*
* Constantes du programme
*/
// Constantes de direction
const int DEVANT     = 90;
const int DROITE     = 180;
const int GAUCHE     = 0;
const int DEMI_TOUR  = 270;

// Constantes de distance
const int TRES_PRES 	= 0;
const int PRES		= 1;
const int LOIN		= 2;
const int RIEN	        = 3;

// variable to store the servo position
int pos = DEVANT;     
// Distances percues par le robot
int distDroite = TRES_PRES;
int distGauche = TRES_PRES;
int distDevant = TRES_PRES;
/* ------------------------------------------------------------------------
* Mes fonctions
------------------------------------------------------------------------ */
/*
* The speed of sound is 340 m/s or 29 microseconds per centimeter.
* The ping travels out and back, so to find the distance of the
* object we take half of the distance travelled.
*/
long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

/*
* Retourne la distance en durée, sinon posible en centimetre
* cf : http://goo.gl/0Co9nL
*/
long litDistance(boolean retourEnCm = false){
  long duration;
  pinMode(PIN_LECTURE_DISTANCE, OUTPUT);
  digitalWrite(PIN_LECTURE_DISTANCE, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_LECTURE_DISTANCE, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_LECTURE_DISTANCE, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(PIN_LECTURE_DISTANCE, INPUT);
  duration = pulseIn(PIN_LECTURE_DISTANCE, HIGH);
  
  if(retourEnCm){
    return microsecondsToCentimeters(duration);
  }else{
    return duration;
  }
}

// prend la mesure
int detecteDistance() {
  // Lecture de la distance
  // int x = analogRead(PIN_LECTURE_DISTANCE); 
  int x = litDistance();
  Serial.print("PIN_LECTURE_DISTANCE : ");
  Serial.println(x);
  
  if(x > 400) {
    return TRES_PRES;
  } else if(x > 300) {
    return PRES;
  } else if(x > 200) {
    return LOIN;
  } else {
    return RIEN;
  }
}

int regardeVers(int angle, boolean retourneValeur=false){
  pos = angle;
  // tourne vers l'angle donné
  digitalWrite(PIN_SERVO_MOTEUR,HIGH);
  myservo.write(pos);
  digitalWrite(PIN_SERVO_MOTEUR,LOW);
  delay(450); // ce timing est le plus bref posible tout en lui laissant le temps !
  if(retourneValeur == true){
    // retourne la distance dans cette direction
    return detecteDistance();
  }else{
    // retourne TRES_PRES;
    return TRES_PRES;
  }
}

int scanDirection(){
  // prend la distance a droite
  distDroite = regardeVers(DROITE, true);
  // prend la distance a gauche
  distGauche = regardeVers(GAUCHE, true);
  // se remet droit
  distDevant = regardeVers(DEVANT);

  if(DEBUG)debug("scanDirection");
}

/*
* Demande aux moteurs d'effectuer le travail pour faire tourner le robot comme désiré
* Fonctionnement du moteur http://goo.gl/CQJaXp
*/
void tourneRobot(int direction){
  // TODO
}

void debug(String texte){
  String titre = "*** "+texte+" ***";
  Serial.println(titre);
  
  Serial.print("Droite : ");
  Serial.print(distDroite);
  Serial.print(", Gauche : ");
  Serial.print(distGauche);
  Serial.print(", Devant : ");
  Serial.println(distDevant);
  
  Serial.print("Position : ");
  Serial.println(pos);
  
  Serial.println("---");
}

/*
* Si retourne valeur et true alors renvois la distance dans cette direction
*/
void choixDirection(){
  scanDirection();
  // si la distance à droite et à gauche est courte alors
  if((distDroite == TRES_PRES) && (distGauche == TRES_PRES)){
    // fait demi tour
    tourneRobot(DEMI_TOUR);
  }else{
    // si la distance à droite et plus courte alors
    if(distDroite < distGauche){
      // tourne à droite
      tourneRobot(DROITE);
    }else{
      // tourne à gauche
      tourneRobot(GAUCHE);
    }
  }
}
/* ------------------------------------------------------------------------
* Lancement du programme
------------------------------------------------------------------------ */
void setup(){
  pinMode(PIN_LECTURE_DISTANCE, INPUT);
  pinMode(PIN_SERVO_MOTEUR, OUTPUT);
  myservo.attach(PIN_SERVO_MOTEUR);  // attaches the servo on pin 9 to the servo object
  if(DEBUG)Serial.begin(9600);
  regardeVers(DEVANT);
}

void loop(){ 
  // si obstacle
  if(1 == 1){
    // stop
    choixDirection();
  }else{
    // sinon avance
  } 
      
  delay(TEMPS_CYCLE*1000); // donne un délai d'exécution pour les cycles
}
