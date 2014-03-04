/*
* IA de déplacement du robot
*/
#include <Servo.h>
#include <NewPing.h> 

// pins disponible : 1,2,4,5 ; a0,a1,a2,a3,a4,a5

const int PIN_SONAR_TRIGGER = 7;
const int PIN_SONAR_ECHO = 6; // Lien PWR 
const int PIN_SERVO_MOTEUR = 9;

const int PIN_MOTEUR1_PW  = 3; // Lien PWR
const int PIN_MOTEUR2_PW  = 11; // Lien PWR
const int PIN_MOTEUR1_DIR = 12;
const int PIN_MOTEUR2_DIR = 13;
const int PIN_MOTEUR1_BRK = 10;
const int PIN_MOTEUR2_BRK = 8;

// DEBUG MODE
const boolean DEBUG = true;
const int TEMPS_CYCLE = 2; // EN SECONDES
const int TEMPS_ROTATION = 2000; // TODO regler le timing de rotation du robot en milliseconde pour pouvoir être précis

/*
* Constantes du programme
*/
// Constantes de direction
const int DEVANT     = 90;
const int DROITE     = 180;
const int GAUCHE     = 0;
const int DEMI_TOUR  = 270;

// Constantes de distance
const int TRES_PRES    = 0;
const int PRES         = 1;
const int LOIN         = 2;
const int RIEN         = 3;
const int DISTANCE_MAX = 400; // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

// variable to store the servo position
int pos = DEVANT;     
// Distances percues par le robot
int distDroite = TRES_PRES;
int distGauche = TRES_PRES;
int distDevant = TRES_PRES;

// Déclaration du sonar
NewPing sonar(PIN_SONAR_TRIGGER, PIN_SONAR_ECHO, DISTANCE_MAX); // NewPing setup of pins and maximum distance.
Servo servo;  // create servo object to control a servo

/* ------------------------------------------------------------------------
* Code moteur
------------------------------------------------------------------------ */
const int STOP   = 0;
const int LENT   = 50;
const int MOYEN  = 75;
const int RAPIDE = 100;

void moteurVitesse(int vitGauche, int vitDroite) {
  analogWrite(PIN_MOTEUR1_PW, vitGauche);
  analogWrite(PIN_MOTEUR2_PW, vitDroite);
}

void moteurEnArriere() {
  digitalWrite(PIN_MOTEUR1_DIR,HIGH);
  digitalWrite(PIN_MOTEUR2_DIR,HIGH);
}

void moteurEnAvant() {
  digitalWrite(PIN_MOTEUR1_DIR,LOW);
  digitalWrite(PIN_MOTEUR2_DIR,LOW);
}

void moteurFreins(boolean active = true) {
  if(active){
    digitalWrite(PIN_MOTEUR1_BRK,HIGH);
    digitalWrite(PIN_MOTEUR2_BRK,HIGH);
  }else{
    digitalWrite(PIN_MOTEUR1_BRK,LOW);
    digitalWrite(PIN_MOTEUR2_BRK,LOW);
  }
}

/* ------------------------------------------------------------------------
* Mes fonctions
------------------------------------------------------------------------ */
/*
* Retourne la distance en durée, sinon posible en centimetre
*/
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

// prend la mesure
int detecteDistance() {
  // Lecture de la distance
  int x = litDistance();
  
  if(x < 4) {
    return TRES_PRES;
  } else if(x < 30) {
    return PRES;
  } else if(x < 45) {
    return LOIN;
  } else {
    return RIEN;
  }
}

int regardeVers(int angle, boolean retourneValeur=true){
  if(DEBUG){
    Serial.print("fonc regardeVers :");
    Serial.print(angle);
    Serial.print(", ");
    Serial.println(retourneValeur);
  }
  
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
* Le robot avance à une certaine allure en fonction de la distance qu'il avait detecté devant lui dans la boucle loop
*/
void avance(){
  switch(distDevant) {
    case TRES_PRES:
      moteurEnArriere();
      moteurVitesse(RAPIDE,RAPIDE);
      break;
    case PRES:
      moteurEnAvant();
      moteurVitesse(STOP,STOP);
      break;
    case LOIN:
      moteurEnAvant();
      moteurVitesse(MOYEN,MOYEN);
      break;
    case RIEN:
      moteurEnAvant();
      moteurVitesse(RAPIDE,RAPIDE);
      break;
  }
}

/*
* Arrete le robot
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
  switch(direction) {
    case DROITE:
      moteurEnArriere();
      moteurVitesse(LENT,STOP);
      delay(TEMPS_ROTATION);
      moteurVitesse(STOP,STOP);
      break;
    case GAUCHE:
      moteurEnAvant();
      moteurVitesse(STOP,LENT);
      delay(TEMPS_ROTATION);
      moteurVitesse(STOP,STOP);
      break;
    case DEMI_TOUR:
      moteurEnAvant();
      moteurVitesse(STOP,DROITE);
      delay(TEMPS_ROTATION);
      moteurVitesse(STOP,STOP);
      break;
  }
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
  pinMode(PIN_SERVO_MOTEUR, OUTPUT);

  pinMode(PIN_MOTEUR1_PW, OUTPUT);  //Set control pins to be outputs
  pinMode(PIN_MOTEUR2_PW, OUTPUT);
  pinMode(PIN_MOTEUR1_DIR, OUTPUT);
  pinMode(PIN_MOTEUR2_DIR, OUTPUT);
   
  servo.attach(PIN_SERVO_MOTEUR);  // attaches the servo on pin 9 to the servo object
  if(DEBUG)Serial.begin(9600);
  regardeVers(DEVANT);
  moteurFreins(false);
  stop();
}

void loop(){ 
  distDevant = regardeVers(DEVANT);
  
  // si obstacle devant pres
  if(distDevant == TRES_PRES){
    stop();
    // determine la meilleure direction et y tourne le robot
	choixDirection();
  }else{
	avance();
  } 
      
  if(DEBUG)delay(TEMPS_CYCLE*1000); // donne un délai d'exécution pour les cycles
}
