/* ------------------------------------------------------------------------
* Fonctions IA
------------------------------------------------------------------------ */
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
	// recule pendant 1 sec
	moteurEnArriere();
    moteurVitesse(LENT,LENT);
	delay(1000);	
    // fait demi tour
    tourneRobot(DEMI_TOUR);
	scanDirection();
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

// Detecte ou es la cible
int ia_chercheAngleCible(int ancienneDistance){
	int i;
	float tolerance = 0.1;
	int distMax = int(ancienneDistance + (ancienneDistance*tolerance));
	int distMin = int(ancienneDistance - (ancienneDistance*tolerance));
	int angle = 90;
	int distance;
	int angles[] = {0,36,72,108,144,180};
	for (i = 0; i < 6; i = i + 1) {
	  distance = regardeVers(angles[i]);
	  if((distance <= distMax) && (distMax >= distMin)){
	  angle = angles[i];
	  }
	}

	return angle;
}

void ia_vaVersLObjetLePlusProche(){
	// Cherche la cible
	int angleCible = ia_chercheAngleCible(int ancienneDistance);
	// Se dirige vers elle
	
	stop();
}
