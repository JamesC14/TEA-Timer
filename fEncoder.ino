/*-------------------------------
  ** fEncoder **

v0.1 : Version d'origine

-------------------------------*/

void fEncoder(){
  //Déclaration variables
  static unsigned long last_time = 0;   //Sauvegarde temps 1ère interuption pour antirebond
  const byte ANTI_BOUNCE = 80;          //Anti rebond en ms
  
  if ((millis() - last_time) > ANTI_BOUNCE){
      //Incrémentation compteur si voie B à 1
      if (digitalRead(LANE_B)) counter++;
      //Décrémentation compteur si voie B à 0
      else counter--; 
      //Sauvegarde du temps de la première interruption
      last_time = millis();
  }
}

void fEncoder_v2(){
  //Déclaration variables
  static unsigned long last_time = 0;     //Sauvegarde temps 1ère interuption pour antirebond
  static bool tCW = false, tCCW = false;  //Sauvegarde du sens rotation
  const byte ANTI_BOUNCE = 20;            //Anti rebond en ms
  
  if ( (millis() - last_time) > ANTI_BOUNCE ) {

    if (!tCW && !tCCW) {
    //if (!digitalRead(LANE_A) && !tCW && !tCCW) {
      //Sauvegarde sens rotation horaire si voie B à 1
      if (digitalRead(LANE_B)) tCW = true;
      //Sauvegarde sens rotation anti-horaire si voie B à 0
      else tCCW = true;
    }
    
    else if (tCW || tCCW) {
    //else if (digitalRead(LANE_A) && (tCW || tCCW)){
      //Incrémentation compteur si rotation horaire
      if (tCW) rot_pos = true; //counter++;
      //Décrémentation compteur si rotation anti-horaire
      else if (tCCW) rot_neg = true; //counter--;
      //Reset sauvegarde sens rotation
      tCW = tCCW = false;
    }
    
    //Sauvegarde du temps de la première interruption
    last_time = millis();
  }
}
