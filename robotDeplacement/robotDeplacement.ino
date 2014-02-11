/*
* IA de déplacement du robot
*/
const int pinLectureDistance = 0;
/*
* Constantes du programme
*/
// Constantes de direction
const int DEVANT = 0;
const int DROITE = 90;
const int GAUCHE = -90;
const int DEMITOUR = 180;


// Distances percues par le robot
int DIST_DROITE = 0;
int DIST_GAUCHE = 0;

// Constantes de distance
const int VERY_CLOSE 	= 0;
const int CLOSE		= 1;
const int FAR		= 2;
const int NOTHING	= 3;


void setup(){ //void setup start
    scanDirection();
}// void setup end

void loop(){
	// si obstacle 
		// stop
		choixDirection();
	// sinon avance
}

void choixDirection(){
	scanDirection();
	// si la distance à droite et à gauche est courte alors
	if((DIST_DROITE == VERY_CLOSE) && (DIST_GAUCHE == VERY_CLOSE)){
		// fait demi tour
		tourneRobot(DEMITOUR);
	}else{
		// si la distance à droite et plus courte alors
		if(DIST_DROITE < DIST_GAUCHE){
			// tourne à droite
			tourneRobot(DROITE);
		}else{
			// tourne à gauche
			tourneRobot(GAUCHE);
		}
	}
}

/*
* Si retourne valeur et true alors renvois la distance dans cette direction
*/
int regardeVers(int angle, boolean retourneValeur=false){
	// tourne vers l'angle donné
	// TODO demander au servo moteur de tourner selon l'angle
	if(retourneValeur == true){
		// retourne la distance dans cette direction
		return detecteDistance();
	}else{
		// retourne 0;
		return VERY_CLOSE;
	}
}

int scanDirection(){
	// prend la distance a droite
	DIST_DROITE = regardeVers(DROITE, true);
	// prend la distance a gauche
	DIST_GAUCHE = regardeVers(GAUCHE, true);
	// se remet droit
	int noNeed = regardeVers(DEVANT);
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

// demande aux moteurs d'effectuer le travail pour faire tourner le robot comme désiré
void tourneRobot(int direction){
	// TODO
}
