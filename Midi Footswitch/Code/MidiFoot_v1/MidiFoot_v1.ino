#include "MIDIUSB.h"
#include <avr/pgmspace.h>

//Midi messages per Button  [page] [Message] [Val_Message]
//    Example:  - Two messages when we press Button 2 On Page 1
//              Page: 1 - Message: 0 - Value 001002127000  [Channel]
//              Page: 1 - Message: 1 - Value 002001127000
//              
//   Val_Message = 12 bytes as below
//      - 3 bytes - Channel
//      - 3 bytes - CC Midi
//      - 3 bytes - CC Val On
//      - 3 bytes - CC Val Off
//  

const char Val_msg_btn1[3][5][13] = {
                                      {{"001069000000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001001127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"001002127000"}, {"002001127000"}, {"002004002000"},{"002002000000"},{"002003002000"}}  //Pag 2 
                                    };

const char Val_msg_btn2[3][5][13] = {
                                      {{"001069001001"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001002127000"}, {"002001127000"}, {"002004002000"},{"002002000000"},{"002003002000"}}, //Pag 1
                                      {{"002002127000"}, {"002005002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };
                                
const char Val_msg_btn3[3][5][13] = {
                                      {{"001069002002"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001003127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"002004004002"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };
  
const char Val_msg_btn4[3][5][13] = {
                                      {{"001052127127"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001004127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"002003001000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };
  
const char Val_msg_btn5[3][5][13] = {
                                      {{"001053127127"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001005127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"002003002000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };
                                

//Midi Auxiliar Array
  char Val_msg[5][13];

// Buttons / Led Definitions / Table Size
const int8_t numControls           = 5;
const int8_t BtnPins[numControls]  = {2,3,4,5,6};
const int8_t LedPin[numControls]   = {A5,A4,A3,A2,A1};     // Blue , Green, Yellow, White, Red
const int8_t LedRGBPin[3]          = {11,10,9};            // Red, Green, Blue
const int8_t BtnRGBPin             = 7;   
const int8_t sizeTab               = 65;

// Variables
  int8_t LedState[3][5]            = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
  int8_t OldBtnState[3][5]         = {{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}};
  int8_t NewBtnState[3][5]         = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
  int8_t Page                      = 0;
  int8_t midisendmsg               = 0;
  int8_t OldBtnRGBState            = 1;
  int8_t NewBtnRGBState            = 0;
  int    RGBOn                     = 255;
  int8_t RGBOff                    = 0;
  int8_t BtnTS                     = 3;
  int8_t BtnKOT                    = 4;
  
  int8_t dt                        = 20;
  int8_t debug                     = HIGH;
  

// Variables for Midi Message
  int Midi_CH;
  int Midi_CC;
  int Midi_VaL_On;
  int Midi_VaL_Off;
  
void setup() {
  //start serial connection
  //  Serial.begin(9600);
  Serial.begin(115200);

  //configure Switches [1-5] as Input and enable the internal pull-up resistor
  //configure Leds [1-5] as ouput 
  for (int i = 0; i < numControls; i++) {
    pinMode(BtnPins[i], INPUT_PULLUP); // Configure Internall Pull-up for each switch
    pinMode(LedPin[i], OUTPUT);
  }

  //configure Switches [6] as an input and enable the internal pull-up resistor
  pinMode(BtnRGBPin, INPUT_PULLUP);

  //configure RGB Led as ouput
  for (int i = 0; i < 3; i++) {
    pinMode(LedRGBPin[i], OUTPUT);
    digitalWrite(LedRGBPin[i],invertColor(RGBOff));
  }
    
  for(int i2 = 0; i2 < numControls; i2++) {
    for(int i3 = 0; i3 < 3; i3++) {
      delay(50);   
      digitalWrite(LedPin[i2], HIGH);
      delay(50);
      digitalWrite(LedPin[i2],LOW);
    }
  }

  //Enable RGB LED with Magenta Colour and turn off all other Leds.
  delay(200);
  SubRGBMagenta();
  SubResetPage_1();

  }  

void loop() {
  
  Midireceive();
  
  for(int i = 0; i < numControls; i++) {
  
    NewBtnState[Page][i]=digitalRead(BtnPins[i]);
    NewBtnRGBState=digitalRead(BtnRGBPin);
    
    if (OldBtnState[Page][i] == 0 && NewBtnState[Page][i] ==1){
      if (LedState[Page][i] == 0) {
        // Button On
        if (debug == HIGH){
          Serial.print("button on --> ");
          Serial.println(i);          
        }
        SubOn(i);
      } else {   
        // Button Off
        if (debug == HIGH){
          Serial.print("button off --> ");
          Serial.println(i);          
        }
        Midi_msg_prep(i,0);
        digitalWrite(LedPin[i], LOW);
        LedState[Page][i] = 0;
        if ( Page == 2 && i == 0) {
          for(int i2 = 0; i2 < numControls; i2++) {
            if ( i2 != i) {
              digitalWrite(LedPin[i2],LOW);
              LedState[Page][i2] = 0;
            }
          }
        LedState[Page-1][1] = 0;
        }
        if ( Page == 1 && i == 1) {
          for(int i3 = 0; i3 < numControls; i3++) {
            LedState[Page+1][i3] = 0;
          }
        }
      }
    }
    
    OldBtnState[Page][i] = NewBtnState[Page][i];

    if (OldBtnRGBState == 0 && NewBtnRGBState ==1){
      SubsetRGB();
    }
    
    OldBtnRGBState = NewBtnRGBState;
  }
  delay(dt);
}


int invertColor(int color) {
  return (color * -1) +255;
}

// Btn/Leds Functions
// ==================

void SubOn(int btnind) {
  switch (Page) {
    case 0:  //* Magenta
      SubLedBlink(btnind);
      Midi_msg_prep(btnind,1);
      if (btnind == 3 || btnind == 4){
        SubResetPages();  
      }
      break;
    case 1: //* Aqua  
      LedState[Page][btnind] = 1;
      SubBtnOff(btnind);
      digitalWrite(LedPin[btnind], HIGH );          
      if (btnind == 1) {   
        LedState[2][0] = 1;
        LedState[2][4] = 1;
      }
      Midi_msg_prep(btnind,1);
      break;
    case 2: //* Orange
      if (btnind != 0 && LedState[Page][0] == 1){
          if (btnind == BtnTS) {
            digitalWrite(LedPin[BtnKOT],LOW);
            LedState[Page][BtnKOT] = 0;
          }
          if (btnind == BtnKOT) {
            digitalWrite(LedPin[BtnTS],LOW);
            LedState[Page][BtnTS] = 0;
          }
          digitalWrite(LedPin[btnind], HIGH );
          LedState[Page][btnind] = 1;
          SubBtnOff(btnind);
          Midi_msg_prep(btnind,1);
      } else {
        if (btnind == 0) {   
          // Activating Btn Page 2
          digitalWrite(LedPin[btnind], HIGH );
          LedState[Page][btnind] = 1;
          SubBtnOff(btnind);
          // Activating KoT
          digitalWrite(LedPin[BtnKOT],HIGH);
          LedState[Page][BtnKOT] = 1;
          //Activating Btn Page 1
          LedState[Page-1][btnind+1] = 1;
          
          Midi_msg_prep(btnind,1);
        }
      }
      break;
    default:
      break;
  }
}

void SubLedBlink(int btnind) {
  for(int i = 0; i < 4; i++) {
    delay(50);   
    digitalWrite(LedPin[btnind], HIGH);
    delay(50);
    digitalWrite(LedPin[btnind],LOW);
  } 
}

void SubResetPage_1() {
  for(int i = 0; i < numControls; i++) {
    digitalWrite(LedPin[i],LOW);
    LedState[0][i] = 0;
  }
}

void SubResetPages() {
  for(int ind_1 = 0; ind_1 < 3; ind_1++) {
    for(int ind_2 = 0; ind_2 < numControls; ind_2++) {
      LedState[ind_1][ind_2] = 0;
    }
  }
}

void SubLoadBtnState() {
  for(int i = 0; i < numControls; i++) {
    Serial.print("Loading State.: ");
    Serial.println(LedState[Page][i]);
    if ( LedState[Page][i] == 1) {
      digitalWrite(LedPin[i],HIGH);
    } else {
      digitalWrite(LedPin[i],LOW);
    }
  }
}

void SubLedsOff(int btnind, int page) {
  if (page == 0 ) {      
    for(int i = 0; i < numControls; i++) {
      if ( i != btnind) {
        digitalWrite(LedPin[i],LOW);
        LedState[Page][i] = 0;
      }
    }
  } 
}

void SubBtnOff(int btnind) {
  for(int i = 0; i < numControls; i++) {
    if ( i != btnind) {
      OldBtnState[Page][i] = 1;
    }    
  }
}


// RGB Buttons/Led Functions
// =========================

void SubsetRGB() {
  switch (Page) {
    case 0:  //* Magenta
      Page = 1;
      SubRGBLedsOff();
      SubRGBAqua(); 
      SubLoadBtnState();       
      break;
    case 1: //* Aqua
      Page = 2;
      SubRGBLedsOff();
      SubRGBOrange();
      SubLoadBtnState();
      break;
    case 2: //* Orange
      Page = 0;
      SubRGBLedsOff();
      SubRGBMagenta();
      SubResetPage_1();
      break;
    default:
      break;
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
  analogWrite(LedRGBPin[2],invertColor(255));  
}

void SubRGBAqua() {
  analogWrite(LedRGBPin[0],invertColor(0));
  analogWrite(LedRGBPin[1],invertColor(255));
  analogWrite(LedRGBPin[2],invertColor(80));  
}

void SubRGBOrange() {
  analogWrite(LedRGBPin[0],invertColor(255));
  analogWrite(LedRGBPin[1],invertColor(191));
  analogWrite(LedRGBPin[2],invertColor(0));  
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

void programChange(byte channel, byte program) {
  midiEventPacket_t pc = {0x0C, 0xC0 | channel, program, 0};
  MidiUSB.sendMIDI(pc);
}  


//* Receiving Midi Message
//  ====================== 

void Midireceive(){
  midiEventPacket_t rx;
    do {
      rx = MidiUSB.read();
      if (midisendmsg == 0) {
        if (rx.header != 0) {
          if (rx.byte1 == 0xB0) {
            int val_input1;
            memcpy(val_input1, rx.byte1, sizeof(rx.byte1));
            Serial.print("### Receiving on Channel..: ");
            Serial.print(val_input1);
            Serial.print("  ### CC Message..: ");
            Serial.print(rx.byte2);
            Serial.print("  ### CC Value..: ");
            Serial.println(rx.byte3);
  
            int ind_btn = rx.byte2 -1;
            
            if (rx.byte3 == 127) {
              LedState[1][ind_btn] = 1;
              if (Page == 1) {
                digitalWrite(LedPin[ind_btn],HIGH);
              }
            } else {
              LedState[1][ind_btn] = 0;
              if (Page == 1) {
                digitalWrite(LedPin[ind_btn],LOW);
              }  
            }      
          } else {
              Serial.print("Received: ");
              Serial.print(rx.header, HEX);
              Serial.print("-");
              Serial.print(rx.byte1, HEX);
              Serial.print("-");
              Serial.print(rx.byte2, HEX);
              Serial.print("-");
              Serial.println(rx.byte3, HEX);
          }
        }
      } 
    } while (rx.header != 0);
    midisendmsg = 0;   
}

//* Sending Midi Message
//  ==================== 
void Midi_msg_prep(int btn, int OffOn){
  
  switch (btn) {
    case 0:    
      memcpy(Val_msg, Val_msg_btn1[Page], sizeTab);
      break;
    case 1:
      memcpy(Val_msg, Val_msg_btn2[Page], sizeTab);
      break;
    case 2:
      memcpy(Val_msg, Val_msg_btn3[Page], sizeTab);
      break;
    case 3:
      memcpy(Val_msg, Val_msg_btn4[Page], sizeTab);
      break;
    case 4:
      memcpy(Val_msg, Val_msg_btn5[Page], sizeTab);
      break;
    default:
      break;
  }

  Serial.println("Sending Midi Message");
  
  for (int ind_msg = 0; ind_msg < 5; ind_msg++) {
    
    String Val_Str = Val_msg[ind_msg];

    if (Val_Str.substring(0,3).toInt() != 0) {
      // Substring (var (pos_ini , pos_fin)
      Midi_CH       = ((Val_Str.substring(0,3).toInt())-1);
      Midi_CC       = (Val_Str.substring(3,6).toInt());
      Midi_VaL_On   = (Val_Str.substring(6,9).toInt());
      Midi_VaL_Off = (Val_Str.substring(9,12).toInt());
      delay(10);
      if (OffOn == 1) {
   //     Serial.print("Midi On ");
   //     Serial.println(Midi_VaL_On);
        controlChange(Midi_CH, Midi_CC, Midi_VaL_On);
      } else {
   //     Serial.print("Midi Off ");
   //     Serial.println(Midi_VaL_Off);
        controlChange(Midi_CH, Midi_CC, Midi_VaL_Off);
      }
      Serial.println(Val_Str);
      MidiUSB.flush();
      midisendmsg = 1;
    }
  }
}    
