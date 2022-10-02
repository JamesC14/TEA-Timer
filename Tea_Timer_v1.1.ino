/*------------------------------------------------------------------------------------------------------------------------------
  ** TEA TIMER **

Version v1.0  : Version final
Version v1.1  : Optimisation des polices de caractères

------------------------------------------------------------------------------------------------------------------------------*/

//Class
class cBattery{
  private:
    byte pinBat;  //Contient le numéro de pin analogique pour mesurer la tension
    float vBat;   //Contient la tension de la batterie
    int pBat;     //Contient le pourcentage de la batterie

    float tempVal, averageVal = 4.2;  //Variable pour calcul intermédiaire (besoin d'être sauvegardé en dehors des fonctions)\
                                        Forçage de la valeur moyenne à 4.2V (100%) pour éviter d'attendre plusieurs cycles de l'µc pour avoir un retour stable
  public:
    void Init(int _pinBat){
      //Mémorise la pin analogique où est branché la batterie pour mesurer
      pinBat = _pinBat;
    }  
    float Voltage(){
      //Mapping de l'entrée analogique vs tension max entrée analogique
      tempVal = float(map(analogRead(pinBat), 0, 1023, 0, 500))/100;
      
      //Moyenne glissante
      averageVal = 0.9 * averageVal + 0.1 * tempVal;

      //Met à jour la tension uniquement si elle est différente de +/-0.02V
      if ( (averageVal > (vBat + 0.02)) || (averageVal < (vBat - 0.02)) ) vBat = averageVal;
     
      //Retour tension batterie filtrée
      return vBat;  
    }
    int PerCent(){
      //Mesure de la tension batterie
      float v = Voltage();

      //Correspondance tension - %
      if (v > 4.10) pBat = 100;
      else if (v > 3.97) pBat = 90;
      else if (v > 3.92) pBat = 80;
      else if (v > 3.87) pBat = 70;
      else if (v > 3.83) pBat = 60;
      else if (v > 3.79) pBat = 50;
      else if (v > 3.75) pBat = 40;
      else if (v > 3.70) pBat = 30;
      else if (v > 3.60) pBat = 20;
      else if (v > 3.30) pBat = 10;
      else pBat = 0;

      //Retour pourcentage batterie
      return pBat; 
    }
};

//Bibliothèques
#include <Servo.h>                //Pilotage du servmoteur
#include <Adafruit_SSD1306.h>     //Gestion écran OLED
#include <Fonts/Letters9pt.h>  //Police pour l'écran OLED FreeSans9pt7b
#include <Fonts/Numbers18pt.h> //Police pour l'écran OLED FreeSans18pt7b
#include <OneButton.h>            //Gestion bouton

//DEFINE
#define LED_INFUSE 8
#define BOUTON 4
#define SERVO 5
#define OLED_ADR 0x3C
#define BUZZER 7
#define V_BAT A1
#define LANE_A 2
#define LANE_B 3
#define POWER_LATCH 9
#define SET_WORK A3
#define SET_HOME A2

//Déclaration des objets
Servo Arm;
Adafruit_SSD1306 ScreenOLED(128, 64, &Wire, -1);
cBattery Battery;
OneButton btn(BOUTON, false, false);  //Boutton actif lorsque signal = 1 // Pull down avec circuit auto power latch

//Défintion des structures
struct sTimer{
  unsigned long TimeStart, RemainingTime_ms;
  float Duration;
  byte TimeLeft_s, TimeLeft_min;
  bool Start, Active, Out;
};

//Enumération des étapes
enum :byte {SETUP, INITIALE, DOWN_ARM, READJUSMENT, INFUSE, UP_ARM, DOWN, MIXING, POWER_OFF, CONFIG} _step;
enum :byte {TIME, MIX, SET_POS_HOME, SET_POS_WORK} _setting;

//Déclaration des constantes
int HOME_POSITION = 90, WORK_POSITION = 35, OFFSET = 10;              //Position du servomoteur 1477 = 90° / 546 = 0°
const float SPEED = 30;                                               //Unit/s
const byte DURATION_MIN = 0, DURATION_MAX = 10;                       //Durée min/max du minuteur

