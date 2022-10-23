/*-------------------------------
  ** fServoPos **

v0.1 :  Version d'origine
v0.2 :  Génération de la rampe dans la fonction
v0.3 :  Sauvegarde ancienne position avec readMicroseconds() au lieu de variable externe
v0.4 :  Passage en ° au lieu de µs (servo 9g)

-------------------------------*/

void ServoPos(int _position, float _speed, bool _stop){
  int rPosition, oldPosition = Arm.read();              //Position pour tampe + sauvegarde la position d'origine
  bool arret_pos = false;                               //Arrêt du position si paramétré avec _stop = true;
  unsigned long rampDuration, timeStart;                //Temps total pour parcourir la distance + temps départ positionnement
  unsigned long currentMillis;                          //Temps boucle do while
  
  //Activation du positionnement du servomoteur
  Arm.attach(SERVO);
  
  //Calcul du temps nécessaire pour parcourir la distance en ms
  rampDuration = float(abs(oldPosition-_position)/_speed)*1000; //Valeur absolue car si distance négative

  //Attente servo avant lancement positionnement
  while (!Arm.attached());

  //Enregistrement du temps départ positionement
  timeStart = millis();
  
  //Positionnement
  do {
    //Sauvegarde du temps
    currentMillis = millis();
    //Calcul position et positionnement servo en fonction de la rampe (temps)
    rPosition = map(rampDuration - (currentMillis - timeStart), rampDuration, 0, oldPosition, _position);
    Arm.write(rPosition);
    //Si arrêt paramètré + conditions
    arret_pos = _stop && digitalRead(BOUTON);
  } while ( ((currentMillis - timeStart) < rampDuration) && !arret_pos );  //Tant que la rampe n'est pas terminée, ou qu'il n'y a pas de condition d'arrêt

  //Désactivation du positionnement du servomoteur
  Arm.detach();
}
