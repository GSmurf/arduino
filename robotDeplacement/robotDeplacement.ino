/*
* IA de déplacement du robot
* Le moteur 1 est le moteur de gauche, le 2 le droite
*/
#include "Wire.h" // gestion du compass
#include "I2Cdev.h" // gestion du compass
#include "HMC5883L.h" // gestion du compass
#include <Servo.h> // gestion du servo moteur
#include <NewPing.h> // gestion du sonar

// pins disponible : 1,2,5,8,10,12 ; A2,A3,A4,A5

const int PIN_SONAR_TRIGGER = 13;
const int PIN_SONAR_ECHO = 11; // Lien PWR ~
const int PIN_SERVO_MOTEUR = 9;

const int PIN_MOTEUR1_PW  = 3; // Lien PWR ~
const int PIN_MOTEUR2_PW  = 6; // Lien PWR ~
const int PIN_MOTEUR1_DIR = 4;
const int PIN_MOTEUR2_DIR = 7;

const int PIN_COMPASS1 = A0; // Lien analogique a0 pour le compass
const int PIN_COMPASS2 = A1; // Lien analogique a1 pour le compass

// DEBUG MODE
const boolean DEBUG = true;
const int TEMPS_CYCLE = 10; // en millisecondes
const float TOLERANCE_ROTATION = 0.10; // pourcentage de la tolerance pour la rotation

/*
* Constantes du programme
*/
// Constantes de direction
const int DEVANT     = 0;
const int DROITE     = 90;
const int GAUCHE     = -90;
const int DEMI_TOUR  = 180;

// Constantes de distance le nombre exprime la distance en cm limite pour cette distance
const int TRES_PRES    = 8;
const int PRES         = 30;
const int LOIN         = 70;
const int RIEN         = 115;
const int DISTANCE_MAX = 400; // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

const int STOP   = 0;
const int LENT   = 100;
const int MOYEN  = 150;
const int RAPIDE = 255;

// variable to store the servo position
int pos = getAngleServo(DEVANT);     
// Distances percues par le robot
int distDroite = TRES_PRES;
int distGauche = TRES_PRES;
int distDevant = TRES_PRES;

// Déclaration du sonar
NewPing sonar(PIN_SONAR_TRIGGER, PIN_SONAR_ECHO, DISTANCE_MAX); // NewPing setup of pins and maximum distance.
Servo servo;  // create servo object to control a servo

// Déclaration pour le compass
HMC5883L mag;
int16_t mx, my, mz;
/* ------------------------------------------------------------------------
* Fonctions compass
------------------------------------------------------------------------ */
// Lit la direction du compass
float litDirection() {
  // read raw heading measurements from device
  mag.getHeading(&mx, &my, &mz);

  // To calculate heading in degrees. 0 degree indicates North
  float heading = atan2(mz, mx);
  if(heading < 0)
    heading += 2 * M_PI;
  
  return (heading * 180/M_PI);
}
/* ------------------------------------------------------------------------
* Mes fonctions
------------------------------------------------------------------------ */
// Retourne la distance en durée, sinon posible en centimetre
long litDistance(boolean retourEnCm = true){
  unsigned int duration = sonar.ping();
  if(DEBUG){
    Serial.print("distance : ");
    Serial.print((duration / US_ROUNDTRIP_CM));
    Serial.print(" cm (");
    Serial.print(duration);
    Serial.println(")");
  }
  if(retourEnCm){
    return (duration / US_ROUNDTRIP_CM);
  }else{
    return duration;
  }
}

// Retourne des constantes en fonction de la distance lue en cm
int detecteDistance() {
  // Lecture de la distance
  int x = litDistance();
  
  if(x < TRES_PRES) {
    return TRES_PRES;
  } else if(x < PRES) {
    return PRES;
  } else if(x < LOIN) {
    return LOIN;
  } else {
    return RIEN;
  }
}

int getAngleServo(int angle){
	return angle+90;
}

