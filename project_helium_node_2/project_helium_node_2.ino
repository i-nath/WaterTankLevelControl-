/////////////////PROJECT : HELIUM ////////////////////////////////
/*
 * 
 * water level monitor  
 * two nodes
 * node 1 contains a arduino pro mini and ultrasonic sensor
 * node 2 contains a arfuino and 16*2 LCD and switchs and relay
 * node1 and node comunicate each other with serial communication
 * node 1 is in passive state, waiting for the request from node 2
 * when request recived the node 1 send the data to node 2
 * Implemented max485 comunication
 * Half duplex handshaking communication
 *
 *Project:HELIUM
 * ZAKTITAS RESEARCH AND DEVOELPMENT 
 */
#include <LiquidCrystal.h>
#include<EEPROM.h>
LiquidCrystal lcd(7,8,9,10,11,12);
byte auto_on;
byte full[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
byte copyr[8]=
{
  0b01110,
  0b10111,
  0b11001,
  0b11001,
  0b11001,
  0b10111,
  0b01110,
};
const int offswitch=6,onswitch=4,aonswitch=5,light_on_switch=3; //switches
const int aonled=14,mled=15,mrelay=16,moffled=17,dled=18,eled=13;//led and relay
int k=1,m,dled_stat=1,level=0,dtimer=0;
unsigned long ct,pt;
const long interval = 10000;

////////////progrom starts here/////////////////////
void setup() {
     Serial.begin(9600);
     //////*reading setting from eeprom*///////
    auto_on=EEPROM.read(1);
    ////////////////////////////
    
    ////Setting the switches to inputpulllup//////
     pinMode(aonswitch,INPUT_PULLUP);
     pinMode(offswitch,INPUT_PULLUP);
     pinMode(onswitch,INPUT_PULLUP);
     pinMode(light_on_switch,INPUT_PULLUP);
     ////////////////////////////////

     ////setting status ledpins ////
     pinMode(mled,OUTPUT);
     pinMode(aonled,OUTPUT);
     pinMode(moffled,OUTPUT);
     pinMode(dled,OUTPUT);
     pinMode(eled,OUTPUT);
     //////////////////////////////
     digitalWrite(mled,0);
     digitalWrite(eled,0);
     digitalWrite(dled,1);
     digitalWrite(moffled,1);
     digitalWrite(aonled,auto_on);
     //////////////
     pinMode(mrelay,OUTPUT);
     digitalWrite(mrelay,HIGH);//low trigger relay 
     /////////////
     pinMode(2, OUTPUT);//max485 send,recive control pin
     digitalWrite(2,LOW);//intially reciving mode
     lcd.begin(16, 2);
     lcd.createChar(2, full);
     lcd.createChar(3, copyr);
     lcd.setCursor(5, 0);
     lcd.print("WELCOME");
     lcd.setCursor(2, 1);
     lcd.write(3);
     lcd.setCursor(3,1);
     lcd.print("ZAKTITAS R&D");
     delay(5000);
     lcd.clear();
     lcd.setCursor(0, 1);
     lcd.print("    MOTOR OFF    ");
     Serial.setTimeout(2000);//serial time out  
  }
  
 //////////////////////////////
int testconnection()
{
     int i;
     digitalWrite(2,HIGH);
     delay(100);
     Serial.write('E');
     delay(10);
     digitalWrite(2,LOW);
     delay(10);
     for(i=0;i<10;i++)
     {
        delay(10);
        if(Serial.available())
        {
          if(Serial.read()==69)
          {
            return 1;
          }
        }
    }
    return 0;
}

///////////////////////////

//////////////////////////
int distance()
{
  int j,dist;
  j=testconnection();
  delay(100);
  if (j==1)
  {

    digitalWrite(2,HIGH);
    Serial.write("0");
    delay(10);
    digitalWrite(2,LOW);
    delay(10);
    if(Serial.available()){
    dist=Serial.parseInt();
    return dist;
    }
  } 
  else 
  return -1;
}
////////////////////////////

///////////////////////////
void motor_on()
{
  m=distance();
  if(m>9 || m==-1)
  {
    digitalWrite(mrelay,LOW);
    digitalWrite(mled,HIGH);
    digitalWrite(moffled,LOW);
    lcd.setCursor(0, 1);
    lcd.print("    MOTOR ON    ");
    while(1)
    {
      m=distance();
      printlevel();
      if(!digitalRead(aonswitch))
      {
        updatesettings(1);
        lcd.setCursor(0, 1);
        lcd.print("    MOTOR ON    ");
      }
      if((m<=6 && m!=-1) || !digitalRead(offswitch))
      {
        digitalWrite(mrelay,HIGH);
        digitalWrite(mled,LOW);
        digitalWrite(moffled,HIGH);
        lcd.setCursor(0, 1);
        lcd.print("    MOTOR OFF   ");
        delay(200);
        break;
      }  
   }
   delay(1000);
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("ALREADY FULLED");
    delay(3000);
    lcd.setCursor(0,1);
    lcd.print("    MOTOR OFF   ");
    
    
  } 
}
/////////////////////////////////

////////////////////////////////
void updatesettings(int add)
{
  if(add==1)
  {
   if(auto_on==1)
   {
    EEPROM.write(add,0);
    lcd.setCursor(0, 1);
    lcd.print("Auto_on OFF      ");
   }
   else
   {
    EEPROM.write(add,1);
    lcd.setCursor(0, 1);
    lcd.print("Auto_on ON      ");
   }
  }
  auto_on=EEPROM.read(1);
  digitalWrite(aonled,auto_on);
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("    MOTOR OFF    ");
}
///////////////////////////////

////////////////////////////////
void standby()
{
  m=distance();
 printlevel();
   if(auto_on==1)
  {
    if(m>69)
    {
      dled_stat=1;
    digitalWrite(dled,HIGH);
    motor_on();
    }
  }   
}
/////////////////////////////////

//////////////////////////////////
void printlevel()
{
  int x;
  m=distance();
  if(m==-1)
  {
   lcd.setCursor(0, 0);
   lcd.print("__Sensor_Failed___");
   digitalWrite(eled,HIGH);
   return;
  }
  lcd.setCursor(0, 0);
  lcd.print("LEVEL:");
  digitalWrite(eled,0);
  if(m>75)
  level=0;
  else if(m>68 && m<=75)
  level=1;
  else if(61<m && m<=68)
  level=2;
  else if(54<m && m<=61)
  level=3;
  else if(47<m && m<=54)
  level=4;
  else if(40<m && m<=47)
  level=5;
  else if(33<m && m<=40)
  level=6;
  else if(26<m && m<=33)
  level=7;
  else if(19<m && m<=26)
  level=8;
  else if(12<=m && m<=19)
  level=9;
  else if(m<12)
  level=10;
  
  lcd.setCursor(6, 0);
  lcd.print("          ");
  lcd.setCursor(15, 0);
  for(x=1;x<=level;x++)
  {
    lcd.write(2);
    lcd.setCursor((15-x ), 0);
  }
}
////////////////////////////////////

///////////////////////////////////
void loop() {
  standby();
  if(!digitalRead(aonswitch))
  {
    dled_stat=1;
    digitalWrite(dled,1);
    updatesettings(1);
    lcd.setCursor(0, 1);
    lcd.print("    MOTOR OFF    ");
  }
  if(!digitalRead(onswitch))
  {
    dled_stat=1;
    digitalWrite(dled,HIGH);
    motor_on();
  }
  if(!digitalRead(light_on_switch))
  {
    dled_stat=1;
  }
  if(dled_stat==1)
  {
    dtimer=1;
    dled_stat=0;
    pt=millis();
    digitalWrite(dled,HIGH);
  }
  if(dtimer==1)
  {
    ct=millis();
    if((ct-pt)>interval)
    {
      dtimer=0;
      digitalWrite(dled,LOW);
      
    }
  }
  delay(100);
}


/////////////////PROJECT : HELIUM ////////////////////////////////
