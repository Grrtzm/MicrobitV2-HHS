/* 
  This example shows how you can send and receive data with the Microbit with acknowledgment of receipt.
  Upload the code to the microbits of all members of your team. Press the buttons and see what happens!
  
  This version: Gert den Neijsel, Haagse Hogeschool, December 2022

   Original version:
    Name:       PingPong_example.ino
    Created:	10/12/2018 08.35.50
    Author:     RAHR\michael
*/

#include <Adafruit_Microbit.h>
#include "NRF52_Radio_library.h"

Adafruit_Microbit_Matrix matrix;
NRF52_Radio MicrobitRadio = NRF52_Radio();  // Replace 52 by 51 everywhere if you use a Microbit V1 (nRF51822) instead of a Microbit V2 (nRF52833)

const int POWER = 7;           // (0..7)   Set transmit power for Bluetooth radio (other setting seems to make no difference).
const int GROUPCODE = 10;      // (0..255) Group code for Bluetooth radio. Make sure all participating Microbits use the same (unique) code!
const int FREQUENCYBAND = 50;  // (0..100) Frequency group for Bluetooth radio. Make sure all participating Microbits use the same (unique) code!

enum STATENAME {
  START,
  SEND,
  RECV,
  RECVACT,
  SENDACK,
  WAITTOSEND
};

// Define Function Prototypes that use User Types below here or use a .h file
//
FrameBuffer* myDataSendData;  //FrameBuffer for sending data to another device
FrameBuffer* myReceivedData;

int current_state;                //Var to hold the current state
static long currentMillis;        //Var to store the time gone since last time
const long interval = 5000;       //Wait time before sending
const long start_time = 10000;    //Wait time doing startup, we use this time to see if any other device is already running
const long send_interval = 1000;  //In state send, after start we send out at 1 sec interval, ontil we start receiving something
const long wait_to_send = 100;
bool got_data = false;

// The setup() function runs once each time the micro-controller starts
void setup() {
  Serial.begin(9600);
  Serial.println("Starting the PingPong example using Group 10, frequency band 50");
  matrix.begin();
  matrix.show(matrix.HEART);
  MicrobitRadio.setTransmitPower(POWER);
  MicrobitRadio.enable();  // Turn radio on
  MicrobitRadio.setGroup(GROUPCODE);
  MicrobitRadio.setFrequencyBand(FREQUENCYBAND);
  Serial.println("Radio running");

  current_state = STATENAME::START;

  myDataSendData = new FrameBuffer();
  currentMillis = millis();  // For explanation, look at Examples -> 01.Digital -> BlinkWithoutDelay
  // and http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
}

// Add the main program code into the continuous loop() function
void loop() {
  if ((MicrobitRadio.dataReady() > 0) && (got_data != true)) {  //Check if there are any data ready for use
    myReceivedData = MicrobitRadio.recv();                      //If so then let's get it
    got_data = true;
  }
  switch (current_state) {
    case STATENAME::START:
      {

        if (got_data == true) {
          current_state = STATENAME::RECV;  //If we got data then we are not the first one, we change state to recv.
          got_data == false;                //reset the data flag;
        }

        if (millis() - currentMillis >= start_time) {  // This block is executed after 10000 mSec. For explanation take a look at http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
          current_state = STATENAME::SEND;             // We did not find any signal, so now we assume that we are the first one running
          currentMillis = millis();                    // so we start to send, and then wait for a ack.
          Serial.println("State go to SEND");
        }
      };
      break;

    case STATENAME::RECV:
      {

        if (got_data == true) {
          if (myReceivedData->group == 2) {
            Serial.println("Got Data");
            String received = "";                                       // Create empty String
            for (uint8_t i = 0; i < myReceivedData->length - 3; i++) {  // -3 because an empty framebuffer is 3 bytes in size
              received = String(received + (char)myReceivedData->payload[i]);
            }
            Serial.print("Received data : ");
            Serial.println(received);
            current_state = STATENAME::SENDACK;  // If we got data then we are not the first one, we change state to recv.
          }

          delete myReceivedData;
          got_data = false;
        }
      };
      break;

    case STATENAME::SENDACK:
      {
        Serial.println("Send ACK");
        matrix.print("A");
        MicrobitRadio.send(myDataSendData);
        myDataSendData->length = 3;
        myDataSendData->group = 1;  //(1=ACK 2=SEND)
        myDataSendData->version = 10;
        myDataSendData->protocol = 10;
        MicrobitRadio.send(myDataSendData);
        current_state = STATENAME::WAITTOSEND;
        currentMillis = millis();
      }
      break;

    case STATENAME::RECVACT:
      {
        current_state = STATENAME::RECV;
      };
      break;

    case STATENAME::WAITTOSEND:
      {
        if (millis() - currentMillis >= wait_to_send) {
          Serial.println("State go to SEND");
          current_state = STATENAME::SEND;
        }
      };
      break;

    case STATENAME::SEND:
      {

        if (got_data == true) {
          if (myReceivedData->group == 1) {
            current_state = STATENAME::RECV;  //If we got data then we are not the first one, we change state to recv.
            Serial.println("Got ACK");
          }
          delete myReceivedData;
          got_data = false;  //reset the data flag;
        }
        if (millis() - currentMillis >= send_interval) {
          Serial.println("State = SEND");
          current_state = STATENAME::SEND;  //We did not find any signal, so now we assume that we are the first one running
          currentMillis = millis();         //so we start to send, and then wait for a ack.
          matrix.print("S");

          String bericht = "Radio 01 reporting for duty!";
          myDataSendData->length = 3 + bericht.length();
          myDataSendData->group = 2;  //(1=ACK 2=SEND)
          myDataSendData->version = 10;
          myDataSendData->protocol = 10;

          for (uint8_t i = 0; i < bericht.length(); i++) {
            myDataSendData->payload[i] = bericht.charAt(i);
          }

          MicrobitRadio.send(myDataSendData);
        }
      };
      break;


    default:
      break;
  }
}
