/*-------------------------------
  ** fEncoder **

v0.1 : Version d'origine
v0.2 : Enregistrement états voies A et B au début de l'interruption et contrôle sens de rotation
       uniquement si la voie A est à 0.
v0.3 : Version avec antirebond MC14490 --> fEncoderMC14490

-------------------------------*/

void fEncoder(){
  //Déclaration variables
  bool stateLANE_A = digitalRead(LANE_A), stateLANE_B = digitalRead(LANE_B);
  unsigned long currentMillis = millis();
  static unsigned long last_time = 0;     //Sauvegarde temps 1ère interuption pour antirebond
  static bool tCW = false, tCCW = false;  //Sauvegarde du sens rotation
  const byte ANTI_BOUNCE = 20;            //Anti rebond en ms
  
  if ( (currentMillis - last_time) > ANTI_BOUNCE ) {

    if (!tCW && !tCCW && !stateLANE_A) {
      //Sauvegarde sens rotation horaire si voie B à 1
      if (stateLANE_B) tCW = true;
      //Sauvegarde sens rotation anti-horaire si voie B à 0
      else tCCW = true;
    }
    
    else if (tCW || tCCW) {
      //Incrémentation compteur si rotation horaire
      if (tCW) cw = true;
      //Décrémentation compteur si rotation anti-horaire
      else if (tCCW) ccw = true;
      //Reset sauvegarde sens rotation
      tCW = tCCW = false;
    }
    
    //Sauvegarde du temps de la première interruption
    last_time = currentMillis;
  }
}

void fEncoderMC14490(){

  if (digitalRead(LANE_B)) ccw = true;
  else cw = true;
  
}
