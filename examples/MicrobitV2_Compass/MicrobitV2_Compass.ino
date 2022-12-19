/* Microbit Compass demo
 *  Displays the north using an led on the edge of the display.
 *  The heading (0-360 degrees) is displayed in the serial monitor.
 *  The middle LED is lit when the Microbit is level; it indicates an accurate reading.
 *  
 *  For more information have a look at:
 *  https://learn.adafruit.com/lsm303-accelerometer-slash-compass-breakout/calibration
 *  and https://learn.adafruit.com/calibrating-sensors/two-point-calibration
 *  
 *  Gert den Neijsel, Haagse Hogeschool, december 2019
 */

#include <Adafruit_Microbit.h>
#include <LSM303AGR_ACC_Sensor.h>
#include <LSM303AGR_MAG_Sensor.h>

#define DEV_I2C Wire1  //Define which I2C bus is used. Wire1 for the Microbit V2. Wire is used for the Microbit V1
LSM303AGR_ACC_Sensor Acc(&DEV_I2C);
LSM303AGR_MAG_Sensor Mag(&DEV_I2C);
Adafruit_Microbit microbit;

// The number Pi for calculations.
const float Pi = 3.14159;

const int leveled = 160;  // In order to keep the compass horizontal (leveled).

int32_t accelerometer[3];
int32_t magnetometer[3];
int _x[16] = { 2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3 };  // Coordinates of leds in the matrix
int _y[16] = { 0, 0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3, 2, 1, 0, 0 };

int32_t x, MagMinX, MagMaxX;
int32_t y, MagMinY, MagMaxY;
int32_t z, MagMinZ, MagMaxZ;
boolean calibrated = false;
long lastDisplayTime;
float heading;

void setup(void) {
  // Start LED matrix driver
  microbit.begin();
  microbit.matrix.clear();  // Clear display

  // Initialiseer I2C bus.
  DEV_I2C.begin();

  // Initilialize components.
  Acc.begin();
  Acc.Enable();
  Acc.EnableTemperatureSensor();
  Mag.begin();
  Mag.Enable();

  Serial.begin(9600);
  pinMode(PIN_BUTTON_A, INPUT);
  Serial.println("LSM303 Calibration started.");
  Serial.println("Twist your microbit around all axes until values are stable.");
  Serial.println("Then press button A to end calibration.");
  // You can copy the MagMin and MagMax values from the serial monitor to your program.
  // In that way you need to calibrate each time.
  // Please note: these calibration values are different and unique for every microbit.
  Serial.println("");
  lastDisplayTime = millis();
}

void loop(void) {
  Mag.GetAxes(magnetometer);   // Used for compass
  Acc.GetAxes(accelerometer);  // This one is only used to check if the compass is leveled.
  x = magnetometer[0];
  y = magnetometer[1];
  z = magnetometer[2];

  if (!digitalRead(PIN_BUTTON_A)) {
    calibrated = true;
    Serial.println();
    Serial.println("Calibration is finished...");
    delay(1000);
  }

  if (!calibrated) {
    if (x < MagMinX) MagMinX = x;
    if (x > MagMaxX) MagMaxX = x;

    if (y < MagMinY) MagMinY = y;
    if (y > MagMaxY) MagMaxY = y;

    if (z < MagMinZ) MagMinZ = z;
    if (z > MagMaxZ) MagMaxZ = z;
  }

  // Change the calibrated values to an expected range.
  x = map(x, MagMinX, MagMaxX, -1023, 1023);
  y = map(y, MagMinY, MagMaxY, -1023, 1023);
  z = map(z, MagMinZ, MagMaxZ, 0, 1023);

  if ((millis() - lastDisplayTime) > 1000)  // Once a second.
  {
    if (!calibrated) {
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

    // Calculate the angle (in degrees) of vector y,x
    heading = (atan2(y, x) * 180) / Pi;
    heading += 90;  // I don't know why, but you need to add 90 degrees to reach the correct heading.

    // Normalize to 0-360 degrees
    if (heading < 0) {
      heading = 360 + heading;
    }
    Serial.println(heading);
  }

  if ((abs(accelerometer[0]) < leveled) && (abs(accelerometer[1]) < leveled)) {  // Compas is sufficiently held leveled (horizontal).
    microbit.matrix.clear();                                                     // Clear display
    int index = heading / 22.5;
    microbit.matrix.drawPixel(_x[index], _y[index], LED_ON);
    microbit.matrix.drawPixel(2, 2, LED_ON);
  } else {                    // Clear display when compass isn't leveled.
    microbit.matrix.clear();  // Clear display
  }
}
