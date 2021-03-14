
char Val_msg_btn1[3][5][13] = {
                                {{"001069000000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                {{"001001127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                {{"001002127000"}, {"002001127000"}, {"002004002000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                              };

char Val_msg_btn2[3][5][13] = {
                                {{"001069001000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                {{"001002127000"}, {"002001127000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                {{"002003001000"}, {"002004002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                              };
                              
char Val_msg_btn3[3][5][13] = {
                                {{"001069002000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                {{"001003127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                {{"002003001000"}, {"002004002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                              };


char Val_msg_btn4[3][5][13] = {
                                {{"001052127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                {{"001004127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                {{"002002127000"}, {"002005002000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                              };


char Val_msg_btn5[3][5][13] = {
                                {{"001052127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 0
                                {{"001005127000"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}, //Pag 1
                                {{"002004004002"}, {"000000000000"}, {"000000000000"},{"000000000000"},{"000000000000"}}  //Pag 2 
                              };

int Midi_Ch;
int Midi_CC;
int Midi_CC_val_on;
int Midi_CC_val_off;
/*
  int Midi_ar_ch[5];
  int Midi_ar_CC[5];
  int Midi_ar_CC_val_on[5];
  int Midi_ar_CC_val_off[5];
*/

void setup() {
// put your setup code here, to run once:
  delay(5000);
  Serial.begin(9600);
}

void loop() {

  long time_last = micros();  
  for (int i0 = 0; i0 < 3; i0++) {
    Serial.print(" #### Button 05 - Pagina ");       
    Serial.println(i0 + 1);       
    for (int i = 0; i < 5; i++) {
  
      //  Serial.print(sizeof(Val_msg_btn1[0]));
        String Val_Str = Val_msg_btn5[i0][i];
       if (Val_Str.substring(0,3).toInt() != 0) {
        // Substring (var (pos_ini , pos_fin)
//          Midi_ar_ch[i]   = (Val_Str.substring(0,3).toInt());
          Midi_Ch         = (Val_Str.substring(0,3).toInt());
          Midi_CC         = (Val_Str.substring(3,6).toInt());
          Midi_CC_val_on  = (Val_Str.substring(6,9).toInt());
          Midi_CC_val_off = (Val_Str.substring(9,12).toInt());
          Serial.print(" Message  ");       
          Serial.print(i + 1);
          Serial.print(" = ");       
          Serial.print(Val_Str);
          Serial.print(" -  Channel: "); 
    //      Serial.print(Midi_ar_ch[i]);
          Serial.print(Midi_Ch);
          Serial.print(" -  CC: "); 
          Serial.print(Midi_CC);
          Serial.print(" -  CC_VAL on: "); 
          Serial.print(Midi_CC_val_on);
          Serial.print(" -  CC_VAL off: "); 
          Serial.println(Midi_CC_val_off); 

       };
    } 
  }
    long time_now = micros();  
    float elap = time_now - time_last;
    Serial.println(elap);

    delay(5000);
 }
