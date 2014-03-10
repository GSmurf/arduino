/*
* IA de déplacement du robot
* Le moteur 1 est le moteur de gauche, le 2 le droite
*/
#include <Servo.h>
#include <NewPing.h> 

// pins disponible : 1,2,5,8,10,12 ; a0,a1,a2,a3,a4,a5

const int PIN_SONAR_TRIGGER = 13;
const int PIN_SONAR_ECHO = 11; // Lien PWR ~
const int PIN_SERVO_MOTEUR = 9;

const int PIN_MOTEUR1_PW  = 3; // Lien PWR ~
const int PIN_MOTEUR2_PW  = 6; // Lien PWR ~
const int PIN_MOTEUR1_DIR = 4;
const int PIN_MOTEUR2_DIR = 7;

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
const int LENT   = 35;
const int MOYEN  = 50;
const int RAPIDE = 100;

void moteurVitesse(int vitGauche, int vitDroite) {
  analogWrite(PIN_MOTEUR1_PW, vitGauche);
  analogWrite(PIN_MOTEUR2_PW, vitDroite);
}

void moteurEnAvant() {
  digitalWrite(PIN_MOTEUR1_DIR,HIGH);
  digitalWrite(PIN_MOTEUR2_DIR,HIGH);
}

void moteurEnArriere() {
  digitalWrite(PIN_MOTEUR1_DIR,LOW);
  digitalWrite(PIN_MOTEUR2_DIR,LOW);
}

// Permet de tourner vite à droite
void moteurTourneADroite() {
  digitalWrite(PIN_MOTEUR1_DIR,HIGH);
  digitalWrite(PIN_MOTEUR2_DIR,LOW);
}

// Permet de tourner vite à gauche
void moteurTourneAGauche() {
  digitalWrite(PIN_MOTEUR1_DIR,LOW);
  digitalWrite(PIN_MOTEUR2_DIR,HIGH);
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

// Tourne le sonar dans une direction et renvoi la distance
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
  switch(direction) {
    case DROITE:
      moteurTourneADroite();
      moteurVitesse(LENT,LENT);
      break;
    case GAUCHE:
      moteurTourneAGauche();
      moteurVitesse(LENT,LENT);
      break;
    case DEMI_TOUR:
      moteurTourneADroite();
      moteurVitesse(MOYEN,MOYEN);
      break;
  }
  delay(TEMPS_ROTATION);
  stop();
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
* Le robot avance à une certaine allure en fonction de la distance 
* qu'il avait detecté devant lui dans la boucle loop
* TODO : faire en sorte qu'il renvoi un boolean pour savoir s'il avance ou non
*/
void ia_avanceAllure(){
  switch(distDevant) {
    case TRES_PRES:
      stop();
      break;
    case PRES:
      moteurEnAvant();
      moteurVitesse(LENT,LENT);
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
* Scan la direction la plus courte pour y tourner le robot
*/
void ia_choixDirection(){
  scanDirection();
  // si la distance à droite et à gauche est courte alors
  if((distDroite == TRES_PRES) && (distGauche == TRES_PRES)){
    // fait demi tour
    tourneRobot(DEMI_TOUR);
  }else{
    // si la distance à droite et la plus courte ou egale à gauche alors
    if(distDroite <= distGauche){
      // tourne à droite
      tourneRobot(DROITE);
    }else{
      // tourne à gauche
      tourneRobot(GAUCHE);
    }
  }
}

// Viens tater l'obstacle devant lui
void ia_essayeDePasser(){}

// Suis l'obstacle devant lui
void ia_suisUneCible(){}

// Va se ranger dans le coin cardinal demandé
void ia_vaSeRangerDansLeCoin(){}


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
      
  // if(DEBUG)delay(TEMPS_CYCLE*1000); // donne un délai d'exécution pour les cycles
}
