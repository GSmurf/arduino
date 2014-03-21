/* ------------------------------------------------------------------------
* Code moteur
------------------------------------------------------------------------ */
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
