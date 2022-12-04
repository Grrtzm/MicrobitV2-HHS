/* 
   Dit voorbeeld laat zien hoe je data kunt verzenden en ontvangen met de Microbit.
   Upload de code naar de microbits van alle leden van je team. Druk op de knopjes en kijk wat er gebeurt!

   Deze versie: Gert den Neijsel, Haagse Hogeschool, December 2022

   Originele versie:
   Name:		NRF_Radio.ino
   Created:	11/30/2018 2:40:19 AM
   Author:	michael
*/

#include "NRF52_Radio_library.h"
#include <Adafruit_Microbit.h>

Adafruit_Microbit_Matrix matrix;  // alleen nodig voor het matrix display

NRF52_Radio MicrobitRadio = NRF52_Radio();

const int POWER = 7;            // (0..7)   Zendvermogen instellen voor Bluetooth radio (andere instelling lijkt geen verschil te maken).
const int GROEPCODE = 10;       // (0..255) Groepcode voor Bluetooth radio. Zorg dat alle deelnemende Microbits dezelfde (unieke) code gebruiken!
const int FREQUENTIEBAND = 50;  // (0..100) Frequentiegroep voor Bluetooth radio. Zorg dat alle deelnemende Microbits dezelfde (unieke) code gebruiken!

FrameBuffer* myDataSendData;  // Hier staat de data in (bij verzenden) of komt de data in (bij ontvangen).

void setup() {  // eenmalig bij het opstarten een aantal dingen instellen.
  Serial.begin(9600);
  Serial.println("nRF52 Radio Library wordt gestart.");
  matrix.begin();
  pinMode(PIN_BUTTON_A, INPUT);
  pinMode(PIN_BUTTON_B, INPUT);

  myDataSendData = new FrameBuffer();
  MicrobitRadio.setTransmitPower(POWER);
  MicrobitRadio.hello("Test");
  MicrobitRadio.enable();  // Radio aanzetten
  MicrobitRadio.setGroup(GROEPCODE);
  MicrobitRadio.setFrequencyBand(FREQUENTIEBAND);
}

// De twee regels hieronder (de lege functies) heten 'forward declarations', ik gebruik dit voor de leesbaarheid.
// De eigenlijke functies staan onder het loop() blok.
void verzenden(String bericht);
String ontvangen();


void loop() {  // het hoofdprogramma, dit blok wordt voordurend doorlopen.

  String ontvangst = ontvangen();  // elke lusdoorgang kijken of er data is ontvangen
  if (ontvangst.length() > 0) {    // als er data is ontvangen, de data afdrukken
    Serial.println(ontvangst);
  }

  // Voor de demo geen antidender of statechange detection gebruikt, dus nu veel dubbele regels bij het knopjes uitlezen.
  if (!digitalRead(PIN_BUTTON_A)) {
    verzenden("A");
  }
  if (!digitalRead(PIN_BUTTON_B)) {
    verzenden("B");
  }

  // Ontvangen data weergeven op het matrix display.
  if (ontvangst == "A") {
    matrix.print("A");
  } else if (ontvangst == "B") {
    matrix.print("B");
  } else {
    matrix.clear();  // display leegmaken
  }

  // Verzenden en ontvangen werkt momenteel alleen met Strings. Als je een ander datatype wilt gebruiken, dan moet je dat eerst omzetten.
  // Omzetting die je nodig kunt hebben bij verzenden; van float naar String:
  //    float f = 123.456; // voorbeeld met float, dit kun je ook met integer of char doen.
  //    String mystring;
  //    mystring = String(f); // hier wordt de float naar String omgezet, dit kun je ook met integer of char doen.
  // Omzetting die je nodig kunt hebben bij ontvangen; van String naar float;
  //    Serial.println(ontvangst.toFloat());
  // voor alle andere mogelijkheden zie https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/

  if (!digitalRead(PIN_BUTTON_A) && !digitalRead(PIN_BUTTON_B)) {  // Beide knoppen tegelijk indrukken.
    verzenden("Daar was laatst een meisje loos!");                 // De maximale berichtlengte is 29 bytes (packet size max 32 bytes -3 voor de header)
  }
  delay(100);
}


String ontvangen() {
  FrameBuffer* myData = MicrobitRadio.recv();
  String ontvangst = "";  // lege String aanmaken
  if (myData != NULL) {
    for (uint8_t i = 0; i < myData->length - 3; i++) {  // -3 want een lege framebuffer is 3 bytes groot
      ontvangst = String(ontvangst + (char)myData->payload[i]);
    }
    delete myData;
  }
  return ontvangst;
}

void verzenden(String bericht) {
  bericht = bericht.substring(0, 30);             // beperken tot maximale lengte van 29 bytes anders krijg je rommel
  myDataSendData->length = bericht.length() + 3;  // +3 want een lege framebuffer is 3 bytes groot
  myDataSendData->group = GROEPCODE;
  myDataSendData->version = 12;
  myDataSendData->protocol = 14;

  for (uint8_t i = 0; i < bericht.length(); i++) {
    myDataSendData->payload[i] = bericht.charAt(i);
  }

  MicrobitRadio.send(myDataSendData);
}
