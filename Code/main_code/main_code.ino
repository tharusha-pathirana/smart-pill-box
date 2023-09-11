

#include <DS3231.h>
#include <Wire.h>  // i2C Conection Library
#include <LiquidCrystal_I2C.h> 
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <LowPower.h>


LiquidCrystal_I2C lcd(0x27,16,2); 

#define bt_time   8         //A0
#define bt_up     7         //A1
#define bt_down   6         //A2
#define bt_alarm  5         //A3

#define buzzer 2

#define ledA A3
#define ledB A2
#define ledC A1
#define ledD 12
#define ledE 11

#define rxPin  10      //for arduino. for bluetoothmodule it is tx
#define txPin  9

SoftwareSerial BTSerial(rxPin, txPin);


DS3231  rtc(SDA, SCL);

Time  t; 

byte hh = 0, mm = 0, ss = 0, dd = 0, bb = 0, set_day;
int yy = 0;
String Day = "  ";

byte AlarmHH  = 21, AlarmMM  = 22, AlarmSS  = 23, setMode = 0, setAlarm = 0, alarmMode=1;
byte Alarm2HH  = 21, Alarm2MM  = 22, Alarm2SS  = 23 ;
byte Alarm3HH  = 21, Alarm3MM  = 22, Alarm3SS  = 23;
//
byte A_1=0, B_1=0, C_1=0, D_1=0, E_1=0 ;
byte A_2=0, B_2=0, C_2=0, D_2=0, E_2=0 ;
byte A_3=0, B_3=0, C_3=0, D_3=0, E_3=0 ;

byte alarm1_counter=0, alarm2_counter=0, alarm3_counter=0 ;

int stop =0, mode=0,mode2=0,mode3=0, flag=0;

int n=75, m=40 ;                 // (n = alarm ringing time in mili seconds / 400)  , (m = time allowed for confirm medicine taken / 250)

//Eeprom Store Variable
uint8_t HH;
uint8_t MM;

 byte bell_symbol[8] = {
        B00100,
        B01110,
        B01110,
        B01110,
        B01110,
        B11111,
        B01000,
        B00100};
byte thermometer_symbol[8] = {
        B00100,
        B01010,
        B01010,
        B01110,
        B01110,
        B11111,
        B11111,
        B01110};

const char START_TOKEN = '?';
const char END_TOKEN = ';';
const int CHAR_TIMEOUT = 20;

bool waitingForStartToken = true;
String messageBuffer = "";


void setup(){
// Setup Serial connection
  Serial.begin(9600);
  BTSerial.begin(9600);

  Serial.println("welcome");
  

  rtc.begin(); 

 pinMode(bt_time,  INPUT);
 pinMode(bt_up,    INPUT);
 pinMode(bt_down,  INPUT);
 pinMode(bt_alarm, INPUT);



 pinMode(buzzer, OUTPUT);

 pinMode(ledA, OUTPUT);
 pinMode(ledB, OUTPUT);
 pinMode(ledC, OUTPUT);
 pinMode(ledD, OUTPUT);
 pinMode(ledE, OUTPUT);

 digitalWrite(ledA, LOW);
 digitalWrite(ledB, LOW);
 digitalWrite(ledC, LOW);
 digitalWrite(ledD, LOW);
 digitalWrite(ledE, LOW);

 pinMode(rxPin, INPUT);
 pinMode(txPin, OUTPUT);
 

  lcd.init();
  lcd.backlight();

  lcd.createChar(1, thermometer_symbol);
  lcd.createChar(2, bell_symbol);
  
  //lcd.begin(16, 2); 
  //lcd.init();
 

  lcd.setCursor (0,0);
  lcd.print(" Smart Pill Box");
  
  
  delay (2000);
  lcd.clear();

//EEPROM.write(50,1);

stop=EEPROM.read(50);
if(stop==0){  
}else{WriteEeprom ();}

//EEPROM.write(50,0);

//EEPROM.write(71,75);
//
//EEPROM.write(72,40);


ReadEeprom();

n = EEPROM.read(71);
m = EEPROM.read(72);



//rtc.setDOW(2);     
//rtc.setTime (00, 9, 50); 
//rtc.setDate(22, 04, 2023);  
}

