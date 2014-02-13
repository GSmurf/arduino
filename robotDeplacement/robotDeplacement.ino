/*
* IA de déplacement du robot
*/

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
const int PIN_LECTURE_DISTANCE = 0;
const int PIN_SERVO_MOTOR = 9;

// variable to store the servo position
int pos = 0;     
// Distances percues par le robot
int distDroite = 0;
int distGauche = 0;

/*
* Constantes du programme
*/
// Constantes de direction
const int DEVANT = 90;
const int DROITE = 180;
const int GAUCHE = 0;
const int DEMITOUR = 270;

// Constantes de distance
const int VERY_CLOSE 	= 0;
const int CLOSE		= 1;
const int FAR		= 2;
const int NOTHING	= 3;

/* ------------------------------------------------------------------------
* Mes fonctions
------------------------------------------------------------------------ */
int regardeVers(int angle, boolean retourneValeur=false){
	// tourne vers l'angle donné
	myservo.write(angle);
	delay(500); // TODO : Optimiser ce timing pour être le plus bref posible tout en lui laissant le temps
	if(retourneValeur == true){
		// retourne la distance dans cette direction
		return detecteDistance();
	}else{
		// retourne 0;
		return VERY_CLOSE;
	}
}

/*
* Si retourne valeur et true alors renvois la distance dans cette direction
*/
void choixDirection(){
	scanDirection();
	// si la distance à droite et à gauche est courte alors
	if((distDroite == VERY_CLOSE) && (distGauche == VERY_CLOSE)){
		// fait demi tour
		tourneRobot(DEMITOUR);
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

int scanDirection(){
	// prend la distance a droite
	distDroite = regardeVers(DROITE, true);
	// prend la distance a gauche
	distGauche = regardeVers(GAUCHE, true);
	// se remet droit
	int noNeed = regardeVers(DEVANT);
}

// prend la mesure
int detecteDistance() {
	int x = analogRead(PIN_LECTURE_DISTANCE);
	if(x > 400) {
		return VERY_CLOSE;
	} else if(x > 300) {
		return CLOSE;
	} else if(x > 200) {
		return FAR;
	} else {
		return NOTHING;
	}
}

// demande aux moteurs d'effectuer le travail pour faire tourner le robot comme désiré
void tourneRobot(int direction){
	// TODO
}

/* ------------------------------------------------------------------------
* Lancement du programme
------------------------------------------------------------------------ */
void setup(){ //void setup start
	myservo.attach(PIN_SERVO_MOTOR);  // attaches the servo on pin 9 to the servo object            // in steps of 1 degree 
	regardeVers(DEVANT);
    scanDirection();
}// void setup end

void loop(){
	// si obstacle 
		// stop
		choixDirection();
	// sinon avance
}
