// GET $GPRMC LINE FROM GPS-module.
// USING: MKRGSM1400 & ADAFRUIT ULTIMATE GPS BREAKOUT v3

#include <Arduino.h>
#include <MKRGSM.h>
#include <wiring_private.h>
#include <string>
#include <Arduino.h>#include "wiring_private.h"
#include <ctype.h>
Uart Serialbt(&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0); //adding another serial port

void SERCOM3_Handler(){  Serialbt.IrqHandler();}
#include <CapacitiveSensor.h>
CapacitiveSensor   cs_4_5 = CapacitiveSensor(4,5);
CapacitiveSensor   cs_8_9 = CapacitiveSensor(8,9);       //First Digital input has resistor, second doesn't
CapacitiveSensor   cs_2_3 = CapacitiveSensor(2,3); 
bool b1=false, b2=false, b3=false;
const char PINNUMBER[] = "0000";

// initialize the library instance

GSM gsmAccess;

GSM_SMS sms;

#define SerialGPS Serial1  // RX&TX PINS
#define CHECK_INTERVAL 5  //seconds.

//VARS
int value = 1;
char remoteNum[20]; 
int buttonState = 0; 
char txtMsg[300]= "I need help at :"; 
char btxtMsg[300]= "I am lost at :"; 
char gpsEnv[50];
char gpshold[50];
char gpshort[30];
char ctxtMsg[300]; 
char btMsg[310];
bool blueC= false;
unsigned long previousTest;
bool mGPS_got_line = false, mGPS_paused = false;
uint8_t mGPS_idx=0;
char mGPS_TempLine[120];

bool numCheck(char *numArray)
{
    int i;
    const int len = strlen(numArray);

    for (i = 0; i < len; i++)
    {
        /* #include <ctype.h> for 'isdigit()'. */
        if (isdigit(numArray[i])) break;
        return false;
    }
      return true; 
    }


void setup() {
  
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);

  Serial.begin(9600); //USB
  SerialGPS.begin(9600); //TEST GPS
  Serialbt.begin(9600, SERIAL_8E1); 
while(!Serialbt){   
  Serial.println("DEBUG: error opening Serial_plc");}
 
pinPeripheral(0, PIO_SERCOM); //Assign RX function to pin 1  
pinPeripheral(1, PIO_SERCOM); //Assign TX function to pin 0
delay(1000);
Serial.println("\nLFALRT v1.0\n\n");

  bool connected = false;
  digitalWrite(6, HIGH);
   while (!connected) { //loop for connecting to GSM

    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {

      connected = true;

    } else {

      Serial.println("Not connected");
  digitalWrite(6, HIGH); 
  delay(500);
  digitalWrite(6, LOW);
  digitalWrite(6, HIGH); 
  delay(500);
  digitalWrite(6, LOW);
  digitalWrite(6, HIGH); 
  delay(500);
  digitalWrite(6, LOW);
  delay(1000);

   }
  }
Serial.println("GSM initialized");
digitalWrite(6, LOW);
}