void loop(){  
  
  
t = rtc.getTime();
Day = rtc.getDOWStr(1);                         // day of the week in abbreviated form - 3 letters (if argument is 2 - day of week fully)

if (setMode == 0){
hh = t.hour,DEC;
mm = t.min,DEC;
ss = t.sec,DEC;
dd = t.date,DEC;
bb = t.mon,DEC;
yy = t.year,DEC;
}  

  if(setAlarm==0){                                   // first screen (setAlarm = {1,2,3,4} means 2nd screen(alarm screen) {Activated,hour,minute,seconds} 
  lcd.setCursor(0,0);                               // additionally, setMode means pointer. In 1st screen, setMode goes from times to days
  
  lcd.print((hh/10)%10);
  lcd.print(hh % 10); 
   
  lcd.print(":");
  
  lcd.print((mm/10)%10);
  lcd.print(mm % 10);
  
  lcd.print(":");
  
  lcd.print((ss/10)%10);
  lcd.print(ss % 10);
  
  
  lcd.print(" ");  
  if((mode==1)||(mode2==1)||(mode3==1)){lcd.write(2);}                                                  // if alarm set- show bell icon  (mode=1 means alarm 1 set)
  else{lcd.print(" ");}                                                       // if not show blank space

  
  lcd.print(" "); 
  lcd.write(1);                                                              // thermometer icon
  lcd.print(rtc.getTemp(),0);                                               // temperature rounded off to integer
  lcd.write(223);                                                           // degree icon
  lcd.print("C");
  lcd.print("  "); 

  lcd.setCursor(1,1);
  lcd.print(Day);
  lcd.print(" ");
  lcd.print((dd/10)%10);
  lcd.print(dd % 10); 
  lcd.print("/");
  lcd.print((bb/10)%10);
  lcd.print(bb % 10);
  lcd.print("/"); 
  lcd.print((yy/1000)%10);
  lcd.print((yy/100)%10);
  lcd.print((yy/10)%10);
  lcd.print(yy % 10);
  }


if (!BTSerial.available()&& setMode==0 && setAlarm==0){
  LowPower.idle(SLEEP_500MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_OFF);

  LowPower.idle(SLEEP_250MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_OFF);
}

//Serial.println(set_day);
setupClock();
setTimer();
delay (100);
blinking();

//Alarm
if (alarmMode==1 && mode==1 && hh==AlarmHH && mm==AlarmMM && ss>=AlarmSS && ss<AlarmSS+5 && setMode==0) {         // checking if alarm time and real time are same

  lcd.backlight();

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Time for your");
  lcd.setCursor(3,1);
  lcd.print("Medicine");
  int i;     // for 30 seconds
  
  for (i = 0; i<=n; i++){
//        digitalWrite(buzzer, HIGH);
//        delay (300);
//        digitalWrite(buzzer, LOW);
//        delay (100);

         tone(buzzer, 1000);
          delay(300);     
          noTone(buzzer);     
          delay(100); 
        if(digitalRead (bt_alarm) == 0){
          break;
          
        }
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A-");
      lcd.print(A_1);
      lcd.print(" B-");
      lcd.print(B_1);
      lcd.print(" C-");
      lcd.print(C_1);
      lcd.print("   ");

      lcd.setCursor(0,1);
      lcd.print("D-");
      lcd.print(D_1);
      lcd.print(" E-");
      lcd.print(E_1);

      if (A_1 != 0){ digitalWrite(ledA, HIGH); }
      if (B_1 != 0){ digitalWrite(ledB, HIGH); }
      if (C_1 != 0){ digitalWrite(ledC, HIGH); }
      if (D_1 != 0){ digitalWrite(ledD, HIGH); }
      if (E_1 != 0){ digitalWrite(ledE, HIGH); }
      

      delay(3000);

      int med_confirm=0;

      for (i=0; i<m; i++){           //10 seconds for now. change it to 1 minute
        delay(250);
        if(digitalRead (bt_alarm) == 0){
          med_confirm=1;
          break;
        }
      }

 digitalWrite(ledA, LOW);
 digitalWrite(ledB, LOW);
 digitalWrite(ledC, LOW);
 digitalWrite(ledD, LOW);
 digitalWrite(ledE, LOW);

      lcd.clear();
      lcd.setCursor(0,0);
      
      if(med_confirm==0){
        
        lcd.print("Missed Medcine");
        
        BTSerial.println("You have missed your medicine! Please take it as soon as possible");
        BTSerial.print("A-");
        BTSerial.print(A_1);
        BTSerial.print(" B-");
        BTSerial.print(B_1);
        BTSerial.print(" C-");
        BTSerial.print(C_1);
        BTSerial.print(" ");
  
       
        BTSerial.print("D-");
        BTSerial.print(D_1);
        BTSerial.print(" E-");
        BTSerial.println(E_1);
        delay(3000);
      }

      else{
        lcd.print("Stay Healthy");
        delay(3000);
      }

      lcd.noBacklight();
    
      //delay(5000);  
}

if (alarmMode==1 && mode2==1 && hh==Alarm2HH && mm==Alarm2MM && ss>=Alarm2SS && ss<Alarm2SS+5 && setMode==0) {         // checking if alarm time and real time are same

  lcd.backlight();

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Time for your");
  lcd.setCursor(3,1);
  lcd.print("Medicine");
  int i;     // for 30 seconds
  
  for (i = 0; i<=n; i++){
         tone(buzzer, 1000);
          delay(300);     
          noTone(buzzer);     
          delay(100); 
        if(digitalRead (bt_alarm) == 0){
          break;
        }
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A-");
      lcd.print(A_2);
      lcd.print(" B-");
      lcd.print(B_2);
      lcd.print(" C-");
      lcd.print(C_2);
      lcd.print("   ");

      lcd.setCursor(0,1);
      lcd.print("D-");
      lcd.print(D_2);
      lcd.print(" E-");
      lcd.print(E_2);

      if (A_2 != 0){ digitalWrite(ledA, HIGH); }
      if (B_2 != 0){ digitalWrite(ledB, HIGH); }
      if (C_2 != 0){ digitalWrite(ledC, HIGH); }
      if (D_2 != 0){ digitalWrite(ledD, HIGH); }
      if (E_2 != 0){ digitalWrite(ledE, HIGH); }

      delay(3000);

      int med_confirm=0;

      for (i=0; i<m; i++){           //10 seconds for now. change it to 1 minute
        delay(250);
        if(digitalRead (bt_alarm) == 0){
          med_confirm=1;
          break;
        }
      }

 digitalWrite(ledA, LOW);
 digitalWrite(ledB, LOW);
 digitalWrite(ledC, LOW);
 digitalWrite(ledD, LOW);
 digitalWrite(ledE, LOW);

      lcd.clear();
      lcd.setCursor(0,0);
      
      if(med_confirm==0){
        
        lcd.print("Missed Medcine");

        BTSerial.println("You have missed your medicine! Please take it as soon as possible");
        BTSerial.print("A-");
        BTSerial.print(A_2);
        BTSerial.print(" B-");
        BTSerial.print(B_2);
        BTSerial.print(" C-");
        BTSerial.print(C_2);
        BTSerial.print(" ");
  
       
        BTSerial.print("D-");
        BTSerial.print(D_2);
        BTSerial.print(" E-");
        BTSerial.println(E_2);

        delay(3000);
      }

      else{
        lcd.print("Stay Healthy");
        delay(3000);
      }

      lcd.noBacklight();
    
      //delay(5000);  
}

if (alarmMode==1 && mode3==1 && hh==Alarm3HH && mm==Alarm3MM && ss>=Alarm3SS && ss<Alarm3SS+5 && setMode==0) {         // checking if alarm time and real time are same

  lcd.backlight();

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Time for your");
  lcd.setCursor(3,1);
  lcd.print("Medicine");
  int i;     // for 30 seconds
  
  for (i = 0; i<=n; i++){
         tone(buzzer, 1000);
          delay(300);     
          noTone(buzzer);     
          delay(100); 
        if(digitalRead (bt_alarm) == 0){
          break;
        }
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A-");
      lcd.print(A_3);
      lcd.print(" B-");
      lcd.print(B_3);
      lcd.print(" C-");
      lcd.print(C_3);
      lcd.print("   ");

      lcd.setCursor(0,1);
      lcd.print("D-");
      lcd.print(D_3);
      lcd.print(" E-");
      lcd.print(E_3);

      if (A_3 != 0){ digitalWrite(ledA, HIGH); }
      if (B_3 != 0){ digitalWrite(ledB, HIGH); }
      if (C_3 != 0){ digitalWrite(ledC, HIGH); }
      if (D_3 != 0){ digitalWrite(ledD, HIGH); }
      if (E_3 != 0){ digitalWrite(ledE, HIGH); }

      delay(3000);

      int med_confirm=0;

      for (i=0; i<m; i++){           //10 seconds for now. change it to 1 minute
        delay(250);
        if(digitalRead (bt_alarm) == 0){
          med_confirm=1;
          break;
        }
      }

 digitalWrite(ledA, LOW);
 digitalWrite(ledB, LOW);
 digitalWrite(ledC, LOW);
 digitalWrite(ledD, LOW);
 digitalWrite(ledE, LOW);
 
      lcd.clear();
      lcd.setCursor(0,0);
      
      if(med_confirm==0){
        
        lcd.print("Missed Medcine");
        
        BTSerial.println("You have missed your medicine! Please take it as soon as possible");
        BTSerial.print("A-");
        BTSerial.print(A_3);
        BTSerial.print(" B-");
        BTSerial.print(B_3);
        BTSerial.print(" C-");
        BTSerial.print(C_3);
        BTSerial.print(" ");
  
       
        BTSerial.print("D-");
        BTSerial.print(D_3);
        BTSerial.print(" E-");
        BTSerial.println(E_3);
        
        delay(3000);
      }

      else{
        lcd.print("Stay Healthy");
        delay(3000);
      }
    
      //delay(5000);

      lcd.noBacklight();
        
}



readBTData();





delay(100);

}

// end of void loop

void blinking (){
//BLINKING SCREEN
if (setAlarm <2 && setMode == 1){lcd.setCursor(0,0);  lcd.print("  ");}                        // blink hour
if (setAlarm <2 && setMode == 2){lcd.setCursor(3,0);  lcd.print("  ");}                        // blink minute 
if (setAlarm <2 && setMode == 3){lcd.setCursor(6,0);  lcd.print("  ");}                       // blink seconds
if (setAlarm <2 && setMode == 4){lcd.setCursor(1,1);  lcd.print("   ");}                      // blink day of week
if (setAlarm <2 && setMode == 5){lcd.setCursor(5,1);  lcd.print("  ");}                       // blink day
if (setAlarm <2 && setMode == 6){lcd.setCursor(8,1);  lcd.print("  ");}                       // blink month
if (setAlarm <2 && setMode == 7){lcd.setCursor(11,1); lcd.print("    "); }                    // blink year



if ((setMode==0)&&(setAlarm>0)){
  if ((alarm1_counter==0)&& (alarm2_counter==0)&& (alarm3_counter==0)){
    if(setAlarm==1){ lcd.setCursor(0,1);  lcd.print("  "); }
    if(setAlarm==2){ lcd.setCursor(4,1);  lcd.print("  "); }
    if(setAlarm==3){ lcd.setCursor(7,1);  lcd.print("  "); }
  }

  if ((alarm1_counter==1)||(alarm2_counter==1)||(alarm3_counter==1)){ lcd.setCursor(7,0);  lcd.print("     "); }
  if ((alarm1_counter==2)||(alarm2_counter==2)||(alarm3_counter==2)){ lcd.setCursor(4,1);  lcd.print("  "); }
  if ((alarm1_counter==3)||(alarm2_counter==3)||(alarm3_counter==3)){ lcd.setCursor(7,1);  lcd.print("  "); }
  if ((alarm1_counter==4)||(alarm2_counter==4)||(alarm3_counter==4)){ lcd.setCursor(10,1);  lcd.print("  "); }

  if ((alarm1_counter==5)||(alarm2_counter==5)||(alarm3_counter==5)){ lcd.setCursor(2,0);  lcd.print(" "); }
  if ((alarm1_counter==6)||(alarm2_counter==6)||(alarm3_counter==6)){ lcd.setCursor(6,0);  lcd.print(" "); }
  if ((alarm1_counter==7)||(alarm2_counter==7)||(alarm3_counter==7)){ lcd.setCursor(10,0);  lcd.print(" "); }
  if ((alarm1_counter==8)||(alarm2_counter==8)||(alarm3_counter==8)){ lcd.setCursor(2,1);  lcd.print(" "); }
  if ((alarm1_counter==9)||(alarm2_counter==9)||(alarm3_counter==9)){ lcd.setCursor(6,1);  lcd.print(" "); }
  if ((alarm1_counter==10)||(alarm2_counter==10)||(alarm3_counter==10)){ lcd.setCursor(10,1);  lcd.print(" "); }
  
}





}


void setupClock (void) {
   
    if (setMode == 8){                                          // cycle completed
    lcd.setCursor (0,0);
    lcd.print (F("Set Date Finish "));
    lcd.setCursor (0,1);
    lcd.print (F("Set Time Finish "));
    delay (1000);
    rtc.setTime (hh, mm, ss);
    rtc.setDate (dd, bb, yy);  
    lcd.clear();
    setMode = 0;
    }

    if (setAlarm == 4){                                    // cycle completed for alarm setting
    lcd.setCursor (0,0);
    lcd.print (F("Set Alarm Finish"));
    lcd.setCursor(0,1);
    lcd.print("              ");
    
    WriteEeprom();
    delay (2000); 
    lcd.clear();
    setAlarm=0;
    alarmMode=1;
    }

    if (alarm1_counter==10){
      lcd.clear(); 
      alarm1_counter = 0;
    }

    if (alarm2_counter==10){
      lcd.clear(); 
      alarm2_counter = 0;
    }

    if (alarm3_counter==10){
      lcd.clear(); 
      alarm3_counter = 0;
    }
    
 if (setAlarm >0){ alarmMode=0;}                               // do not ring alarm when adjusting alarm settings

 
    
 if(digitalRead (bt_time) == 0 && flag==0) {flag=1;             // so that keeping on holding the button will not increment setMode
 
//  if(setAlarm>0){setAlarm=5;}                                 // this means right now in setAlarm menu. Have to exit the alarm setting mode
//  else{setMode = setMode+1;}                                  // in first screen. increment setMode

    if(setAlarm==0){ setMode = setMode + 1;  }

    else if(setAlarm == 1){ alarm1_counter = alarm1_counter+1; }
    else if(setAlarm == 2){ alarm2_counter = alarm2_counter+1; }
    else if(setAlarm == 3){ alarm3_counter = alarm3_counter+1; }

   // lcd.clear();
 
 }



 
  
 if(digitalRead (bt_alarm) == 0 && flag==0){flag=1;
 
  if(setMode>0){setMode=8;}                               // this means right now in 1st screen. Have to exit the 1st screen
//  else{setAlarm = setAlarm+1;}                            // in alarm screen. increment setAlarm

  else if (alarm1_counter>0){alarm1_counter = 10;}
  else if (alarm2_counter>0){alarm2_counter = 10;}
  else if (alarm3_counter>0){alarm3_counter = 10;}

  else {setAlarm = setAlarm+1;}  
  
  lcd.clear();
  }


   

if(digitalRead (bt_time) == 1 && digitalRead (bt_alarm) == 1){flag=0;}        // If both bt_time and bt_alarm buttons are released, the flag variable is set to 0.

  
 if(digitalRead (bt_up) == 0){                          
            if (setAlarm<2 && setMode==1)hh=hh+1; 
            if (setAlarm<2 && setMode==2)mm=mm+1;
            if (setAlarm<2 && setMode==3)ss=ss+1;
            if (setAlarm<2 && setMode==4)set_day=set_day+1;
            if (setAlarm<2 && setMode==5)dd=dd+1;
            if (setAlarm<2 && setMode==6)bb=bb+1;
            if (setAlarm<2 && setMode==7)yy=yy+1;

            if (setAlarm==0 && setMode==0) { lcd.backlight();   }


           

            if (setMode==0 && setAlarm==1){
              if(alarm1_counter==1){mode=1;}
              if(alarm1_counter==2){AlarmHH=AlarmHH+1 ;}
              if(alarm1_counter==3){AlarmMM=AlarmMM+1 ;}
              if(alarm1_counter==4){AlarmSS=AlarmSS+1 ;}
              if(alarm1_counter==5){A_1 = A_1 + 1 ;}
              if(alarm1_counter==6){B_1 = B_1 + 1 ;}
              if(alarm1_counter==7){C_1 = C_1 + 1 ;}
              if(alarm1_counter==8){D_1 = D_1 + 1 ;}
              if(alarm1_counter==9){E_1 = E_1 + 1 ;}
              
             
            }

            if (setMode==0 && setAlarm==2){
              if(alarm2_counter==1){mode2=1;}
              if(alarm2_counter==2){Alarm2HH=Alarm2HH+1 ;}
              if(alarm2_counter==3){Alarm2MM=Alarm2MM+1 ;}
              if(alarm2_counter==4){Alarm2SS=Alarm2SS+1 ;}
              if(alarm2_counter==5){A_2 = A_2 + 1 ;}
              if(alarm2_counter==6){B_2 = B_2 + 1 ;}
              if(alarm2_counter==7){C_2 = C_2 + 1 ;}
              if(alarm2_counter==8){D_2 = D_2 + 1 ;}
              if(alarm2_counter==9){E_2 = E_2 + 1 ;}
              
             
            }

            if (setMode==0 && setAlarm==3){
              if(alarm3_counter==1){mode3=1;}
              if(alarm3_counter==2){Alarm3HH=Alarm3HH+1 ;}
              if(alarm3_counter==3){Alarm3MM=Alarm3MM+1 ;}
              if(alarm3_counter==4){Alarm3SS=Alarm3SS+1 ;}
              if(alarm3_counter==5){A_3 = A_3 + 1 ;}
              if(alarm3_counter==6){B_3 = B_3 + 1 ;}
              if(alarm3_counter==7){C_3 = C_3 + 1 ;}
              if(alarm3_counter==8){D_3 = D_3 + 1 ;}
              if(alarm3_counter==9){E_3 = E_3 + 1 ;}
              
             
            }

            

if(hh>23)hh=0;
if(mm>59)mm=0;
if(ss>59)ss=0;
if(set_day>7)set_day=0;
if(dd>31)dd=0;
if(bb>12)bb=0;
if(yy>2030)yy=2000;
rtc.setDOW(set_day);

if(AlarmHH>23)AlarmHH=0;
if(AlarmMM>59)AlarmMM=0;
if(AlarmSS>59)AlarmSS=0;

if(Alarm2HH>23)Alarm2HH=0;
if(Alarm2MM>59)Alarm2MM=0;
if(Alarm2SS>59)Alarm2SS=0;

if(Alarm3HH>23)Alarm3HH=0;
if(Alarm3MM>59)Alarm3MM=0;
if(Alarm3SS>59)Alarm3SS=0;

if(A_1>9)A_1=0;
if(B_1>9)A_1=0;
if(C_1>9)A_1=0;
if(D_1>9)A_1=0;
if(E_1>9)A_1=0;


if(A_2>9)A_2=0;
if(B_2>9)A_2=0;
if(C_2>9)A_2=0;
if(D_2>9)A_2=0;
if(E_2>9)A_2=0;


if(A_3>9)A_3=0;
if(B_3>9)A_3=0;
if(C_3>9)A_3=0;
if(D_3>9)A_3=0;
if(E_3>9)A_3=0;


 }        

if(digitalRead (bt_down) == 0){                                      
            if (setAlarm<2 && setMode==1)hh=hh-1; 
            if (setAlarm<2 && setMode==2)mm=mm-1;
            if (setAlarm<2 && setMode==3)ss=ss-1;
            if (setAlarm<2 && setMode==4)set_day=set_day-1;
            if (setAlarm<2 && setMode==5)dd=dd-1;
            if (setAlarm<2 && setMode==6)bb=bb-1;
            if (setAlarm<2 && setMode==7)yy=yy-1;

            if (setAlarm==0 && setMode==0) { lcd.noBacklight();   }

        

              if (setMode==0 && setAlarm==1){
               if(alarm1_counter==1){mode=0;}
               if(alarm1_counter==2){AlarmHH=AlarmHH-1 ;}
               if(alarm1_counter==3){AlarmMM=AlarmMM-1 ;}
               if(alarm1_counter==4){AlarmSS=AlarmSS-1 ;}
               if(alarm1_counter==5){A_1 = A_1 - 1 ;}
               if(alarm1_counter==6){B_1 = B_1 - 1 ;}
               if(alarm1_counter==7){C_1 = C_1 - 1 ;}
               if(alarm1_counter==8){D_1 = D_1 - 1 ;}
               if(alarm1_counter==9){E_1 = E_1 - 1 ;}
               
             
            }

            if (setMode==0 && setAlarm==2){
              if(alarm2_counter==1){mode2=0;}
              if(alarm2_counter==2){Alarm2HH=Alarm2HH-1 ;}
              if(alarm2_counter==3){Alarm2MM=Alarm2MM-1 ;}
              if(alarm2_counter==4){Alarm2SS=Alarm2SS-1 ;}
              if(alarm2_counter==5){A_2 = A_2 - 1 ;}
              if(alarm2_counter==6){B_2 = B_2 - 1 ;}
              if(alarm2_counter==7){C_2 = C_2 - 1 ;}
              if(alarm2_counter==8){D_2 = D_2 - 1 ;}
              if(alarm2_counter==9){E_2 = E_2 - 1 ;}
              
             
            }

            if (setMode==0 && setAlarm==3){
              if(alarm3_counter==1){mode3=0;}
              if(alarm3_counter==2){Alarm3HH=Alarm3HH-1 ;}
              if(alarm3_counter==3){Alarm3MM=Alarm3MM-1 ;}
              if(alarm3_counter==4){Alarm3SS=Alarm3SS-1 ;}
              if(alarm3_counter==5){A_3 = A_3 - 1 ;}
              if(alarm3_counter==6){B_3 = B_3 - 1 ;}
              if(alarm3_counter==7){C_3 = C_3 - 1 ;}
              if(alarm3_counter==8){D_3 = D_3 - 1 ;}
              if(alarm3_counter==9){E_3 = E_3 - 1 ;}
              
             
            }





            
if(hh<0||hh>200)hh=23;
if(mm<0||mm>200)mm=59;
if(ss<0||ss>200)ss=59;
if(set_day<0||set_day>200)set_day=7;
if(dd<0||dd>200)dd=31;
if(bb<0||bb>200)bb=12;
if(yy<0)yy=2030;
rtc.setDOW(set_day);

if(AlarmHH<0||AlarmHH>200)AlarmHH=23;
if(AlarmMM<0||AlarmMM>200)AlarmMM=59;
if(AlarmSS<0||AlarmSS>200)AlarmSS=59;

if(Alarm2HH<0||Alarm2HH>200)Alarm2HH=23;
if(Alarm2MM<0||Alarm2MM>200)Alarm2MM=59;
if(Alarm2SS<0||Alarm2SS>200)Alarm2SS=59;

if(Alarm3HH<0||Alarm3HH>200)Alarm3HH=23;
if(Alarm3MM<0||Alarm3MM>200)Alarm3MM=59;
if(Alarm3SS<0||Alarm3SS>200)Alarm3SS=59;


if(A_1<0||A_1>200)A_1=9;
if(B_1<0||B_1>200)B_1=9;
if(C_1<0||C_1>200)C_1=9;
if(D_1<0||D_1>200)D_1=9;
if(E_1<0||E_1>200)E_1=9;


if(A_2<0||A_2>200)A_2=9;
if(B_2<0||B_2>200)B_2=9;
if(C_2<0||C_2>200)C_2=9;
if(D_2<0||D_2>200)D_2=9;
if(E_2<0||E_2>200)E_2=9;


if(A_3<0||A_3>200)A_3=9;
if(B_3<0||B_3>200)B_3=9;
if(C_3<0||C_3>200)C_3=9;
if(D_3<0||D_3>200)D_3=9;
if(E_3<0||E_3>200)E_3=9;




 }
 
}

void setTimer (){
  //Alarm
// if (setMode == 0 && setAlarm >0){
//  lcd.setCursor (0,0);
//  lcd.print("Alarm ");
// if(mode==0){lcd.print("Deactivate");}
//        else{lcd.print("Activated ");}
//        
//  lcd.setCursor (4,1);
//  lcd.print((AlarmHH/10)%10);
//  lcd.print(AlarmHH % 10);
//  lcd.print(":");
//  lcd.print((AlarmMM/10)%10);
//  lcd.print(AlarmMM % 10);
//  lcd.print(":");
//  lcd.print((AlarmSS/10)%10);
//  lcd.print(AlarmSS % 10);
// }

  if (setMode == 0 && setAlarm >0){

    if((alarm1_counter==0)&&(alarm2_counter==0)&&(alarm3_counter==0)) {
      lcd.setCursor(5,0);
      lcd.print("Alarms");
      lcd.setCursor(0,1);
      lcd.print(" 1  2  3     ");
    // lcd.print(setMode);
     //lcd.print(setAlarm);
    }



    else if ((1<= alarm1_counter)&&( alarm1_counter<= 4)){
      
     // lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Alarm 1  ");
      if (mode==0){lcd.print("OFF");}
      else{lcd.print("ON");}

      lcd.setCursor(0,1);
      lcd.print("   ");
      lcd.setCursor (4,1);
      lcd.print((AlarmHH/10)%10);
      lcd.print(AlarmHH % 10);
      lcd.print(":");
      lcd.print((AlarmMM/10)%10);
      lcd.print(AlarmMM % 10);
      lcd.print(":");
      lcd.print((AlarmSS/10)%10);
      lcd.print(AlarmSS % 10);
      lcd.print(" ");
      
    }

    else if (4 < alarm1_counter){
      
     // lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A-");
      lcd.print(A_1);
      lcd.print(" B-");
      lcd.print(B_1);
      lcd.print(" C-");
      lcd.print(C_1);
      lcd.print("   ");

      lcd.setCursor(0,1);
      lcd.print("D-");
      lcd.print(D_1);
      lcd.print(" E-");
      lcd.print(E_1);
      lcd.print("     ");
      
    }



     else if ((1<= alarm2_counter)&&( alarm2_counter<= 4)){

     // lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Alarm 2  ");
      if (mode2==0){lcd.print("OFF");}
      else{lcd.print("ON");}

      lcd.setCursor(0,1);
      lcd.print("   ");
      lcd.setCursor (4,1);
      lcd.print((Alarm2HH/10)%10);
      lcd.print(Alarm2HH % 10);
      lcd.print(":");
      lcd.print((Alarm2MM/10)%10);
      lcd.print(Alarm2MM % 10);
      lcd.print(":");
      lcd.print((Alarm2SS/10)%10);
      lcd.print(Alarm2SS % 10);
      
    }

    else if (4 < alarm2_counter){

    //  lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A-");
      lcd.print(A_2);
      lcd.print(" B-");
      lcd.print(B_2);
      lcd.print(" C-");
      lcd.print(C_2);
      lcd.print("   ");

      lcd.setCursor(0,1);
      lcd.print("D-");
      lcd.print(D_2);
      lcd.print(" E-");
      lcd.print(E_2);
      lcd.print("     ");
      
      
    }

    else if ((1<= alarm3_counter)&&( alarm3_counter<= 4)){

     // lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Alarm 3  ");
      if (mode3==0){lcd.print("OFF");}
      else{lcd.print("ON");}

      lcd.setCursor(0,1);
      lcd.print("   ");
      lcd.setCursor (4,1);
      lcd.print((Alarm3HH/10)%10);
      lcd.print(Alarm3HH % 10);
      lcd.print(":");
      lcd.print((Alarm3MM/10)%10);
      lcd.print(Alarm3MM % 10);
      lcd.print(":");
      lcd.print((Alarm3SS/10)%10);
      lcd.print(Alarm3SS % 10);
      
    }

    else if (4 < alarm3_counter){

     // lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("A-");
      lcd.print(A_3);
      lcd.print(" B-");
      lcd.print(B_3);
      lcd.print(" C-");
      lcd.print(C_3);
      lcd.print("   ");

      lcd.setCursor(0,1);
      lcd.print("D-");
      lcd.print(D_3);
      lcd.print(" E-");
      lcd.print(E_3);
      lcd.print("     ");
     
    }

    
    
  }

}







void ReadEeprom () {
  
  AlarmHH=EEPROM.read(1);
  AlarmMM=EEPROM.read(2);
  AlarmSS=EEPROM.read(3);
  mode=EEPROM.read(4); 
  A_1 = EEPROM.read(5);
  B_1 = EEPROM.read(6);
  C_1 = EEPROM.read(7);
  D_1 = EEPROM.read(8);
  E_1 = EEPROM.read(9);
  

  Alarm2HH=EEPROM.read(11);
  Alarm2MM=EEPROM.read(12);
  Alarm2SS=EEPROM.read(13);
  mode2=EEPROM.read(14); 
  A_2 = EEPROM.read(15);
  B_2 = EEPROM.read(16);
  C_2 = EEPROM.read(17);
  D_2 = EEPROM.read(18);
  E_2 = EEPROM.read(19);
  

  Alarm3HH=EEPROM.read(21);
  Alarm3MM=EEPROM.read(22);
  Alarm3SS=EEPROM.read(23);
  mode3=EEPROM.read(24);
  A_3 = EEPROM.read(25);
  B_3 = EEPROM.read(26);
  C_3 = EEPROM.read(27);
  D_3 = EEPROM.read(28);
  E_3 = EEPROM.read(29);
  
  
}

void WriteEeprom () {
  EEPROM.write(1,AlarmHH);
  EEPROM.write(2,AlarmMM);
  EEPROM.write(3,AlarmSS);
  EEPROM.write(4,mode);
  EEPROM.write(5,A_1);
  EEPROM.write(6,B_1);
  EEPROM.write(7,C_1);
  EEPROM.write(8,D_1);
  EEPROM.write(9,E_1);
 

  EEPROM.write(11,Alarm2HH);
  EEPROM.write(12,Alarm2MM);
  EEPROM.write(13,Alarm2SS);
  EEPROM.write(14,mode2);
  EEPROM.write(15,A_2);
  EEPROM.write(16,B_2);
  EEPROM.write(17,C_2);
  EEPROM.write(18,D_2);
  EEPROM.write(19,E_2);
  

  EEPROM.write(21,Alarm3HH);
  EEPROM.write(22,Alarm3MM);
  EEPROM.write(23,Alarm3SS);
  EEPROM.write(24,mode3);
  EEPROM.write(25,A_3);
  EEPROM.write(26,B_3);
  EEPROM.write(27,C_3);
  EEPROM.write(28,D_3);
  EEPROM.write(29,E_3);
    
}

void readBTData() {
  char nextData;

  if (BTSerial.available()) {  // replace Serial with BTSerial if you are using SoftwareSerial
    // check for start of message
    if (waitingForStartToken) {
      do {
        nextData = BTSerial.read();   // BT
      } while ((nextData != START_TOKEN) && BTSerial.available());   //BT
      if (nextData == START_TOKEN) {
        Serial.println("message start");
        waitingForStartToken = false;
      }
    }

    // read command
    if (!waitingForStartToken && BTSerial.available()) {              //BT
      do {
        nextData = BTSerial.read();    //BT

        if (nextData != '\n') {
          Serial.println(nextData);
          messageBuffer += nextData;
        }
      } while ((nextData != END_TOKEN) && BTSerial.available());   //BT

      // check if message complete
      if (nextData == END_TOKEN) {
        // remove last character
        messageBuffer = messageBuffer.substring(0, messageBuffer.length() - 1);
        Serial.println("message complete - " + messageBuffer);
        //setLeds(messageBuffer);    ---  setBTtime(messageBuffer)

         if (messageBuffer.length() > 10 ) {
        setBTtime(messageBuffer);
        }

        else{
          setTimerDurations(messageBuffer);
          
        }



        
        messageBuffer = "";
        waitingForStartToken = true;
      }

      // check for char timeout
      if (messageBuffer.length() > CHAR_TIMEOUT) {
        Serial.println("message data timeout - " + messageBuffer);
        messageBuffer = "";
        waitingForStartToken = true;
      }
    }
  }
}

void setTimerDurations(String message){

  // message = "075040" (n = 75, m = 40)
  n = message.substring(0, 3).toInt();
  m = message.substring(3, 6).toInt();

  EEPROM.write(71,n);
  EEPROM.write(72,m);

  Serial.print("n = ");
  Serial.print(n);
  Serial.print("  , m = ");
  Serial.println(m);

  
  
}


void setBTtime(String message) {





  //message = "1011453011020_0"   - alarm 1 off , time - 11:45:30, doses- A-1, B-1, C-0, D-2, E-0        (14 characters)  //final '0' is not important

  // byte alarm_no = message.substring(0,1).toInt();
  byte alarm_no = message.charAt(0) - '0';

  if (alarm_no != 0 ) { // 0 means time


    byte state = message.charAt(1) - '0';
    byte hours = message.substring(2, 4).toInt();
    byte minutes = message.substring(4, 6).toInt();
    byte seconds = message.substring(6, 8).toInt();
    byte A = message.charAt(8) - '0';
    byte B = message.charAt(9) - '0';
    byte C = message.charAt(10) - '0';
    byte D = message.charAt(11) - '0';
    byte E = message.charAt(12) - '0';

    if (hours>23){ hours=23;}
    if (minutes>59){ minutes = 59;}
    if (seconds>59){ seconds = 59;}

    if (alarm_no == 1) {  //alarm 1

      mode = state;
      AlarmHH = hours;
      AlarmMM = minutes;
      AlarmSS = seconds;

      A_1 = A;
      B_1 = B;
      C_1 = C;
      D_1 = D;
      E_1 = E;

      EEPROM.write(1, AlarmHH);
      EEPROM.write(2, AlarmMM);
      EEPROM.write(3, AlarmSS);
      EEPROM.write(4, mode);
      EEPROM.write(5, A_1);
      EEPROM.write(6, B_1);
      EEPROM.write(7, C_1);
      EEPROM.write(8, D_1);
      EEPROM.write(9, E_1);




    }

    if (alarm_no == 2) {  //alarm 2

      mode2 = state;
      Alarm2HH = hours;
      Alarm2MM = minutes;
      Alarm2SS = seconds;

      A_2 = A;
      B_2 = B;
      C_2 = C;
      D_2 = D;
      E_2 = E;

      EEPROM.write(11, Alarm2HH);
      EEPROM.write(12, Alarm2MM);
      EEPROM.write(13, Alarm2SS);
      EEPROM.write(14, mode2);
      EEPROM.write(15, A_2);
      EEPROM.write(16, B_2);
      EEPROM.write(17, C_2);
      EEPROM.write(18, D_2);
      EEPROM.write(19, E_2);

    }

    if (alarm_no == 3) {  //alarm 3

      mode3 = state;
      Alarm3HH = hours;
      Alarm3MM = minutes;
      Alarm3SS = seconds;

      A_3 = A;
      B_3 = B;
      C_3 = C;
      D_3 = D;
      E_3 = E;

      EEPROM.write(21, Alarm3HH);
      EEPROM.write(22, Alarm3MM);
      EEPROM.write(23, Alarm3SS);
      EEPROM.write(24, mode3);
      EEPROM.write(25, A_3);
      EEPROM.write(26, B_3);
      EEPROM.write(27, C_3);
      EEPROM.write(28, D_3);
      EEPROM.write(29, E_3);

    }

    Serial.print("Alarm " );
    Serial.print(alarm_no);
    Serial.print("  ");
    Serial.print(state);
    Serial.print("  ");
    Serial.print(hours);
    Serial.print(" : ");
    Serial.print(minutes);
    Serial.print(" : ");
    Serial.println(seconds);

    Serial.print("A - ");
    Serial.print(A);
    Serial.print("B - ");
    Serial.print(B);
    Serial.print("C - ");
    Serial.print(C);
    Serial.print("D - ");
    Serial.print(D);
    Serial.print("E - ");
    Serial.print(E);

  }



  else {          ///alarmmode = 0 - means time    0_12_30_00_23_05_03_1  - Monday
    // time 00_13_30_00_2023_05_01

    byte hours = message.substring(1, 3).toInt();
    byte minutes = message.substring(3, 5).toInt();
    byte seconds = message.substring(5, 7).toInt();
    int years = message.substring(7, 9).toInt() + 2000;
    byte months = message.substring(9, 11).toInt();
    byte days = message.substring(11, 13).toInt();
    byte dayofweek =  message.charAt(13) - '0';

    if (hours>23){ hours=23;}
    if (minutes>59){ minutes = 59;}
    if (seconds>59){ seconds = 59;}

    if (months>12){ months=12;}
    if (days>31){ days=31;}
    if (dayofweek >7){ dayofweek=7;}


    hh = hours;
    mm = minutes;
    ss = seconds;
    yy = years;
    bb = months;
    dd = days;
    set_day = dayofweek;

    Serial.print("Time  " );
    Serial.print(hh);
    Serial.print(" : ");
    Serial.print(mm);
    Serial.print(" : ");
    Serial.println(ss);

    Serial.print("Date " );
    Serial.print(yy);
    Serial.print(" / ");
    Serial.print(bb);
    Serial.print(" / ");
    Serial.println(dd);

    // Serial.println(yy + "

    rtc.setTime (hh, mm, ss);
    rtc.setDate (dd, bb, yy);
    rtc.setDOW(set_day);



  }

}
