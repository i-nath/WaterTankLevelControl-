/*
 * 
 * water level monitor  
 * two nodes
 * node 1 contains a arduino pro mini and ultrasonic sensor
 * node 2 contains a arfuino and 16*2 LCD and switchs and relay
 * node1 and node comunicate each other with serial communication
 * node 1 is in passive state, waiting for the request from node 2
 * when request recived the node 1 send the data to node 2
 * here two nodes are connected with help of max485 comunication is RS485
 * Implemented max485 comunication
 * Half duplex handshaking communication
 * 
 * Project:HELIUM
 * ZAKTITAS RESEARCH AND DEVOELPMENT 
 */
 int opt=0,opt1=0;
unsigned long timetake;
const static int trigger=5;
const static int echo=6;
void setup() {
 Serial.begin(9600);
 pinMode(trigger,OUTPUT);
 pinMode(echo,INPUT);
 pinMode(9,INPUT_PULLUP);
  pinMode(7, OUTPUT);
  digitalWrite(7,LOW);
}
long dist()
{
  digitalWrite(trigger,LOW);
  delayMicroseconds(2);
  digitalWrite(trigger,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger,LOW);
  delayMicroseconds(2);
  timetake=pulseIn(echo,HIGH,30000);
  return ((timetake/2)*0.03432);//sound speed in air is 34320 centimeters per second
}
void loop() {

  digitalWrite(7,LOW);
  delay(10);
  if(Serial.available())
  {
   opt1= Serial.read();
   if(opt1!=-1)
   opt=opt1;
   delay(10);
   digitalWrite(7,HIGH);
   delay(10);
   switch(opt)
   {
    case 48:Serial.println(dist(),DEC);
            opt=48;
            break;
    case 69: Serial.println("E");
              opt=69;
              break;
   }
  }
  delay(10);
}
