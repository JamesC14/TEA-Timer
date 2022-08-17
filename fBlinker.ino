/*-------------------------------
  ** fBlinker **

v0.1 : Version d'origine

-------------------------------*/ 
bool fBlinker(byte i, byte j, float on, float off){
  Timer[i].Duration = off;
  Timer[j].Duration = on;
  
  Timer[i].Start = !Timer[j].Out;
  Timer[j].Start = Timer[i].Out;
  
  fTimerTON(i);
  fTimerTON(j);
  
  return Timer[i].Out;
}

void fBlinkerVoid(byte i, byte j, float on, float off){
  Timer[i].Duration = off;
  Timer[j].Duration = on;
  
  Timer[i].Start = !Timer[j].Out;
  Timer[j].Start = Timer[i].Out;
  
  fTimerTON(i);
  fTimerTON(j);
}
