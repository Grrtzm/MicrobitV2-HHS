/*
 *  MicrobitV2_Temperature_sensor
 *
 *  The temperature is displayed on the matrix display of the Microbit (v1 or v2).
 *  It is not accurate. The temperature sensor in the accelerometer which will heat up after a while.
 *  The temperature reading is probably 2 to 3 degrees higher than the actual room temperature.
 *
 *  This version: Gert den Neijsel, Haagse Hogeschool, December 2022
*/

#include <LSM303AGR_ACC_Sensor.h>
#include <Adafruit_Microbit.h>

#define DEV_I2C Wire1  //Define which I2C bus is used. Wire1 for the Microbit V2, Wire for the Microbit V1

LSM303AGR_ACC_Sensor Acc(&DEV_I2C);
Adafruit_Microbit microbit;

void setup() {
  Serial.begin(9600);
  microbit.begin();

  // Initialize I2C bus.
  DEV_I2C.begin();

  Acc.begin();
  Acc.Enable();
  Acc.EnableTemperatureSensor();
}

void loop() {
  // Read temperature LSM303AGR.
  float temperature;
  Acc.GetTemperature(&temperature);
  Serial.println(temperature);
  microbit.matrix.print(temperature,1);
  delay(1000);
}
