/* Microbit Lichtsterkte meten
 *  De Microbit heeft een 5x5 led display.
 *  Met dit programma wordt de lichtsterkte gemeten met de middelste led.
 *  
 *  Gebruik de Serial Plotter om een grafiek weer te geven.
 *  Probeer het eens met- en zonder gemiddelde (comments verwijderen van 2 na onderste regel).
 *  
 *  Gert den Neijsel, Haagse Hogeschool, okt-2019
 *  Gebaseerd op Arduino AnalogReadSerial voorbeeldcode.
 *  Zie http://www.arduino.cc/en/Tutorial/AnalogReadSerial
 *  This example code is in the public domain.
 */

const int LED = 23;           // 'row 3' led
const int COL = 3;            // 'col 3' led
const int arrayGrootte = 50;  // hoe groter het array hoe gelijkmatiger, maar wel trager
int waardes[arrayGrootte];    // array met bovenstaande grootte declareren

// Functie gemiddelde() - array vullen met de laatste 'arrayGrootte' aantal waardes en daar het gemiddelde van bepalen.
int berekenGemiddelde(int waarde) {
  int som = waarde;
  waardes[0] = waarde;
  for (int n = arrayGrootte - 1; n > 0; n--) {  // teller van hoog naar laag
    // oude waardes doorschuiven in het array, bovenaan beginnen:
    waardes[n] = waardes[n - 1];
    som = som + waardes[n];  // huidige waarde bij de som optellen
  }
  return som / arrayGrootte;  // het resultaat (het gemiddelde) teruggeven
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  // Lees de lichtwaarde op de middelste led (row3 / col3).
  // Let op: door de gemeten waarde van 1023 af te trekken wordt het gedrag omgedraaid.
  // Weinig licht geeft nu lage waarde, veel licht geeft hoge waarde, precies zoals je zou verwachten.
  int sensorValue = 1023 - analogRead(COL);
  int gemiddelde = berekenGemiddelde(sensorValue);
  Serial.print("Lichtsterkte:");  // label voor de seriële plotter
  // print out the value you read:
  // Serial.println(gemiddelde); // met gemiddelde, dus trager
  Serial.println(sensorValue);  // zonder gemiddelde, dus snel
  delay(10);                    // experimenteer ook eens met de waarden voor delay (standaard: 10)
}