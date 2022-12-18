/* MicrobitV2_Radio_guess_distance
This example shows how you can "approximately" determine the distance based on the signal strength of the Microbit.
    Note: this is far from accurate! The position of the Microbit and objects in the environment
    affect the signal strength of the radio waves through attenuation and reflection.

    Upload this code to the microbits of the other members of your team.
    Press button A on one Microbit and button B on the other and see what happens!

    NOTE: This is not Bluetooth, this example only uses Bluetooth radio capabilities.

   This version: Gert den Neijsel, Haagse Hogeschool, December 2022
*/

#include "NRF52_Radio_library.h"
NRF52_Radio MicrobitRadio = NRF52_Radio();  // Replace 52 by 51 everywhere if you use a Microbit V1 (nRF51822) instead of a Microbit V2 (nRF52833)

const int POWER = 7;             // (0..7)   Set transmit power for Bluetooth radio (other setting seems to make no difference).
const int GROUPCODE = 10;        // (0..255) Group code for Bluetooth radio. Make sure all participating Microbits use the same (unique) code!
const int FREQUENCYBAND = 50;    // (0..100) Frequency group for Bluetooth radio. Make sure all participating Microbits use the same (unique) code!
const int RssiAtOneMeter = -52;  // reception strength at 1 m distance in free space; note, this is a negative number! (and a bit of a gambled value).
const float N = 2.0f;            // (2..4)  Environment-dependent constant (you can also "tweak" it, i.e. it's all rather imprecise)
FrameBuffer* myDataSendData;     // Here the data is in (when sending) or the data comes in (when receiving).

void setup() {  // set a number of things once at startup.
  Serial.begin(9600);
  Serial.println("nRF5 Radio Library is starting.");

  myDataSendData = new FrameBuffer();
  MicrobitRadio.setTransmitPower(POWER);
  MicrobitRadio.hello("Test");
  MicrobitRadio.enable();  // Turn on Radio
  MicrobitRadio.setGroup(GROUPCODE);
  MicrobitRadio.setFrequencyBand(FREQUENCYBAND);
}

// The line below (the empty function) is called 'forward declaration', I use this for readability.
// The actual function is below the loop() block.
float determineDistance(String id);

void loop() {  // the main program, this block is to run continuously.
  float distance = determineDistance();

  // Commented the block below, but you'd like to do something with it
  // if (distance > 0) { // if data is received, print the data
  // Serial.println(distance);
  // }

  delay(10);
}


float determineDistance() {  // was float determineDistance(String id)
  String id;
  float distance = 0;
  int rssi = 0;
  FrameBuffer* myData = MicrobitRadio.recv();
  String received = "";  // Create empty String
  if (myData != NULL) {
    if (myData->length - 3 == 1) {  // if true, i received 1 character.
      id = String(received + (char)myData->payload[0]);
      //if (String (received + (char)myData->payload[0]) == id) { // When true, is the recieved charakter equal to 'id'?
      rssi = myData->rssi;
      // https://iotandelectronics.wordpress.com/2016/10/07/how-to-calculate-distance-from-the-rssi-value-of-the-ble-beacon/
      //distance in meters = pow(10 * N, (RssiAtOneMeter - ReceivedRSSI) / 20)
      distance = pow(10.0f * N, (RssiAtOneMeter - rssi) / 20.0f);

      // If you're actually going to use this function you have to comment out all Serial.print lines:
      Serial.print("Microbit \"");
      Serial.print(id);
      Serial.print("\", RSSI:");  // Received Signal Strength Indication
      Serial.print(rssi);
      Serial.print("dBm, distance:");
      Serial.print(distance);
      Serial.println("m");
      //}
    }
    delete myData;
  }
  return distance;
}
