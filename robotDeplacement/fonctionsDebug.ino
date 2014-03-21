void debug(String texte){
  Serial.print("*** ");Serial.print(texte);Serial.println(" ***");  
  Serial.print("Droite :\t");Serial.print(distDroite);Serial.print("\tGauche :\t");Serial.print(distGauche);Serial.print("\tDevant :\t");Serial.println(distDevant);
  Serial.print("Position :\t");Serial.println(pos);
  Serial.println("---");
}

void debugRotation(float direction, float positionDebut, float positionActuelle, int positionMin, int positionMax){
  Serial.print("*** ");Serial.print("DebugRotation , direction : \t");Serial.print(direction);Serial.println(" ***");  
  Serial.print("Debut :\t");Serial.print(positionDebut);
  Serial.print("\tActuelle :\t");Serial.print(positionActuelle);
  Serial.print("\tMin :\t");Serial.print(positionMin);
  Serial.print("\tMax :\t");Serial.println(positionMax);
  Serial.println("---");
}
