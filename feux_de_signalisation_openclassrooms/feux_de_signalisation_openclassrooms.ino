/*
* Feux de signalisation OpenClassrooms
* http://goo.gl/TZGztH
*/
int feu_1_rouge  = 13;
int feu_1_orange = 12;
int feu_1_vert   = 11;
int feu_2_rouge  = 10;
int feu_2_orange = 9;
int feu_2_vert   = 8;

int temps_vert_orange    = 3000;
int temps_orange_rouge   = 1000;
int temps_rouge_vert     = 2000;

// the setup routine runs once when you press reset:
void setup() {
  pinMode(feu_1_rouge, OUTPUT);     
  pinMode(feu_1_orange, OUTPUT);     
  pinMode(feu_1_vert, OUTPUT);  
  pinMode(feu_2_rouge, OUTPUT);     
  pinMode(feu_2_orange, OUTPUT);     
  pinMode(feu_2_vert, OUTPUT); 
  
  digitalWrite(feu_1_rouge, HIGH); 
  digitalWrite(feu_2_rouge, HIGH);    
}


// the loop routine runs over and over again forever:
void loop() {
  // les feux sont rouges
  delay(temps_rouge_vert);
  // le feu 1 est vert
  digitalWrite(feu_1_rouge, LOW); 
  digitalWrite(feu_1_vert, HIGH); 
  delay(temps_vert_orange);
  // le feu 1 est orange
  digitalWrite(feu_1_vert, LOW); 
  digitalWrite(feu_1_orange, HIGH);
  delay(temps_orange_rouge);
  // les feux sont rouges
  digitalWrite(feu_1_orange, LOW); 
  digitalWrite(feu_1_rouge, HIGH);
  delay(temps_rouge_vert);
  // le feu 1 est rouge et le feu 2 est vert
  digitalWrite(feu_2_rouge, LOW); 
  digitalWrite(feu_2_vert, HIGH); 
  delay(temps_vert_orange);
  // le feu 2 est orange
  digitalWrite(feu_2_vert, LOW); 
  digitalWrite(feu_2_orange, HIGH);
  delay(temps_orange_rouge);
  // les feu 2 sont rouges
  digitalWrite(feu_2_orange, LOW); 
  digitalWrite(feu_2_rouge, HIGH);
}


