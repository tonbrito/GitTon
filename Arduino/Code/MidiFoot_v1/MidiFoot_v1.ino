#include "MIDIUSB.h"

const int numControls          = 5;
const int BtnPins[numControls] = {2,3,4,5,6};
const int LedPin[numControls]  = {A5,A4,A3,A2,A1};     // Blue , Green, Yellow, White, Red
const int LedRGBPin[3]         = {11,10,9};                   // Red, Green, Blue
const int BtnRGBPin            = 7;   

//Midi messages per Button

  char Val_msg_btn1[3][5][13] = {
                                  {{"001069000000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                  {{"001001127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                  {{"001002127000"}, {"002001127000"}, {"002004002000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                };

  char Val_msg_btn2[3][5][13] = {
                                  {{"001069001001"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                  {{"001002127000"}, {"002001127000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                  {{"002003001000"}, {"002004002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                };
                                
  char Val_msg_btn3[3][5][13] = {
                                  {{"001069002002"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                  {{"001003127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                  {{"002003002000"}, {"002004002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                };
  
  
  char Val_msg_btn4[3][5][13] = {
                                  {{"001052127127"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                  {{"001004127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                  {{"002002127000"}, {"002005002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                };
  
  
  char Val_msg_btn5[3][5][13] = {
                                  {{"001053127127"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                  {{"001005127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                  {{"002004004002"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                };
                                

  char Val_msg[5][13];
   
// Variables
  int LedState[numControls]     = {0,0,0,0,0};
  int LedRGBState               = 0;
  int OldBtnState[numControls]  = {1,1,1,1,1};
  int NewBtnState[numControls]  = {0,0,0,0,0};
  int OldBtnRGBState = 1;
  int NewBtnRGBState = 0;
  
  int Pagina=0;
  
  int Midi_CH;
  int Midi_CC;
  int Midi_VaL_On;
  int Midi_VaL_Off;
  
  int RGBOn=255;
  int RGBOff=0;
  int dt=10;
  int debug=HIGH;


void setup() {
  //start serial connection
  //  Serial.begin(9600);
  Serial.begin(115200);
  //configure Switches [1-5] as an input and enable the internal pull-up resistor
  //configure Leds [1-5] as ouput 
  for (int i = 0; i < numControls; i++)
  {
    pinMode(BtnPins[i], INPUT_PULLUP);
    pinMode(LedPin[i], OUTPUT);
  }


  //configure Switches [6] as an input and enable the internal pull-up resistor
  
  pinMode(BtnRGBPin, INPUT_PULLUP);

  //configure RGB Led as ouput
  for (int i = 0; i < 3; i++)
  {
    pinMode(LedRGBPin[i], OUTPUT);
    digitalWrite(LedRGBPin[i],invertColor(RGBOff));
  }

  //Enable RGB LED with Magenta Colour
    SubRGBMagenta();
  //digitalWrite(LedRGBPin[0],invertColor(RGBOn));  // RGB Red

  
  if (debug == HIGH){
    Serial.print("Btn State ");
    Serial.print(digitalRead(BtnPins[0]));
  }

//  Val_msg [0][0] = Val_msg_btn1[0][0];
//  strcpy(Val_msg[0], Val_msg_btn1[0]);
  delay(7000);


  memcpy(Val_msg, Val_msg_btn1[2], sizeof(Val_msg_btn1[1]));

  Serial.println(sizeof(Val_msg_btn1[1]));


  for(int i = 0; i < 5; i++) {
//    memcpy(Val_msg[i], Val_msg_btn1[0][i], sizeof(Val_msg_btn1[0][i]));
//    Val_msg [i][0] = Val_msg_btn1[0][i][0];
    Serial.print("Message: ");
    Serial.println(i + 1);
    Serial.print("  Val msg: ");
    Serial.println((char*)Val_msg_btn1[2][i]);
    Serial.print("  Val msg To: ");
    Serial.println((char*)Val_msg[i]);
    
  }
  
}

void loop() {
  for(int i = 0; i < numControls; i++) {
    delay(dt);
    NewBtnState[i]=digitalRead(BtnPins[i]);

    // Toggle State for Pages 1 and 2
    if (OldBtnState[i] == 0 && NewBtnState[i] ==1){
      if (LedState[i] == 0) {
        if (debug == HIGH){
          Serial.print("button on --> ");
          Serial.println(i);          
        }
    //    if (i == 0){    // 1st Switch
          Midi_msg_prep(LedRGBState, i,1);
          Serial.println("Sending Control Change");
    //    }
        digitalWrite(LedPin[i], HIGH );
        LedState[i] = 1;
        SubLedsOff(i);
        SubBtnOff(i);
      } else {

        if (debug == HIGH){
          Serial.print("button off --> ");
          Serial.println(i);          
        }
//        if (LedRGBState != 0) {
          Midi_msg_prep(LedRGBState, i,0);
//        }  
        digitalWrite(LedPin[i], LOW);
        LedState[i] = 0;
      }
    }
  //  if (debug == HIGH){
  //    
  //    Serial.println(OldBtnState[i]);
  //    Serial.println(NewBtnState[i]);
  //  }      
    OldBtnState[i] = NewBtnState[i];
  }
  delay(dt);
  NewBtnRGBState=digitalRead(BtnRGBPin);
  if (OldBtnRGBState == 0 && NewBtnRGBState ==1){

    switch (LedRGBState) {
      case 0:
        LedRGBState = 1;
        SubRGBLedsOff();
        SubRGBAqua();        
        digitalWrite(LedRGBPin[LedRGBState],invertColor(RGBOn));        // RGB Green On
        break;
      case 1:
        LedRGBState = 2;
        SubRGBLedsOff();
        SubRGBOrange();
        // digitalWrite(LedRGBPin[LedRGBState],invertColor(RGBOn));    // RGB Blue On
        break;
      case 2:
        LedRGBState = 0;
        SubRGBLedsOff();
        SubRGBMagenta();
        // digitalWrite(LedRGBPin[LedRGBState],invertColor(RGBOn));    // RGB Magenta On
        break;
      default:
        break;
    }
  }
  OldBtnRGBState = NewBtnRGBState;
}

int invertColor(int color) {
  return (color * -1) +255;
}

// Btn/Leds Functions
// ==================

  void SubLedsOff(int btnind) {
    for(int i = 0; i < numControls; i++) {
      if ( i != btnind) {
        digitalWrite(LedPin[i],LOW);
        LedState[i] = 0;
      }    
    }
  }
  
  void SubBtnOff(int btnind) {
    for(int i = 0; i < numControls; i++) {
      if ( i != btnind) {
        OldBtnState[i] = 1;
      }    
    }
  }
  
  void SubRGBLedsOff() {
    for(int i = 0; i < 3; i++) {
        digitalWrite(LedRGBPin[i],invertColor(RGBOff)); 
    }
  }
  
  void SubRGBMagenta() {
    analogWrite(LedRGBPin[0],invertColor(255));
    analogWrite(LedRGBPin[1],invertColor(0));
    analogWrite(LedRGBPin[2],invertColor(100));  
  }
  
  void SubRGBAqua() {
    analogWrite(LedRGBPin[0],invertColor(0));
    analogWrite(LedRGBPin[1],invertColor(255));
    analogWrite(LedRGBPin[2],invertColor(80));  
  }
  
  void SubRGBOrange() {
    analogWrite(LedRGBPin[0],invertColor(0));
    analogWrite(LedRGBPin[1],invertColor(102));
    analogWrite(LedRGBPin[2],invertColor(153));  
  }

//* Midi Library Funcions 
// ======================
  void noteOn(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOn);
  }
  
  void noteOff(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOff);
  }
  
  void controlChange(byte channel, byte control, byte value) {
    midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
    MidiUSB.sendMIDI(event);
  }

//* Sending Midi Message
//  ==================== 
  void Midi_msg_prep(int pag, int btn, int OffOn){

    switch (btn) {
      case 0:    
        memcpy(Val_msg, Val_msg_btn1[pag], sizeof(Val_msg_btn1[pag]));
        break;
      case 1:
        memcpy(Val_msg, Val_msg_btn2[pag], sizeof(Val_msg_btn2[pag]));
        break;
      case 2:
        memcpy(Val_msg, Val_msg_btn3[pag], sizeof(Val_msg_btn3[pag]));
        break;
      case 3:
        memcpy(Val_msg, Val_msg_btn4[pag], sizeof(Val_msg_btn4[pag]));
        break;
      case 4:
        memcpy(Val_msg, Val_msg_btn5[pag], sizeof(Val_msg_btn5[pag]));
        break;
      default:
        break;
    }

    for (int ind_msg = 0; ind_msg < 5; ind_msg++) {
      
      String Val_Str = Val_msg[ind_msg];
      if (Val_Str.substring(0,3).toInt() != 0) {
        // Substring (var (pos_ini , pos_fin)
        Midi_CH       = ((Val_Str.substring(0,3).toInt())-1);
        Midi_CC       = (Val_Str.substring(3,6).toInt());
        Midi_VaL_On   = (Val_Str.substring(6,9).toInt());
        Midi_VaL_Off = (Val_Str.substring(9,12).toInt());
        Serial.print(" Pagina ");       
        Serial.print(pag);
        Serial.print(" ");       
        Serial.print(" Message  ");       
        Serial.print(ind_msg + 1);
        Serial.print(" = ");       
        Serial.print(Val_Str);
        Serial.print(" -  Channel: "); 
        Serial.print(Midi_CH + 1);
        Serial.print(" -  CC: "); 
        Serial.print(Midi_CC);
        Serial.print(" -  CC_VAL on: "); 
        Serial.print(Midi_VaL_On);
        Serial.print(" -  CC_VAL off: "); 
        Serial.print(Midi_VaL_Off);               
        Serial.print(" $$$ On Off: "); 
        Serial.println(OffOn);               
        delay(10);
        if (OffOn == 1) {
          Serial.print("Midi On ");
          Serial.println(Midi_VaL_On);
          controlChange(Midi_CH, Midi_CC, Midi_VaL_On);
        } else {
          Serial.print("Midi Off ");
          Serial.println(Midi_VaL_Off);
          controlChange(Midi_CH, Midi_CC, Midi_VaL_Off);
        }
        MidiUSB.flush();
       }
    }
  }    


//*
