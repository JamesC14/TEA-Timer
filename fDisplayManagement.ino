/*-------------------------------
  ** fDisplayManagement **

v0.1 : Version d'origine
v0.2 : Version avec fonction pour affichage batterie et triangle indiquant les mélanges

-------------------------------*/

void fDisplayManagement(int _StepToDisplay){

//Déclaration des constante
const byte BAR_SIZE = 13;   //Hauteur de la barre de progression
const byte cBLACK = 0, cWHITE = 1, cINVERSE = 2;      //Couleur noir, blanc ou inverse la couleur

//Déclaration des variables
unsigned long BarGraph;
bool UpdateDiplay = false;

//NOTES
// Size text 1 : 6px / caractère
// Size text 2 : 12px / caractère
// Size text 3 : 18px / caractère
// Formule pour centrer : 128-(n*Size)/2

  //Reset
  ScreenOLED.clearDisplay();
  ScreenOLED.setTextColor(cWHITE);
  ScreenOLED.setFont();
      
  switch (_StepToDisplay){
    
    case SETUP:
      
      //LIGNE 1
      ScreenOLED.setTextSize(2);
      ScreenOLED.setCursor(10,0);
      ScreenOLED.println("TEA TIMER");
      
      //Affichage du logo de démarrage
      ScreenOLED.drawBitmap(43,20,TeaBITMAP,42,42,cWHITE);
      
      UpdateDiplay = true;
    break;

    case CONFIG:

      //LIGNE 1
      ScreenOLED.setTextSize(2);
      ScreenOLED.setCursor(22,0);
      ScreenOLED.println("REGLAGE");

      ScreenOLED.setCursor(0,20);

        //Affichage réglage position home
      if (_setting == SET_POS_HOME) ScreenOLED.print(">");
      else ScreenOLED.print(" ");
      ScreenOLED.print("HOME: ");
      ScreenOLED.println(pos_home);

        //Affichage réglage position work
      if (_setting == SET_POS_WORK) ScreenOLED.print(">");
      else ScreenOLED.print(" ");
      ScreenOLED.print("WORK: ");
      ScreenOLED.println(pos_work);

      ScreenOLED.setTextSize(1);
      ScreenOLED.setCursor(0,54);
      ScreenOLED.print("Position : ");
      ScreenOLED.print(Arm.read());

      ScreenOLED.setCursor(98, 54);
      ScreenOLED.print(Battery.Voltage());
      ScreenOLED.print("V");

      UpdateDiplay = true;
    break;
     
    
    case INITIALE:

      //LIGNE 1
      ScreenOLED.setTextSize(1);
      ScreenOLED.setFont(&Letters9pt);
      ScreenOLED.setCursor(30,12); //Minuteur 16 / Time 40
        //Affichage durée
      if (_setting == TIME) ScreenOLED.println("Minuteur");
        //Affichage mélange
      if (_setting == MIX) ScreenOLED.println("Melange");

      //Appel de la fonction pour déssiner la batterie
      fDrawBattery(cWHITE);

      //LIGNE 2
      ScreenOLED.setTextSize(1); //3
      ScreenOLED.setFont(&Numbers18pt);
        //Affichage durée
      if (_setting == TIME) {
        if (duration_min >= 10) ScreenOLED.setCursor(19,43); //64/3
        else ScreenOLED.setCursor(31,43); 
        ScreenOLED.print(duration_min);
        if (duration_s != 30) ScreenOLED.print(":0");
        else ScreenOLED.print(":");
        ScreenOLED.println(duration_s);
      }
        //Affichage mélange
      if (_setting == MIX) {
        ScreenOLED.setCursor(54,43);
        ScreenOLED.println(NbInfuse);
      }

      //LIGNE 3
      
      //Rectangle
      ScreenOLED.drawRect(0,ScreenOLED.height()-BAR_SIZE, ScreenOLED.width(), BAR_SIZE, cWHITE);
      //Remplissage avec auto power latch time left
      BarGraph = map(Timer[5].RemainingTime_ms, 0, Timer[5].Duration*1000, 2, ScreenOLED.width() - 3);
      ScreenOLED.fillRect(2, ScreenOLED.height()-BAR_SIZE + 2, BarGraph, BAR_SIZE - 4, cWHITE);
      
      //Mélange
      if (NbInfuse > 0) {
        for (int i = 1; i <= NbInfuse; i++) {
          //Appel de la fonction pour dessiner la position des mélanges
          fDrawMix(i, BAR_SIZE, cBLACK);
        }
      }

      UpdateDiplay = true;
    break;
    
    case INFUSE:
       
      //LIGNE 1
      ScreenOLED.setTextSize(1);
      ScreenOLED.setFont(&Letters9pt);
      ScreenOLED.setCursor(32,12);
      ScreenOLED.println("Infusion");

      //Appel de la fonction pour déssiner la batterie
      fDrawBattery(cWHITE);
        
      //LIGNE 2
      ScreenOLED.setTextSize(1);
      ScreenOLED.setFont(&Numbers18pt);

      if (Timer[3].TimeLeft_min >= 10) ScreenOLED.setCursor(19,43);
      else ScreenOLED.setCursor(31,43);
       
      ScreenOLED.print(Timer[3].TimeLeft_min);
      if (!blinker_1Hz){
        if (Timer[3].TimeLeft_s < 10) ScreenOLED.print(":0");
        else ScreenOLED.print(":");
      }
      else {
        if (Timer[3].TimeLeft_s < 10) ScreenOLED.print(" 0");
        else ScreenOLED.print(" ");
      }
      ScreenOLED.println(Timer[3].TimeLeft_s);
      
      //LIGNE 3

      //Rectangle
      ScreenOLED.drawRect(0,ScreenOLED.height()-BAR_SIZE, ScreenOLED.width(), BAR_SIZE, cWHITE);
      //Remplissage
      BarGraph = map(Timer[3].RemainingTime_ms, 0, Timer[3].Duration*1000, 2, ScreenOLED.width() - 3);
      ScreenOLED.fillRect(2, ScreenOLED.height()-BAR_SIZE + 2, BarGraph, BAR_SIZE - 4, cWHITE);

      //Mélange
      for (int i = 1; i <= NbInfuse; i++) {
        //Appel de la fonction pour dessiner la position des mélanges
        fDrawMix(i, BAR_SIZE, cBLACK);
      }

      UpdateDiplay = true;
    break;
    
    case DOWN:

      //LIGNE 1
      ScreenOLED.setTextSize(2);
      ScreenOLED.setCursor(22,64/3);
      ScreenOLED.println("TERMINE");

      UpdateDiplay = true;
    break;

    case MIXING:
      
      //LIGNE 1
      ScreenOLED.setTextSize(2);
      ScreenOLED.setCursor(22,64/3);
      ScreenOLED.println("MELANGE");

      UpdateDiplay = true;
    break;

    case POWER_OFF:

      //LIGNE 1
      ScreenOLED.setTextSize(2);
      ScreenOLED.setCursor(10,64/3);
      ScreenOLED.println("POWER OFF");

      UpdateDiplay = true; 

    break;
  }

  //Affichage
 if (UpdateDiplay){
  ScreenOLED.display();
 }

}

