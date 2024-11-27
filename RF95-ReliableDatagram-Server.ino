// M0-RFM95-Central-Min (server) Minimalist version for debugging 
//
// Adafruit reference:  https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module/overview
//
// Class reference:     http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html
//
// Examples:            http://www.airspayce.com/mikem/arduino/RadioHead/examples.html
//
// Central station which receives messages from sensors
//  - receive and ack message from satellite(s)
//  - display message on OLED
//
// Hardware:  Adafruit Feather M0 RFM95 LoRa
//
// Software:  Arduino IDE 2.3.3 (also tested with 1.8.x with same result)
//            RadioHead library  1.122.1
//
// 241110 - testing with PIR satellites - problems...
//        - satellite reboots on trigger and always starts with ID = 1 which produces duplicate message error
//        - using #define RH_ENABLE_EXPLICIT_RETRY_DEDUP 1 doesn't fix it
//        - ignore errors on manager.recvfromAck() and do my own error checking in later versions
//
// 241124 - further testing to find why receivefromAck() variables are getting corrupted
// 241126 - use getter fns manager.headerFrom(), etc to get correct info and get a good senttoWait()
//

#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#define ADDR_CENTRAL     100
#define ADDR_SATELLITE1  101
#define ADDR_SATELLITE2  102

RH_RF95 radio(8,3);     // setup for Adafruit Feather M0 RFM95
RHReliableDatagram manager(radio, ADDR_CENTRAL);

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.print(__DATE__);  Serial.println( __FILE__);

  //------ radio setup
  if( !manager.init()){
    Serial.println("Radio/manager init failed.");
  }
  radio.setFrequency( 915.0 );
}

uint8_t msgIn[RH_RF95_MAX_MESSAGE_LEN];   // incoming data = max len 251 (250 + NULL)
uint8_t msgOut[RH_RF95_MAX_MESSAGE_LEN];  // outgoing data

void loop() {

  if (manager.available()) {

    uint8_t len = sizeof(msgIn);
    uint8_t from, getFrom;
    uint8_t result;
    uint8_t toId;
    uint8_t msgId;
    uint8_t flags;

    Serial.println("Manager is available.");
    // Wait for a message addressed to us from the client

    result = manager.recvfromAck(msgIn, &len, &from, &toId, &msgId, &flags);
    Serial.print("Result: "); Serial.println(result);
    if(result==0 || result==1) {    // ignore errors because of dupe IDs
      Serial.print("got request from unit#: ");
      Serial.print(from, DEC);
      Serial.print(" Data:");
      Serial.println((char*)msgIn);

      // Send a reply back to the originator client
//      if (!manager.sendtoWait(msgOut, strlen((char*)msgOut), from )) {
      if (!manager.sendtoWait(msgOut, strlen((char*)msgOut), manager.headerFrom() )) { // use this! 
        Serial.println("sendtoWait failed");
      }
    
    } else {
      Serial.println("Receive failed");
    }
    
    Serial.print("--msg:"); Serial.println((char*)msgIn);
    Serial.print("--len:"); Serial.println(len);
    Serial.print("--from:");Serial.print(from);   Serial.print("  GetFrom:");Serial.println(manager.headerFrom());
    Serial.print("--to:");Serial.print(toId);     Serial.print("  GetTo:");Serial.println(manager.headerTo());
    Serial.print("--ID:");Serial.print(msgId);    Serial.print("  GetId:");Serial.println(manager.headerId());
    Serial.print("--flags:");Serial.print(flags); Serial.print("  GetFlags:");Serial.println(manager.headerFlags(),HEX);
  }
}


