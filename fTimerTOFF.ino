/*-------------------------------
  ** fTimerTOFF **

v0.1 : Version d'origine

-------------------------------*/

void fTimerTOFF(byte i){

  //Capture du temps au démarrage de la tempo
  if (Timer[i].Start){
    Timer[i].TimeStart = millis();  
    Timer[i].Active = true;
    Timer[i].Out = true;
  }

  //Vérification du temps écoulé depuis le démarrage de la tempo
  if ((!Timer[i].Start && Timer[i].Active) && ( (millis() - Timer[i].TimeStart) >= (Timer[i].Duration*1000)) ){
    Timer[i].Active = false;
    Timer[i].Out = false;
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