void loop() {
  start:
    long start = millis();
    long total1 =  cs_8_9.capacitiveSensor(30); // 
    long total2 =  cs_2_3.capacitiveSensor(30);
    long total3 =  cs_4_5.capacitiveSensor(30);

if (blueC==true){
  goto blueC;
}

if  (total1 > 1000){
 digitalWrite(6, HIGH); 
 delay(1000);
 digitalWrite(6, LOW); 
}
if  (total2 > 1000){
 digitalWrite(6, HIGH); 
 delay(1000);
 digitalWrite(6, LOW); 
}
if  (total3 > 50){ //strange issue where button is weak
 digitalWrite(6, HIGH); 
 delay(1000);
 digitalWrite(6, LOW); 
}

Serial.print("Waiting for Bluetooth input\n");
  while (!blueC){
  digitalWrite(6, HIGH); 
  delay(500);
  digitalWrite(6, LOW);
    digitalWrite(7, HIGH); 
  delay(500);
  digitalWrite(7, LOW);
if(Serialbt.available())  
{
  Serial.print("Blutooth reciving \n");

int  i=0;
  byte n = Serialbt.available(); //check if a character has arrived via SUART Port

  if (n != 0) //a charctaer has arrived; it has been auto saved in FIFO; say 1 as 0x31
  {
    while(i<310){
    btMsg[i] = Serialbt.read(); //read arrived character from FIFO (say 1) and put into x as 0x31 
      //send 0x31 to Serial Monitor to show 1 via UART Port
        delay(50);
    i++;
    }
  }
 delay(2000);
    blueC=true;
}
}

blueC:  
  int i;
  for( i = 0; i < 10; i++) {
    int j= i+3;
  remoteNum[i]= btMsg[j];
  }
Serial.println("Phone number recived: ");Serial.println(remoteNum);

if (numCheck(remoteNum) == false)
{
       digitalWrite(7, HIGH); 
      delay(500);
       digitalWrite(7, LOW);
        delay(500);
       digitalWrite(7, HIGH); 
      delay(500);
       digitalWrite(7, LOW);
       Serial.println("not phone number");
}
i=0;
  for( i = 0; i < 40; i++) {
    int j= i+13;
    if (ctxtMsg[j] != '\342'){
  ctxtMsg[i]= btMsg[j];
    }
  }
  Serial.println(ctxtMsg);

getgps:

if (millis() - previousTest >= CHECK_INTERVAL*1000UL) 
{
  previousTest = millis();
  Serial.print("\n\nGET NEW LINE\n\n");
  if (mGPS_paused) mGPS_paused = false; 
}
char mGPS = 0;
if ((SerialGPS.available() > 0) && !mGPS_got_line && !mGPS_paused) 
{
  mGPS = SerialGPS.read();
 // Serial.write(mGPS);   //DEBUG
  if (mGPS == '\n') 
  {
    mGPS_TempLine[mGPS_idx] = 0; mGPS_idx = 0; mGPS_got_line = true;
  } 
  else
  {
     mGPS_TempLine[mGPS_idx++] = mGPS;
     if (mGPS_idx >= 120) mGPS_idx = 119; //dont overflow.
  }
}
if (mGPS_got_line)
{
      
  if (strstr(mGPS_TempLine, "$GPRMC"))
  {
    Serial.print("-->");Serial.println(mGPS_TempLine);
    
    mGPS_paused = true; mGPS_got_line = false; //Reset to get a new after paused has been reset.
  }
  else
  {
    mGPS_got_line = false; //wrong line not GPRMC. get another.
    goto getgps;
  }

  }
    else 
  {
  goto getgps;
  }

strcpy(gpshold, mGPS_TempLine);

i=0;
  for( i = 0; i < 40; i++) {
    int j= i+7;

  gpsEnv[i]= gpshold[j];
    
  }
Serial.println("Ready");
if  (total1 > 1000){
  b1 = true;
}
if  (total2 > 1000){

  b2 = true;
}
if  (total3 > 50){
 
  b3 = true;
}

if  (total1 > 1000 && total2 > 1000 && total3 > 50){
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    delay(1000);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    total1=0;
    total2=0;
    total3=0;
    for (int x = 0; x < sizeof(remoteNum) / sizeof(remoteNum[0]); x++)
{
  remoteNum[x] = 0;
}
    for (int x = 0; x < sizeof(txtMsg) / sizeof(txtMsg[0]); x++)
{
  txtMsg[x] = 0;
}
   
    blueC= false;
    Serial.println("Reset");
  goto start;
}
if  (b1==true){
     Serial.println("1 pressed");
      sms.beginSMS(remoteNum);
      sms.print(txtMsg);
      sms.print("\n");
      sms.print(gpsEnv);
      sms.print("\n If a N is at the end of this data it is wrong");

      sms.endSMS();

      Serial.println("sent");
      Serial.println(total1);
      digitalWrite(6, HIGH); 
      delay(1000);
       digitalWrite(6, LOW); 
       b1 = false;
 }
 
if  (b2==true){
  Serial.println(total2);
      Serial.println("2 pressed");

    sms.beginSMS(remoteNum);
    sms.print(btxtMsg);
    sms.print("\n");
    sms.print(gpsEnv);
    sms.print("\n If a N is at the end of this data it is wrong");

    sms.endSMS();
   
    Serial.println("sent");
    
    digitalWrite(6, HIGH);
    delay(1000);
    
    digitalWrite(6, LOW);
    b2 = false;
 }
    if  (b3==true){
    Serial.println("3 pressed");

    sms.beginSMS(remoteNum);
    sms.print(ctxtMsg);
    sms.print("\n");
    sms.print(gpsEnv);
    sms.print("\n If a N is at the end of this data it is wrong");
    sms.endSMS();

    Serial.println("sent");
    
    digitalWrite(6, HIGH); 
    delay(1000);
    digitalWrite(6, LOW); 
     b3 = false;
 }
} //END LOOP
