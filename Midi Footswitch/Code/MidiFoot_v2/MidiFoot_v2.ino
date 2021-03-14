#include "MIDIUSB.h"

const int numControls          = 5;
const int BtnPins[numControls] = {2,3,4,5,6};
const int LedPin[numControls]  = {A5,A4,A3,A2,A1};     // Blue , Green, Yellow, White, Red
const int LedRGBPin[3]         = {11,10,9};                   // Red, Green, Blue
const int BtnRGBPin            = 7;   

// Variables
int LedState[numControls]     = {0,0,0,0,0};
int LedRGBState               = 0;
int OldBtnState[numControls]  = {1,1,1,1,1};
int NewBtnState[numControls]  = {0,0,0,0,0};
int OldBtnRGBState = 1;
int NewBtnRGBState = 0;

int Pagina=0;
int MidiCH=0;
int MidiCC=50;
int MidiVaLOff=0;
int MidiVaLOn=127;
int RGBOn=255;
int RGBOff=0;
int dt=10;
int debug=LOW;


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

}

void loop() {
  for(int i = 0; i < numControls; i++) {
    delay(dt);
    NewBtnState[i]=digitalRead(BtnPins[i]);
    if (OldBtnState[i] == 0 && NewBtnState[i] ==1){
      
      if (LedState[i] == 0) {
        if (debug == HIGH){
          Serial.print("button on --> ");
          Serial.println(i);          
        }
        if (i == 0){
          controlChange(MidiCH, MidiCC, MidiVaLOn);
          MidiUSB.flush();
          Serial.println("Sending note on");
//          noteOn(1, 48, 64);   // Channel 0, middle C, normal velocity
//          MidiUSB.flush();        
        }
        
        digitalWrite(LedPin[i], HIGH );
        LedState[i] = 1;
        SubLedsOff(i);
        SubBtnOff(i);
      } else {
        if (i == 0){
          controlChange(MidiCH, MidiCC, MidiVaLOff);
          MidiUSB.flush();
        }
        digitalWrite(LedPin[i], LOW);
        LedState[i] = 0;
      }
    }
    if (debug == HIGH){
      
      Serial.println(OldBtnState[i]);
      Serial.println(NewBtnState[i]);
    }      
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
        digitalWrite(LedRGBPin[LedRGBState],invertColor(RGBOn));    // RGB Green On
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


// Subroutines
// ***********

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





int invertColor(int color) {
  return (color * -1) +255;
}