void fDrawMix(int _i, int _BarSize, int _color){

  //Calcul du positionement du 1er triangle
    //Coordonné sommet haut
  byte x1 = (ScreenOLED.width()/(NbInfuse+1))*_i; byte y1 = (ScreenOLED.height()-_BarSize) + 2;
    //Coordonné sommet gauche
  byte x2 = x1 - ((_BarSize - 4)/2); byte y2 = y1 + ((_BarSize - 4)/2);
    //Coordonné sommet droite 
  byte x3 = x1 + ((_BarSize - 4)/2); byte y3 = y2;
  
  //Dessin du 1er triangle
  ScreenOLED.fillTriangle(x1, y1, x2, y2, x3, y3, _color);
  
  //Calcul du 2nd triangle, en le décalant de 1 pixel par rapport au 1er pour pas qu'ils se marchent dessus
    //Coordonné sommet haut
  y1 = ScreenOLED.height() - 3;
    //Coordonné sommet gauche
  x2 += 1; y2 += 1;
    //Coordonné sommet droite
  x3 -= 1; y3 += 1;
   
  //Dessin du 2nd triangle
  ScreenOLED.fillTriangle(x1, y1, x2, y2, x3, y3, _color);
}

void fDrawBattery(int _color){

  //Déclaration des variables
  byte Bat_Width = 20, Bat_Height = 10, EndBat_Width = 1, EndBat_Height = 4;
  byte OffsetPosY = 2;

    //Corps de la batterie
  ScreenOLED.drawRoundRect((ScreenOLED.width() - (Bat_Width + EndBat_Width)), OffsetPosY, Bat_Width, Bat_Height, 1, _color);
    //Bout de la batterie
  ScreenOLED.fillRect((ScreenOLED.width() - EndBat_Width), ((Bat_Height - EndBat_Height)/2) + OffsetPosY, EndBat_Width, EndBat_Height, _color);
    //Remplissage en fonction de son %, si < 20% clignotement
  if (Battery.PerCent() > 20 || blinker_1Hz){
    ScreenOLED.fillRect((ScreenOLED.width() - (Bat_Width + EndBat_Width)) + 2, OffsetPosY + 2, map(Battery.PerCent(), 0, 100, 0, Bat_Width - 4), Bat_Height - 4, _color); 
  }
  
}
