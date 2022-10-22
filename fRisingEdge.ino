/*-------------------------------
  ** fRisingEdge **

v0.1 : Version d'origine

-------------------------------*/

bool fRisingEdge(byte _pin){
  //Déclaration des variables
  static unsigned long LastTime[5] {0,0,0,0,0};
  static bool LastState[5] {false, false, false, false, false};
  unsigned long currentMillis = 0;
  bool currentState = false, RisingEdge = false;
  const byte ANTI_BOUNCE = 20;            //Anti rebond en ms

  //Lecture de l'état de l'entrée
  currentState = digitalRead(_pin);

  //Comparaison si l'état actuel est différent de l'ancien
  if (LastState[_pin] != currentState) {
    currentMillis = millis(); //Sauvegarde du temps
    //Gestion anti rebond
    if ((currentMillis - LastTime[_pin]) >= ANTI_BOUNCE) {
      //Si front descendant
      if (currentState){
        RisingEdge = true;
      }
    }
    //Enregistrement du temps pour la gestion de l'anti-rebond
    LastTime[_pin] = currentMillis;
  }
  //Sauvegarde de l'état actuelle pour prochaine détection du front
  LastState[_pin] = currentState;

  return RisingEdge;
}
