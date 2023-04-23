#include "MIDIUSB.h"
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

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
// HX Stop Special Midi Messages
// -----------------------------
// MIDI message: CC 72 value 64-127 = next preset, value 0-63 = previous preset
// MIDI message: CC 69 value 0-2, 8 and 9 = Snapshot select (0=Snapshot 1, 1=Snapshot 2, 2=Snapshot 3, 8=Next snapshot, 9=Previous snapshot)
// FS1/2/3 emulation MIDI messages can now be momentary for HX Stomp (values 64-127 = press; values 0-63 = release) 

const char Val_msg_btn1[3][5][13] = {
                                      {{"001069000000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001001127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"002004004002"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };

const char Val_msg_btn2[3][5][13] = {
                                      {{"001069001001"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001002127000"}, {"002001127000"}, {"002004002000"},{"002002000000"},{"002003002000"}}, //Pag 1
                                      {{"001002127000"}, {"002001127000"}, {"002004002000"},{"002002000000"},{"002003002000"}}  //Pag 2 
                                    };
                                
const char Val_msg_btn3[3][5][13] = {
                                      {{"001069002002"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001003127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"002002127000"}, {"002005002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };


const char Val_msg_btn4[3][5][13] = {
                                      {{"001072063063"}, {"001069000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001004127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"002003001003"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };
  
const char Val_msg_btn5[3][5][13] = {
                                      {{"001072127127"}, {"001069000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                      {{"001005127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                      {{"002003002004"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                                    };
                                
// Buttons / Led Definitions / Table Size
const int8_t  numControls          = 5;
const int8_t  BtnPins[numControls] = {2,3,4,5,7};
const int8_t  LedPin[numControls]  = {A5,A4,A3,A2,A1};     // Blue , Green, Yellow, White, Red
const int8_t  LedRGBPin[3]         = {11,10,9};            // Red, Green, Blue
const int8_t  BtnRGBPin            = 12;
const int8_t  sizeTab              = 65;


// Variables
int8_t        LedState[3][5]       = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; // Off using Normal Logic
int8_t        BtnState[3][5]       = {{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}}; // Off using Inverted Logic
int8_t        NewBtnState[3][5]    = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; // On  using Inverted Logic
int8_t        GBclip[2]            = {1,1};
int8_t        LastBtnDrv           = 4;
int8_t        MidiOnOff            = 0;
int8_t        ValOn                = 1;
int8_t        ValOff               = 0;
int8_t        Page                 = 0;
int8_t        midisendmsg          = 1;
int8_t        BtnRGBState          = 1; // Off using Inverted Logic
int8_t        NewBtnRGBState       = 0; // On  using Inverted Logic
int8_t        BtnTS                = 3;
int8_t        BtnKOT               = 4;
int8_t        dt                   = 20;
int8_t        debug                = LOW;
int8_t        usb_flag             = 0;
unsigned long time_now             = 0;
int           period               = 50;

//Midi Auxiliar Array
char Val_msg[5][13];

// Variables for Midi Message
int Midi_CH;
int Midi_CC;
int Midi_VaL_On;
int Midi_VaL_Off;
byte pc_channel_ant;
byte pc_number_ant;



void setup() {
  //start serial connection
  //Serial.begin(9600);
  Serial.begin(115200);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
//  MIDI.begin(1, 2);
//  MIDI.begin(1);
  MIDI.turnThruOff();

  MIDI.setHandleControlChange(MyCCFunction); // This command tells the MIDI Library
                                              // the function you want to call when a Continuous Controller command is received
  
  MIDI.setHandleProgramChange(MyPCFunction); // This command tells the MIDI Library
                                             // the function you want to call when a Program change command is received.

  
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
    digitalWrite(LedRGBPin[i],invertColor(0));
  }
    
  for(int i2 = 0; i2 < numControls; i2++) {
    for(int i3 = 0; i3 < 3; i3++) {
      delay(50);   
      digitalWrite(LedPin[i2], HIGH);
      delay(50);
      digitalWrite(LedPin[i2],LOW);
    }
  }

  delay(1000);

  //Enable RGB LED with Magenta Colour and turn off all other Leds.
  SubRGBMagenta();
  SubSnap_1();
  Serial.println("Cremoso Midi Started....");          
}  
  
void loop() {
  
  Midireceive();

  if(millis() > time_now + period){
    time_now = millis();
//    Serial.print("Time now --> ");
//    Serial.println(time_now);          
      
    for(int i = 0; i < numControls; i++) {
      NewBtnState[Page][i]=digitalRead(BtnPins[i]);
      NewBtnRGBState=digitalRead(BtnRGBPin);
  
      if (BtnState[Page][i] == 1 && NewBtnState[Page][i] ==0){
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
          SubOff(i);
        } // Else
      }
      
      if (BtnRGBState == 0 && NewBtnRGBState == 1){
        SubsetRGB();
      }
      
      BtnState[Page][i] = NewBtnState[Page][i];
      
      BtnRGBState = NewBtnRGBState;
    }
  }    
}

int invertColor(int color) {
  return (color * -1) +255;
}

// Btn/Leds Functions
// ==================

void SubOn(int btnind) {

  switch (Page) {
    case 0:  //* Magenta
      send_midi_msg(btnind,ValOn);
      if (btnind == 3 || btnind == 4){     //* Program UP - Program Down
        SubLedBlink(btnind);
        SubResetAllPages();
        SubLoadBtnState();  
        //  Turn on Led on Snapshot 1
        LedState[0][0] = 1;
        BtnState[0][0] = 1;
        digitalWrite(LedPin[0],HIGH);
        // SubSnap_1();
      } else {
        SubBtnOff(btnind);
        SubLedsOff(btnind);
        digitalWrite(LedPin[btnind], HIGH );
        LedState[Page][btnind] = 1;
      }
      break;
    case 1: //* Aqua  
      LedState[Page][btnind] = 1;
      SubBtnOff(btnind);
      digitalWrite(LedPin[btnind], HIGH );          
      if (btnind == 1) {   
        LedState[2][1] = 1;
        LedState[2][4] = 1;
      }
      send_midi_msg(btnind,ValOn);
      break;
    case 2: //* Orange
      if (btnind != 1 && LedState[Page][1] == 1){
          if (btnind == BtnTS) {
            digitalWrite(LedPin[BtnKOT],LOW);
            MidiOnOff = GBclip[0];
            LedState[Page][BtnKOT] = 0;
            if (LastBtnDrv == 3) {
              SubDrvBtn(btnind);
            } else {
              LastBtnDrv = 3;
            }
          }
          if (btnind == BtnKOT) {
            digitalWrite(LedPin[BtnTS],LOW);
            MidiOnOff = GBclip[1];
            LedState[Page][BtnTS] = 0;
            if (LastBtnDrv == 4) {
              SubDrvBtn(btnind);
            }else {
              LastBtnDrv = 4;
            }
          }
          digitalWrite(LedPin[btnind], HIGH );
          LedState[Page][btnind] = 1;
          if (btnind == 3 || btnind == 4){
            send_midi_msg(btnind,MidiOnOff);
          } else {
            send_midi_msg(btnind,ValOn);
          }
            
      } else {
        if (btnind == 1) {   
          // Activating Btn Page 2
          digitalWrite(LedPin[btnind], HIGH );
          LedState[Page][btnind] = 1;
          SubBtnOff(btnind);
          // Activating KoT
          LastBtnDrv = 4;
          digitalWrite(LedPin[BtnKOT],HIGH);
          LedState[Page][BtnKOT] = 1;
          //Activating Btn Page 1
          LedState[Page-1][btnind] = 1;
          send_midi_msg(btnind,ValOn);
        }
      }
      break;
    default:
      break;
  }
}

//*-------------------------------------------------------
void SubOff(int btnind) {

  switch (Page) {
    case 0:  //* Magenta
      LedState[Page][btnind] = 1;
      break;      
    case 1:  //* Magenta
      if (btnind == 1) {
        for(int i3 = 0; i3 < numControls; i3++) {
          LedState[Page+1][i3] = 0;
        }
      }
      send_midi_msg(btnind,0);
      digitalWrite(LedPin[btnind], LOW);
      LedState[Page][btnind] = 0;
      break;
    case 2:  //* Magenta
      if (btnind == 1) {
        for(int i2 = 0; i2 < numControls; i2++) {
          if ( i2 != btnind) {
            digitalWrite(LedPin[i2],LOW);
            LedState[Page][i2] = 0;
          }
        }
      } 
      if (btnind == 3 || btnind == 4){
        MidiOnOff = 0;
        if (btnind == 3) {
            MidiOnOff = GBclip[0];
            if (LastBtnDrv == btnind) {
              SubDrvBtn(btnind);
            } else {
              LastBtnDrv = 3;
            }
        } else {
            MidiOnOff = GBclip[1];
            if (LastBtnDrv == btnind) {
              SubDrvBtn(btnind);
            } else {
              LastBtnDrv = 4;
            }
        }
        LedState[Page][btnind] = 0;
        send_midi_msg(btnind,MidiOnOff);
      } else {
        send_midi_msg(btnind,ValOff);
        digitalWrite(LedPin[btnind], LOW);
        LedState[Page][btnind] = 0;
      }
      break;   
    default:
      break;
  }
}

//*-------------------------------------------------------
void SubLedBlink(int btnind) {
  for(int i = 0; i < 4; i++) {
    delay(50);   
    digitalWrite(LedPin[btnind], HIGH);
    delay(50);
    digitalWrite(LedPin[btnind],LOW);
  } 
}

//*-------------------------------------------------------
void SubResetAllPages() {
  for(int ind_1 = 0; ind_1 < 3; ind_1++) {
    for(int ind_2 = 0; ind_2 < numControls; ind_2++) {
      LedState[ind_1][ind_2] = 0;
    }
  }
}

//*-------------------------------------------------------
void SubLoadBtnState() {
  for(int i = 0; i < numControls; i++) {
    if ( LedState[Page][i] == 1) {
      digitalWrite(LedPin[i],HIGH);
    } else {
      digitalWrite(LedPin[i],LOW);
    }
  }
}

//*-------------------------------------------------------
void SubLedsOff(int btnind) {
  if (Page == 0 ) {      
    for(int i = 0; i < 3; i++) {
      if ( i != btnind) {
        digitalWrite(LedPin[i],LOW);
        LedState[Page][i] = 0;
      }
    }
  } 
}

//*-------------------------------------------------------
void SubBtnOff(int btnind) {
  for(int i = 0; i < numControls; i++) {
    if ( i != btnind) {
      BtnState[Page][i] = 1;
    }    
  }
}

//*-------------------------------------------------------
void SubSnap_1() {

  Page = 0;

  // Resetting Page
  for(int i = 0; i < numControls; i++) {
    digitalWrite(LedPin[i],LOW);
    LedState[0][i] = 0;
  }

  //  Snapshot 1
  LedState[0][0] = 1;
  BtnState[0][0] = 1;
  digitalWrite(LedPin[0],HIGH);
  send_midi_msg(0,1);
  
}

//*-------------------------------------------------------
void SubDrvBtn(int btn) {
  if (btn == 4) {
    if (GBclip[1] == 0) {
      GBclip[1] = 1;
    } else {
      GBclip[1] = 0;
    }
  MidiOnOff = GBclip[1];
  } else {
    if (GBclip[0] == 0) {
      GBclip[0] = 1;
    } else {
      GBclip[0] = 0;
    }
  MidiOnOff = GBclip[0];
  }
}


// RGB Buttons/Led Functions
// =========================

void SubsetRGB() {
  switch (Page) {
    case 0:  //* Magenta
      Page = 1;
      SubRGBAqua(); 
      break;
    case 1: //* Aqua
      Page = 2;
      SubRGBOrange();
      break;
    case 2: //* Orange
      Page = 0;
      SubRGBMagenta();
      break;
    default:
      break;
    }
    SubLoadBtnState();
}

//*-------------------------------------------------------
void SubRGBMagenta() {
  analogWrite(LedRGBPin[0],invertColor(255));
  analogWrite(LedRGBPin[1],invertColor(0));
  analogWrite(LedRGBPin[2],invertColor(255));  
}

//*-------------------------------------------------------
void SubRGBAqua() {
  analogWrite(LedRGBPin[0],invertColor(0));
  analogWrite(LedRGBPin[1],invertColor(255));
  analogWrite(LedRGBPin[2],invertColor(80));  
}

//*-------------------------------------------------------
void SubRGBOrange() {
  analogWrite(LedRGBPin[0],invertColor(255));
  analogWrite(LedRGBPin[1],invertColor(191));
  analogWrite(LedRGBPin[2],invertColor(0));  
}


//* Midi Library Funcions 
// ======================

//*------Function not being used at the moment. -------------------
void programChange(byte channel, byte program) {
  midiEventPacket_t pc = {0x0C, 0xC0 | channel, program, 0};
  MidiUSB.sendMIDI(pc);
}  

//*----------------------------------------------------------------
void MyPCFunction(byte pc_channel, byte pc_number) {
  Serial.println("MyPCFunction - Program Changed..:");  
  Serial.print(pc_channel);
  Serial.println(pc_number);
  if (pc_channel == pc_channel_ant && pc_number == pc_number_ant) {
    Serial.println("Same Program change message I sent before");
    } else {
      Serial.println("Sending PC change");
      MIDI.sendProgramChange(pc_number,pc_channel+2);
      pc_channel_ant = pc_channel;
      pc_number_ant = pc_number;
  }
  SubRGBMagenta();
  SubResetAllPages();
  delay(50);
  SubSnap_1();
}

//*----------------------------------------------------------------
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

//*----------------------------------------------------------------
void MyCCFunction(byte cc_channel, byte cc_number, byte cc_value) {
Serial.println("MyCCFunction - ### Received CC Message..: ");  
  
  Serial.println("### Received CC Message..: ");
  Serial.print("Channel.: ");
  Serial.print(cc_channel);
  Serial.print(" # CC Msg.: ");
  Serial.print(cc_number);
  Serial.print(" # CC Value.: ");
  Serial.print(cc_value);
  Serial.println();
  
  if (cc_channel == 1) {     // HX Stomp
    if (Page == 0) {
      int ind_btn = cc_value; 
      Serial.println(ind_btn);
      SubBtnOff(ind_btn);
      SubLedsOff(ind_btn);
      digitalWrite(LedPin[ind_btn], HIGH );
      LedState[Page][ind_btn] = 1;
    }

  } else {
    if (cc_channel == 2) {     // Golden Boy
      SubLoadGB(cc_number, cc_value);  
    }
  }
}

//* Receiving Midi Message
//  ====================== 

void Midireceive(){
 MIDI.read();                    // If we have received a message
/* {
    Serial.print(" Received Midi Message on ");
    Serial.print(" ## Channel : ");
    Serial.println(MIDI.getChannel());
    Serial.print(" ## Type : ");
    Serial.print(MIDI.getType());
    Serial.print(" ## Data1 : ");
    Serial.print(MIDI.getData1());
    Serial.print(" ## Data2 : ");
    Serial.print(MIDI.getData2());
    Serial.println();
 }
*/
  if (usb_flag == 1) {
    midiEventPacket_t rx;
    do {
      rx = MidiUSB.read();
      if (midisendmsg == 0) {
        if (rx.header != 0) {
          Serial.print("### Receiving on Channel..: ");
          // Message Channel 0
          if ((rx.byte1 == 0xB0) && (rx.byte2 != 0)) {    
            int val_input1;
            memcpy(val_input1, rx.byte1, sizeof(rx.byte1));
  
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
            // Program change
            if (rx.byte1 == 0xC0) {
              SubResetAllPages();  
              SubSnap_1();
            } else {
              if ((rx.byte1 == 0xB1) && (rx.byte2 != 0)) { 
                int val_input1;
                memcpy(val_input1, rx.byte1, sizeof(rx.byte1));
                SubLoadGB(rx.byte2, rx.byte3);
              } 
            }
          }
        }
      }
      midisendmsg = 0;  
    } while (rx.header != 0);
  }
}


//* Loading Golden Boy Midi Messages
//  ================================
void SubLoadGB(int CC_midi, int CC_msg){

  Serial.println("Receiving message for Golden Boy");
  Serial.print(CC_midi);
  Serial.print(" - ");
  Serial.print(CC_msg);
  Serial.println();
  Serial.println(Page);
Serial.println(midisendmsg);
  
  switch (CC_midi) {
    case 1:  //* Golden Boy ON/OFF
      if (CC_msg == 127) {
          LedState[1][1] = 1;
          LedState[2][1] = 1;
          LedState[2][4] = 1;
          switch (Page) {
            case 1:        
              digitalWrite(LedPin[1],HIGH);
              break;
            case 2:        
              digitalWrite(LedPin[1],HIGH);
              digitalWrite(LedPin[4],HIGH);
              break;
            default:
              break;
          }

      } else {
        if (CC_msg == 0) {
          LedState[1][1] = 0;
          for(int i = 0; i < numControls; i++) {
            LedState[2][i] = 0;
          }    
          switch (Page) {
            case 1:        
              digitalWrite(LedPin[1],LOW);
              break;
            case 2:        
              for(int i = 0; i < numControls; i++) {
                LedState[2][1] = 0;
                digitalWrite(LedPin[i],LOW);
              }    
              break;
            default:
              break;
          }
        }
      }
      break;
    case 2: //* Boost ON/OFF
      if (CC_msg == 127) {
          LedState[2][2] = 1;
          if (Page == 2) {
            digitalWrite(LedPin[2],HIGH);
          }
      } else {
        if (CC_msg == 0) {
          LedState[2][2] = 0;
          if (Page == 2) {
            digitalWrite(LedPin[2],LOW);
          }
        }
      }
      break;
    case 3: //* Type of Drive
      if (CC_msg == 1) {   // 
          LedState[2][3] = 1;
          if (Page == 2) {
            digitalWrite(LedPin[3],HIGH);
          }
      } else {
        if (CC_msg == 2) {
          LedState[2][4] = 1;
          if (Page == 2) {
            digitalWrite(LedPin[4],HIGH);
          }
        }
      }
      break;
    case 4: //* Gain          
      if (CC_msg == 2) {   // 
          LedState[2][0] = 0;
          if (Page == 2) {
            digitalWrite(LedPin[0],LOW);
          }
      } else {
        if (CC_msg == 4) {
          LedState[2][0] = 1;
          if (Page == 2) {
            digitalWrite(LedPin[0],HIGH);
          }
        }
      }
      break;
    default:
      break;
    }
}    

//* Sending Midi Message
//  ==================== 
void send_midi_msg(int btn, int OffOn){

  Serial.print("Sending Midi Message.. ");
  Serial.print(btn);
  Serial.print("OffOn..: ");
  Serial.println(OffOn);
  
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
  
  for (int ind_msg = 0; ind_msg < 5; ind_msg++) {
    String Val_Str = Val_msg[ind_msg];

    if (Val_Str.substring(0,3).toInt() != 0) {
      // Substring (var (pos_ini , pos_fin)
      Midi_CH      = ((Val_Str.substring(0,3).toInt())-1);
      Midi_CC      =  (Val_Str.substring(3,6).toInt());
      Midi_VaL_On  =  (Val_Str.substring(6,9).toInt());
      Midi_VaL_Off =  (Val_Str.substring(9,12).toInt());
      if (OffOn == 1) {
        controlChange(Midi_CH, Midi_CC, Midi_VaL_On);
        MIDI.sendControlChange(Midi_CC, Midi_VaL_On, Midi_CH+1);
      } else {
        controlChange(Midi_CH, Midi_CC, Midi_VaL_Off);
        MIDI.sendControlChange(Midi_CC, Midi_VaL_Off, Midi_CH+1);
      }
      Serial.print("Midi_Tab_String -> ");
      Serial.println(Val_Str);
      
      
      MidiUSB.flush();
      midisendmsg = 1;
      } else {
        break;  
    }
  }
}    

//*-------------------------------------------------------
int StrToHex(char str[])
{
  return (int) strtol(str, 0, 16);
}
