/* Microbit Kompas demo
 *  Laat een led aan de rand van het display branden en geeft zo het noorden aan.
 *  Kompaskoers 0-360 graden wordt ook in de seriele monitor weergeven.
 *  De middelste LED brandt als het bordje vlak genoeg gehouden wordt voor een foutloze uitlezing.
 *  
 * Let op: Installeer de volgende library: 	STM32duino LSM303AGR door AST
 * 
 *  Voor meer info zie:
 *  https://learn.adafruit.com/lsm303-accelerometer-slash-compass-breakout/calibration
 *  en https://learn.adafruit.com/calibrating-sensors/two-point-calibration
 *  
 *  Gert den Neijsel, Haagse Hogeschool, december 2019
 */

#include <Adafruit_Microbit.h>
#include <LSM303AGR_ACC_Sensor.h>
#include <LSM303AGR_MAG_Sensor.h>

#define DEV_I2C Wire1  //Define which I2C bus is used. Wire1 for the Arduino Due
LSM303AGR_ACC_Sensor Acc(&DEV_I2C);
LSM303AGR_MAG_Sensor Mag(&DEV_I2C);
Adafruit_Microbit microbit;

// Het getal Pi voor berekeningen
const float Pi = 3.14159;

const int vlak = 160;  // tbv het horizontaal (vlak) houden van het kompas (accelerometer waardes).

int32_t accelerometer[3];
int32_t magnetometer[3];
int _x[16] = { 2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3 };  // coordinaten van leds in de matrix
int _y[16] = { 0, 0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3, 2, 1, 0, 0 };

int32_t x, MagMinX, MagMaxX;
int32_t y, MagMinY, MagMaxY;
int32_t z, MagMinZ, MagMaxZ;
boolean gecalibreerd = false;
long lastDisplayTime;
float kompasKoers;

void setup(void) {
  // Start LED matrix driver
  microbit.begin();
  microbit.matrix.clear();  // scherm leegmaken

  // Initialiseer I2C bus.
  DEV_I2C.begin();

  // Initlialize components.
  Acc.begin();
  Acc.Enable();
  Acc.EnableTemperatureSensor();
  Mag.begin();
  Mag.Enable();

  Serial.begin(9600);
  pinMode(PIN_BUTTON_A, INPUT);
  Serial.println("LSM303 Calibratie wordt gestart");
  Serial.println("Draai de microbit in alle richtingen om zijn as totdat de waardes niet meer veranderen.");
  Serial.println("Druk dan op Knop A om calibratie te beeindigen.");
  // Je kunt de bij jouw Microbit gevonden MagMin en MaxMax waardes ook als constantes in je programma opnemen.
  // Dan hoef je niet telkens te calibreren.
  Serial.println("");
  lastDisplayTime = millis();
}

void loop(void) {
  Mag.GetAxes(magnetometer);   // voor het kompas
  Acc.GetAxes(accelerometer);  // deze alleen om te checken of het kompas vlak gehouden wordt.
  x = magnetometer[0];
  y = magnetometer[1];
  z = magnetometer[2];

  if (!digitalRead(PIN_BUTTON_A)) {
    gecalibreerd = true;
    Serial.println();
    Serial.println("Calibratie is beeindigd...");
    delay(1000);
  }

  if (!gecalibreerd) {
    if (x < MagMinX) MagMinX = x;
    if (x > MagMaxX) MagMaxX = x;

    if (y < MagMinY) MagMinY = y;
    if (y > MagMaxY) MagMaxY = y;

    if (z < MagMinZ) MagMinZ = z;
    if (z > MagMaxZ) MagMaxZ = z;
  }

  // de gecalibreerde waardes omzetten naar een verwacht bereik
  x = map(x, MagMinX, MagMaxX, -1023, 1023);
  y = map(y, MagMinY, MagMaxY, -1023, 1023);
  z = map(z, MagMinZ, MagMaxZ, 0, 1023);

  if ((millis() - lastDisplayTime) > 1000)  // Eenmaal per seconde weergeven
  {
    if (!gecalibreerd) {
      Serial.print("Mag Minimums: ");
      Serial.print(MagMinX);
      Serial.print("  ");
      Serial.print(MagMinY);
      Serial.print("  ");
      Serial.print(MagMinZ);
      Serial.println();
      Serial.print("Mag Maximums: ");
      Serial.print(MagMaxX);
      Serial.print("  ");
      Serial.print(MagMaxY);
      Serial.print("  ");
      Serial.print(MagMaxZ);
      Serial.println();
      Serial.println();
    }
    lastDisplayTime = millis();

    // Hoek berekenen van de vector y,x
    kompasKoers = (atan2(y, x) * 180) / Pi;
    kompasKoers += 90;  // Ik ben er niet uit waarom, maar om op de juiste kompaskoers uit te komen moet ik er 90 graden bij optellen.

    // Normaliseren op 0-360
    if (kompasKoers < 0) {
      kompasKoers = 360 + kompasKoers;
    }
    Serial.println(kompasKoers);
  }

  if ((abs(accelerometer[0]) < vlak) && (abs(accelerometer[1]) < vlak)) {  //kompas wordt voldoende vlak gehouden.
    microbit.matrix.clear();                                               // scherm leegmaken
    int index = kompasKoers / 22.5;
    microbit.matrix.drawPixel(_x[index], _y[index], LED_ON);
    microbit.matrix.drawPixel(2, 2, LED_ON);
  } else {                    // weergave uitzetten als kompas niet vlak is
    microbit.matrix.clear();  // scherm leegmaken
  }
}
