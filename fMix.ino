/*-------------------------------
  ** fMix **

v0.1 : Version d'origine

-------------------------------*/

void Mix(byte _nb_mvt, byte _offset, float _speedUp, float _speedDown){
  //Sauvegarde position du servomoteur
  byte posSaved = Arm.read();

  //Mouvement du sachet
  for (byte i = 1; i <= _nb_mvt; i++){
    ServoPos(posSaved + _offset, _speedUp, false);
    ServoPos(posSaved, _speedDown, false);
  }
}
