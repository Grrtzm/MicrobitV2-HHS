/* MicrobitV2_Radio_afstand_bepalen
   Dit voorbeeld laat zien hoe je op basis van de signaalsterkte van de Microbit "ongeveer" de afstand kunt bepalen.
   Let op: dit is verre van nauwkeurig! De stand van de Microbit en objecten in de omgeving
   beïnvloeden de signaalsterkte van de radiogolven door demping en reflectie.

   Upload deze code naar de microbits van de andere leden van je team.
   Druk bij de ene Microbit op knop A en bij de andere op knop B en kijk wat er gebeurt!

   LET OP: Dit is geen Bluetooth, dit voorbeeld maakt slechts gebruik van Bluetooth radio mogelijkheden.

   Deze versie: Gert den Neijsel, Haagse Hogeschool, December 2022
*/

#include "NRF52_Radio_library.h"
NRF52_Radio MicrobitRadio = NRF52_Radio();

const int POWER = 7;            // (0..7)   Zendvermogen instellen voor Bluetooth radio (andere instelling lijkt geen verschil te maken).
const int GROEPCODE = 10;       // (0..255) Groepcode voor Bluetooth radio. Zorg dat alle deelnemende Microbits dezelfde (unieke) code gebruiken!
const int FREQUENTIEBAND = 50;  // (0..100) Frequentiegroep voor Bluetooth radio. Zorg dat alle deelnemende Microbits dezelfde (unieke) code gebruiken!
const int RssiAtOneMeter = -52; // ontvangststerkte op 1 m afstand in vrije ruimte; let op, dit is een negatief getal! (en een beetje gegokte waarde).
const float N = 2.0f;           // (2..4)  Omgevingsafhankelijke constante (kun je ook mee "tweaken", oftewel het is nogal onnauwkeurig allemaal)
FrameBuffer *myDataSendData; // Hier staat de data in (bij verzenden) of komt de data in (bij ontvangen).

void setup() { // eenmalig bij het opstarten een aantal dingen instellen.
  Serial.begin(9600);
  Serial.println("nRF5 Radio Library wordt gestart.");

  myDataSendData = new FrameBuffer();
  MicrobitRadio.setTransmitPower(POWER);
  MicrobitRadio.hello("Test");
  MicrobitRadio.enable(); // Radio aanzetten
  MicrobitRadio.setGroup(GROEPCODE);
  MicrobitRadio.setFrequencyBand(FREQUENTIEBAND);
}

// De regel hieronder (de lege functie) heet 'forward declaration', ik gebruik dit voor de leesbaarheid.
// De eigenlijke functie staat onder het loop() blok.
float bepaalAfstand(String id);


void loop() { // het hoofdprogramma, dit blok wordt voordurend doorlopen.
  float afstand = bepaalAfstand();

  // Onderstaand blok uitgecomment, maar hier zou je wat mee willen doen
  //  if (afstand > 0) { // als er data is ontvangen, de data afdrukken
  //    Serial.println(afstand);
  //  }

  delay(10);
}


float bepaalAfstand() { // was float bepaalAfstand(String id)
  String id;
  float afstand = 0;
  int rssi = 0;
  FrameBuffer* myData = MicrobitRadio.recv();
  String ontvangst = ""; // lege String aanmaken
  if (myData != NULL) {
    if (myData->length - 3 == 1) { // zo ja, dan 1 karakter ontvangen.
      id = String (ontvangst + (char)myData->payload[0]);
      //if (String (ontvangst + (char)myData->payload[0]) == id) { // zo ja, is het ontvangen karakter gelijk aan het 'id'?
        rssi = myData->rssi;
        // https://iotandelectronics.wordpress.com/2016/10/07/how-to-calculate-distance-from-the-rssi-value-of-the-ble-beacon/
        //distance in meters = pow(10 * N, (RssiAtOneMeter - ReceivedRSSI) / 20)
        afstand = pow(10.0f * N, (RssiAtOneMeter - rssi) / 20.0f);
  
        // Als je werkelijk deze functie gaat gebruiken moet je alle Serial.print regels uitcommenten:
        Serial.print("Microbit \"");
        Serial.print(id);
        Serial.print("\", RSSI:"); // Received Signal Strength Indication
        Serial.print(rssi);
        Serial.print("dBm, afstand:");
        Serial.print(afstand);
        Serial.println("m");
      //}
    }
    delete myData;
  }
  return afstand;
}