//Déclaration des variables
sTimer Timer[10];                         //Tableau de timers
float duration;                           //Durée d'infusion sélectionnée
int duration_s, duration_min;             //Durée en mm:ss pour affichage
int PositionSaved;                        //Sauvegarde positions du servo

unsigned long TabNbInfuse[5];             //Tableau découpant l'infusion en x temps égale pour mélange
int NbInfuse = 0;                         //Nombre de remonté du sachet pendant infusion
byte indexNbInfuse = NbInfuse;            //Index nombre de mélange pour calcul temps
bool mix_infuse;                          //Bit pour lancer le mélange
bool infuse_finished;                     //Bit infusion terminée

bool blinker_1Hz;                         //Blinker 1hz
bool blinker;                             //Blinker

bool Click, DClick, MClick, LPress;       //Bouton

byte counter = 6;                         //Compteur encoder, valeur par défaut 6 pour 6/2 = 3mins  

bool power_off = false;                   //Extinction automatique

bool rot_pos, rot_neg;                    //Retour rotation codeur

//Variables "temporaire" pour réglage position
int pos_work, pos_home;
bool select_posWork;

//Logo de démarrage
const unsigned char TeaBITMAP [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x30, 0x60, 0x30, 0x00, 0x00, 0x00, 0x20, 
  0x60, 0x20, 0x00, 0x00, 0x00, 0x20, 0x20, 0x60, 0x00, 0x00, 0x00, 0x20, 0x30, 0x20, 0x00, 0x00, 
  0x00, 0x30, 0x10, 0x30, 0x00, 0x00, 0x00, 0x18, 0x10, 0x10, 0x00, 0x00, 0x00, 0x18, 0x10, 0x10, 
  0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x10, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0x80, 0x00, 0x07, 0xff, 0xff, 0xff, 0x80, 0x00, 
  0x04, 0x00, 0x03, 0x00, 0x80, 0x00, 0x04, 0x00, 0x03, 0x00, 0x80, 0x00, 0x04, 0x00, 0x03, 0x00, 
  0xff, 0x00, 0x04, 0x00, 0x03, 0x00, 0x81, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf9, 0x80, 0x04, 0x00, 
  0x03, 0x00, 0xfd, 0x80, 0x04, 0x00, 0x03, 0x00, 0x85, 0x80, 0x04, 0x00, 0x03, 0x00, 0x85, 0x80, 
  0x06, 0x00, 0x04, 0x81, 0x85, 0x80, 0x06, 0x00, 0x08, 0x41, 0xb9, 0x80, 0x02, 0x00, 0x08, 0x41, 
  0xe1, 0x00, 0x03, 0x00, 0x08, 0x41, 0x0f, 0x00, 0x03, 0x00, 0x08, 0x43, 0x38, 0x00, 0x01, 0x00, 
  0x08, 0x43, 0xe0, 0x00, 0x01, 0x80, 0x08, 0x47, 0x00, 0x00, 0x00, 0xc0, 0x0f, 0xcc, 0x00, 0x00, 
  0x00, 0x40, 0x0f, 0xc8, 0x00, 0x00, 0x00, 0x20, 0x00, 0x18, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 
  0xff, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x18, 0x00, 0x00, 0x00, 0x06, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 
  0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup()
{
  //Configuration des entrées
  pinMode(LANE_A, INPUT_PULLUP);
  pinMode(LANE_B, INPUT_PULLUP);

  //Interruption
  attachInterrupt(digitalPinToInterrupt(LANE_A), fEncoder_v2, CHANGE);
  
  //Configuration des sorties
  pinMode(LED_INFUSE,OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(POWER_LATCH, OUTPUT);
  
  //Initialisation des sorties
  digitalWrite(LED_INFUSE, HIGH);
  digitalWrite(POWER_LATCH, HIGH);
  
  //Affectation et réglage servomoteur
  Arm.attach(SERVO);
  HOME_POSITION = map(analogRead(SET_HOME), 0, 1023, 0, 180);   //Réglage position HOME en fonction du potentiomètre
  WORK_POSITION = map(analogRead(SET_WORK), 0, 1023, 0, 180);   //Réglage position WORK en fonction du potentiomètre
  Arm.write(HOME_POSITION);

  //Affectation des fonctions pour le bouton
  btn.attachClick(fClick);
  btn.attachDoubleClick(fDClicks);
  btn.attachLongPressStart(fLPress);
  btn.attachMultiClick(fMClicks);

  //Initialisation de la gestion batterie
  Battery.Init(V_BAT);

  //Initialisation des variables
  _step = INITIALE;
  _setting = TIME;
  Timer[5].Duration = 5*60;                                     //5min avant coupure alimentation automatique

  //Démarrage de l'écran OLED
  ScreenOLED.begin(SSD1306_SWITCHCAPVCC, OLED_ADR);

  //Gestion écran
  fDisplayManagement(SETUP);

  delay(1000);
  tone(BUZZER, 700, 300);
  delay(500);
  Arm.detach();
  
}

void loop()
{

//ETAPE
  switch (_step){
//STEP CONFIG : Réglage
      case CONFIG:
        
        //Lecture valeurs de réglage
        pos_home = map(analogRead(SET_HOME), 0, 1023, 0, 180);
        pos_work = map(analogRead(SET_WORK), 0, 1023, 0, 180);

        //Positionnement du servomoteur
        if (_setting == SET_POS_WORK) ServoPos(pos_work, SPEED, false);
        if (_setting == SET_POS_HOME) ServoPos(pos_home, SPEED, false);

        //Changement réglage
        if (rot_pos || rot_neg){
          if (_setting == SET_POS_HOME) _setting = SET_POS_WORK;
          else if (_setting == SET_POS_WORK) _setting = SET_POS_HOME;
          rot_pos = rot_neg = false;
        }
        
        //Exit mode réglage
        if (LPress){
          ServoPos(pos_home, SPEED, false);
          _setting = TIME;
          _step = INITIALE;
        }
  
      break;

//STEP INITIALE : Affichage minuteur
      case INITIALE:

          //Réglage si incrémentation codeur
          if (rot_pos) {
            if (_setting == TIME) counter++;
            if (_setting == MIX) NbInfuse++;
            rot_pos = false;
          }

          //Réglage si décrémentation codeur
          if (rot_neg) {
            if (_setting == TIME) counter--;
            if (_setting == MIX) NbInfuse--;
            rot_neg = false;
          }
 
        //Limites codeur (250 si décrémentation depuis 0 on arrive à 255 valeur d'un byte)
        if (counter > 250) counter = DURATION_MAX*2;
        if (counter > DURATION_MAX*2) counter = DURATION_MIN;

        //Limites NbInfuse
        if (NbInfuse > 4) NbInfuse = 0;
        if (NbInfuse < 0) NbInfuse = 4;

        //Conversion du compteur en durée
        duration = float(counter) / 2;
        //Conversion pour affichage mm:ss sur l'écran
        duration_min = duration;
        //Si float > int, c'est qu'il y a des décimales
        if (duration > duration_min) duration_s = 30;
        else duration_s = 0;

        //Calcul des temps intermédiaire pour remonter le sachet pendant l'infusion
        if (NbInfuse > 0) {
          indexNbInfuse = NbInfuse;
          for (int i = 1; i <= NbInfuse; i++) {
            TabNbInfuse[i] = ((duration*60*1000)/(NbInfuse+1)) * i;
          }
        }

        //Lancement de l'infusion
        if (DClick || (Click && _setting == MIX)) {
          //Passage à l'étape suivante
          _step = DOWN_ARM;
        }

        //Mode réglage
        if (Click && _setting == TIME) {
          _setting = MIX;
        }

        //Mode configuration
        if (MClick) {
          _step = CONFIG;
          _setting = SET_POS_HOME;
        }

      break;
      
//STEP DOWN_ARM : Descente du bras
      case DOWN_ARM:
        //Positionnement du servo
        ServoPos(WORK_POSITION, SPEED, true);
        
        //Passage à l'étape suivante
        _step = INFUSE; //READJUSMENT | INFUSE
        
      break;
      
//STEP READJUSMENT : Recalage du bras
      case READJUSMENT:
        //Positionnement du servo
        ServoPos(Arm.read() + OFFSET, SPEED, false);
    
        //Passage à l'étape suivante
        _step = INFUSE;
      break;
      
//STEP INFUSE : Infusion
      case INFUSE:
        //Configuration du timer pour la durée d'infusion
        Timer[3].Duration = duration*60;
        Timer[3].Start = true;
        
        //Appel de la tempo
        fTimerTON(3);
          
        digitalWrite(LED_INFUSE, blinker_1Hz);
  
        //Mélange, timeout écoulé
        if ( (Timer[3].RemainingTime_ms > (TabNbInfuse[indexNbInfuse] - 50)) && (Timer[3].RemainingTime_ms < (TabNbInfuse[indexNbInfuse] + 50)) ){
          indexNbInfuse --;
          mix_infuse = true;  //Set la demande de mélange
        }
  
        //Mélange, ou mélange forcé avec le bouton
        if (mix_infuse || DClick){
          mix_infuse = false;                     //Reset la demande de mélange
          fDisplayManagement(MIXING);             //Affichage OLED
          PositionSaved = Arm.read();             //Sauvegarde de la position
          ServoPos(HOME_POSITION, SPEED, false);  //Remontée du sachet
          ServoPos(PositionSaved, SPEED, false);  //Redescente du sachet
        }

        //Fin de l'infusion, temps écoulé ou appuie long sur le bouton
        if (Timer[3].Out || LPress){
          Timer[3].Start = false;
          //Passage à l'étape suivante
          _step = DOWN;
        }
      break;
      
//STEP DOWN : Infusion terminée
      case DOWN:
        //Indication sur l'infusion est terminée
        infuse_finished = true;
        digitalWrite(LED_INFUSE, HIGH); //Led d'activité pleinement allumé
        tone(BUZZER, 700, 1000);        //Buzzer

        //Remise à 0 du nombre de mélange
        NbInfuse = 0;
        
        //Passage à l'étape suivante
        _step = UP_ARM;
      break;
      
//STEP UP_ARM : Remonté du bras
      case UP_ARM:
        //Positionnement du servo
        ServoPos(HOME_POSITION, SPEED, false);
        Timer[5].Duration = 10*1; //10

        //Passage à l'étape suivante
        _step = INITIALE;
        //Mode réglage sur durée
        _setting = TIME;
      break;

//STEP POWER_OFF : Coupure alimentation
      case POWER_OFF:
        //Coupure alimentation
        if (power_off && !digitalRead(BOUTON)) {
          delay(1000);
          digitalWrite(POWER_LATCH, LOW);
        }
        
        //Activation de la coupure alimentation
        power_off = true;
      break;
  }

//Mise à jour des tempos
  fTimerTON(3);

//Coupure alimentation
  Timer[4].Duration = 3;  //Appuie de 3sec coupe l'alimentation
  Timer[4].Start = (_step == INITIALE && digitalRead(BOUTON));
  fTimerTON(4);
  
  Timer[5].Start = (_step == INITIALE && !DClick && !LPress && digitalRead(LANE_A) && digitalRead(LANE_B));
  fTimerTON(5);
  
  //Passage à l'étape coupure alimentation si appuie long sur le bouton extinction automatique ou temps écoulé coupure automatique
  if (Timer[4].Out || Timer[5].Out) _step = POWER_OFF;

//Gestion écran
  fDisplayManagement(_step);

//Blinker
  blinker_1Hz = fBlinker(6, 7, 0.50, 0.50);
  blinker = fBlinker(8, 9, 0.02, 0.02);
  

//LED
  if (infuse_finished) digitalWrite(LED_INFUSE, blinker);

//MAJ bouton Click, DClick, MClick, LPress;
  Click = DClick = MClick = LPress = false;
  btn.tick();

}
