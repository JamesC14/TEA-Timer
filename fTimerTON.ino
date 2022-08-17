/*-------------------------------
  ** fTimerTON **

v0.1 : Version d'origine

-------------------------------*/

void fTimerTON(byte i){

  //Reset
  if (!Timer[i].Start){
    Timer[i].Active = false;
    Timer[i].Out = false;
    //Timer[i].RemainingTime_ms = 0;
  }

  //Capture du temps au démarrage de la tempo
  if (Timer[i].Start && !Timer[i].Active){
    Timer[i].TimeStart = millis();  
    Timer[i].Active = true;
    Timer[i].Out = false;
  }

  //Vérification du temps écoulé depuis le démarrage de la tempo
  if ((Timer[i].Start && Timer[i].Active) && ( (millis() - Timer[i].TimeStart) >= (Timer[i].Duration*1000)) ){
    Timer[i].Out = true;
    Timer[i].RemainingTime_ms = 0;
  }

  //Calcul du temps restant
  if (Timer[i].Active){
    //format : ms
    Timer[i].RemainingTime_ms = (Timer[i].Duration*1000) - (millis() - Timer[i].TimeStart);
    //format : mm:ss
    Timer[i].TimeLeft_min = (Timer[i].RemainingTime_ms / 1000) / 60;
    Timer[i].TimeLeft_s = (Timer[i].RemainingTime_ms / 1000) - (Timer[i].TimeLeft_min * 60);
  }
}