// Tourne le sonar dans une direction et renvoi la distance
int regardeVers(int angle, boolean retourneValeur=true){
  if(DEBUG)Serial.print("fonc regardeVers :");Serial.print(angle);Serial.print(", ");Serial.println(retourneValeur);
  
  angle = getAngleServo(angle); // converti l'angle pour le servo moteur
  
  // Si la position demandé est différente de la position précédente alors envoi des ordres de deplacement au servo
  if(pos != angle){
    pos = angle;
    // tourne vers l'angle donné
    digitalWrite(PIN_SERVO_MOTEUR,HIGH);
    servo.write(pos);
    digitalWrite(PIN_SERVO_MOTEUR,LOW);
    delay(450); // ce timing est le plus bref posible tout en lui laissant le temps !
  }
  if(retourneValeur == true){
    // retourne la distance dans cette direction
    return detecteDistance();
  }else{
    // retourne TRES_PRES;
    return TRES_PRES;
  }
}

// Retourne les distances dans les directions DROITE, GAUCHE, DEVANT
int scanDirection(){
  // prend la distance a droite
  distDroite = regardeVers(DROITE);
  // prend la distance a gauche
  distGauche = regardeVers(GAUCHE);
  // se remet droit
  regardeVers(DEVANT);

  if(DEBUG)debug("scanDirection");
}

/*
* Arrete le robot et le reinitiale en mode marche avant
*/
void stop(){
  moteurEnAvant();
  moteurVitesse(STOP,STOP);
}

/*
* Demande aux moteurs d'effectuer le travail pour faire tourner le robot comme désiré
* Fonctionnement du moteur http://goo.gl/CQJaXp
*/
void tourneRobot(int direction){  
  float positionDebut = litDirection();
  float positionActuelle = litDirection();
  int positionMax, positionMin;
  
  switch(direction) {
    case DROITE:
      moteurEnAvant();
      moteurVitesse(LENT,STOP);
      break;
    case GAUCHE:
      moteurEnAvant();
      moteurVitesse(STOP,LENT);
      break;
    case DEMI_TOUR:
      moteurEnAvant();
      moteurVitesse(MOYEN,STOP);
      break;
  }
  // Calcule la postition d'arrivée
  float positionArrive = positionDebut + direction;
  
  positionMin = int(positionArrive - (positionArrive * TOLERANCE_ROTATION)) % 360;
  positionMax = int(positionArrive + (positionArrive * TOLERANCE_ROTATION)) % 360;
  
  // Tourne tant que la direction attendu n'est pas atteinte 
  do{
    delay(50); // wait for sensors to stabilize
    positionActuelle = litDirection();
    if(DEBUG)debugRotation(direction, positionDebut, positionActuelle, positionMin, positionMax); 
  } while (!((positionActuelle > positionMin) && (positionActuelle < positionMax)));
  
  stop();
}
/* ------------------------------------------------------------------------
* Lancement du programme
------------------------------------------------------------------------ */
void setup(){
  // initialisation pour le compass
  Wire.begin(); 
  mag.initialize();
  mag.testConnection();
  
  pinMode(PIN_SERVO_MOTEUR, OUTPUT);

  pinMode(PIN_MOTEUR1_PW, OUTPUT);  //Set control pins to be outputs
  pinMode(PIN_MOTEUR2_PW, OUTPUT);
  pinMode(PIN_MOTEUR1_DIR, OUTPUT);
  pinMode(PIN_MOTEUR2_DIR, OUTPUT);
   
  servo.attach(PIN_SERVO_MOTEUR);  // attaches the servo on pin 9 to the servo object
  if(DEBUG)Serial.begin(9600);
  regardeVers(DEVANT);
  stop();
}

void loop(){ 
  distDevant = regardeVers(DEVANT); // scan la distance DEVANT
  
  // Si un obstacle est pres alors tourne
  if(distDevant == TRES_PRES){
    stop();
    // determine la meilleure direction et y tourne le robot
	ia_choixDirection();
  }else{
    // sinon determine son allure en fonction de la distance à un potentiel obstacle
	ia_avanceAllure();
  }
}
