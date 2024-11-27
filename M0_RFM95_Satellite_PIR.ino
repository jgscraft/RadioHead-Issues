// M0_RFM95_Satellite_PIR (client)
//
// LoRa-based remote doorbell or motion sensor with low power sleep and wake on interrupt
//
// Adafruit reference:    https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module/overview
//
// LoRa Class reference:  http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html
//
// LoRa Examples:         http://www.airspayce.com/mikem/arduino/RadioHead/examples.html
//
// Reboot from software:  https://forum.arduino.cc/t/software-reset-in-arduino-zero/366181//
//
// From:      M0-RFM95-Satellite
//
// Hardware:  Adafruit Feather M0 RFM95 LoRa
//            Panasonic PIR EKMB1103113 1uA, 12M range, angle 92-102, or similar
//  
// Software:  Airspayce radio libraries
//            LowPower Library - https://docs.arduino.cc/libraries/arduino-low-power/
//
// 241105 - (JGS) Test low power settings and reboot
// 241107 - 0.12 mA in sleep   (0.02 mA less if remove voltage divider)
//          -- 2000mAh / .12 = 16,666 hrs
//          -- 1 year = 8760 hrs
//
// 241108 - working!
// 241111 - triple-blink LED on successful ACK from central
 
//------ What kind of unit?
//#define CENTRAL   // Alarm Central/Receiver
//#define UNIT1     // Front Door
#define UNIT2     // West Walk
//#define UNIT3     // Lake Path

#include "JGS_Alarm.h"
 
// Debugging switches and macros - use Serial1 ( #0=RX #1=TX)
#define DEBUG 0 // Switch debug output on and off by 1 or 0

#if DEBUG
#define PRINT_BEGIN(b)  { Serial1.begin(b); }
#define PRINT_DELAY(s)  { delay( s ); }
#define PRINTS(s)       { Serial1.print(F(s)); }
#define PRINT(s,v)      { Serial1.print(F(s)); Serial1.print(v); }
#define PRINTX(s,v)     { Serial1.print(F(s)); Serial1.print(v, HEX); }
#else
#define PRINT_BEGIN(b)
#define PRINT_DELAY(s)
#define PRINTS(s)
#define PRINT(s,v)
#define PRINTX(s,v)
#endif

//  #define RH_ENABLE_EXPLICIT_RETRY_DEDUP 1    // doesn't work
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

RH_RF95 radio(8,3);                             // pins for Feather M0 RFM95
RHReliableDatagram manager(radio, MYADDRESS);

uint8_t msgIn[RH_RF95_MAX_MESSAGE_LEN];         // receive buffer - max len 251
uint8_t msgOut[RH_RF95_MAX_MESSAGE_LEN];        // send buffer - max len 251

#include <ArduinoLowPower.h>                    // Note: Sleep() and DeepSleep() is same code

//------ WakeStuff - runs on wakeup interrupt ------
// Forces system reboot to ensure proper setup
void WakeStuff( void ) {
  NVIC_SystemReset();
}

//------ SleepStuff - turn off some things and go to sleep
void SleepStuff( void ){
    radio.sleep();

    //-- doing these reduces power
                        digitalWrite(13, LOW);    // turn off power LED
    pinMode(0, OUTPUT); digitalWrite(0, LOW);
    pinMode(1, OUTPUT); digitalWrite(1, LOW);
    pinMode(4, INPUT);
    pinMode(5, OUTPUT); digitalWrite(5, LOW);
    pinMode(7, OUTPUT); digitalWrite(7, LOW);
    pinMode(10, OUTPUT);digitalWrite(10, LOW);
    pinMode(11, OUTPUT);digitalWrite(11, LOW);
    pinMode(12, OUTPUT);digitalWrite(12, LOW);
    pinMode(20, OUTPUT);digitalWrite(20, LOW);
    pinMode(21, OUTPUT);digitalWrite(21, LOW);
    pinMode(22, OUTPUT);digitalWrite(22, LOW);
    pinMode(23, OUTPUT);digitalWrite(23, LOW);
    pinMode(24, OUTPUT);digitalWrite(24, LOW);

    delay(2500);  // slow down to avoid multiple triggers

    //-- Set up our interrupt and go back to sleep
    //-- Do this right before we go to sleep. Doing it earlier allows possible fast retriggering.
    LowPower.attachInterruptWakeup(6, WakeStuff, CHANGE);

    LowPower.sleep();
}
//------------------------------------- AlarmBeep() --------------------------------
//------ Beep or blink that we got the send confirm

uint8_t   pinBeep = 13;             // Connecter to onboard LED

void AlarmBeep( void ){
  digitalWrite(pinBeep, HIGH);
  delay(50);
  digitalWrite(pinBeep, LOW);
  delay(50);
  digitalWrite(pinBeep, HIGH);
  delay(50);
  digitalWrite(pinBeep, LOW);
  delay(50);
  digitalWrite(pinBeep, HIGH);
  delay(50);
  digitalWrite(pinBeep, LOW);
}

//-------------------------------------- setup() ------------------------------
void setup() {
  //-- do this on waking up...
  digitalWrite(13, LOW);          // turn off builtin LED
  PRINT_BEGIN(115200);
  PRINT_DELAY( 2000 );            // wake up serial port
  PRINTS(__DATE__); PRINTS("  "); PRINTS( __FILE__);

	//------ radio setup
  if( !manager.init()){
    PRINTS("Radio/manager init failed.");
  }
  radio.setFrequency( 915.0 );

  PRINTS("\nSending to rf95_reliable_datagram_server");
  // Send a message to manager_server
  strcpy((char*)msgOut,(char*)RFLabel);
  if (manager.sendtoWait(msgOut, strlen((char*)msgOut), TARGETADDR)) {
    // Now wait for a reply from the server
    uint8_t len = sizeof(msgIn);
    uint8_t from;   
    if (manager.recvfromAckTimeout(msgIn, &len, 2000, &from)) {
      AlarmBeep();    // indicate ACK
      PRINTX("\ngot reply from : 0x", from);
      PRINT(": ", (char*)msgIn);
    } else {
      PRINTS("\nNo reply, is rf95_reliable_datagram_server running?\n");
    }
  } else {
    PRINTS("\nsendtoWait failed");
  }

  delay( 100 );
  SleepStuff();
}

//-------------------------------------- loop() ------------------------------

void loop() {  }
