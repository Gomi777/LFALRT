// GET $GPRMC LINE FROM GPS-module.
// USING: MKRGSM1400 & ADAFRUIT ULTIMATE GPS BREAKOUT v3
/*
 * GPS TEST
 * MKRGSM1400
 */
#include <Arduino.h>
// Include the GSM library
#include <MKRGSM.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number

const char PINNUMBER[] = "0000";

// initialize the library instance

GSM gsmAccess;

GSM_SMS sms;

#define SerialGPS Serial1  // RX&TX PINS
#define CHECK_INTERVAL 5  //seconds.
//VARS

 char remoteNum[20]= "7326749230";
int buttonState = 0; 
char txtMsg[300]= "I need help at : \n"; 
unsigned long previousTest;
bool mGPS_got_line = false, mGPS_paused = false;
uint8_t mGPS_idx=0;
char mGPS_TempLine[120];

void setup() {
  Serial.begin(9600); //USB
  SerialGPS.begin(9600); //TEST GPS.

  while(!Serial) { ; } //WAIT FOR SERIAL USB.
  Serial.print("\nTEST SOMETHING\n\n");
  pinMode(6, INPUT);
  bool connected = false;
   while (!connected) {

    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {

      connected = true;

    } else {

      Serial.println("Not connected");

      delay(1000);

   }
  }
  Serial.println("GSM initialized");

}

void loop() {

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
  //Serial.write(mGPS);   //DEBUG
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
    Serial.print(mGPS_TempLine);
    //DO SOMETHING WITH THE LINE.
    
    //done parsed.
    mGPS_paused = true; mGPS_got_line = false; //Reset to get a new after paused has been reset.
  }
  else
  {
    mGPS_got_line = false; //wrong line not GPRMC. get another.
  }
}


  char dest[300];

//while (!(sizeof(dest)==((sizeof(txtMsg)/sizeof(txtMsg[0]))+1+(sizeof(mGPS_TempLine)/sizeof(mGPS_TempLine[0])))))
//{
//  strcpy(dest, txtMsg);
//  strcat(dest, ";");
//  strcat(dest, mGPS_TempLine);
//  }
//  Serial.print(dest);
//  sms.beginSMS(remoteNum);
//
//  //sms.print(txtMsg);
//  sms.print(mGPS_TempLine);
//     delay(20000);
//   sms.endSMS();


buttonState = digitalRead(6);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if ((buttonState == HIGH) && (strstr(mGPS_TempLine, "$GPRMC")))
  // if ((buttonState == HIGH) && (gsmAccess.begin(PINNUMBER) == GSM_READY) && (strstr(mGPS_TempLine, "$GPRMC")))
  {
Serial.print("pressed");
  sms.beginSMS(remoteNum);

  sms.print(dest);
Serial.print("ok");


Serial.print("passed");
//  } else if ((buttonState == HIGH)&& (!(gsmAccess.begin(PINNUMBER) == GSM_READY))) {
//     // turn LED on:
//    digitalWrite(7, HIGH);
//  }

  }
} //END LOOP
