/*
* IA de déplacement du robot
*/
const int pinLectureDistance = 0;

// Constante du programme
const int DEVANT = 0;
const int DROITE = 90;
const int GAUCHE = -90;

const int VERY_CLOSE 	= 0;
const int CLOSE		= 1;
const int FAR		= 2;
const int NOTHING	= 3;



loop(){
	// si obstacle 
		// stop
		choixDirection();
	// sinon avance
}

choixDirection(){
	scanDirection();
	// si la distance à droite et à gauche est courte alors
		// fait demi tour
	// sinon
		// si la distance à droite et plus courte alors
			// tourne à droite
		// sinon tourne à gauche
			// tourne à gauche
}

scanDirection(){
	// prend la distance a droite
	int distanceDroite = regardeVers(DROITE, true);
	// prend la distance a gauche
	int distanceGauche= regardeVers(GAUCHE, true);
	// se remet droit
	int noNeed = regardeVers(DEVANT);
}

/*
* Si retourne valeur et true alors renvois la distance dans cette direction
*/
int regardeVers(angle, retourneValeur=false){
	// tourne vers l'angle donné
	// si retourneValeur == true 
		// retourne la distance dans cette direction
		return detecteDistance();
	// sinon
		// retourne 0;
}

// prend la mesure
int detecteDistance() {
	int x = analogRead(pinLectureDistance);
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
