/* Measure light intensity using a Microbit V2
 *  The Microbit has a 5x5 led display.
 *  This program measures light intensity using the middle led.
 *  
 *  Use the Serial Plotter to display readings.
 *  Try it with and without averaging (remove comments from 3rd last line; Serial.println(average);).
 *  
 *  Gert den Neijsel, Haagse Hogeschool, oct-2019
 *  Based on Arduino AnalogReadSerial example code.
 *  See http://www.arduino.cc/en/Tutorial/AnalogReadSerial
 *  This example code is in the public domain.
 */

const int LED = 23;        // 'row 3' led on a Microbit V2
const int COL = 3;         // 'col 3' led on a Microbit V2
const int arraySize = 50;  // The bigger the array, the more smooth, but slower.
int values[arraySize];     // Declare array of above size

// Function calculateAverage() - fill an array with the last 'arraySize' number of values and calculated the average.
int calculateAverage(int value) {
  int sum = value;
  values[0] = value;
  for (int n = arraySize - 1; n > 0; n--) {  // Count from low to high
    // Shift old values forward into the array, start on top:
    values[n] = values[n - 1];
    sum = sum + values[n];  // current value
  }
  return sum / arraySize;  // the result
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
  // Measure the amount of light on the middle led (row3 / col3).
  // Pleas note: by subtracting the measuerd value from 1023 you reverse the behaviour.
  // Low light intensity results in a low value, high intensity results in a high number, exactly as you would expect.
  int sensorValue = 1023 - analogRead(COL);
  int average = calculateAverage(sensorValue);
  Serial.print("Light intensity:");  // label for seriële plotter
  // print out the value you read:
  // Serial.println(average);   // With average, so its slower
  Serial.println(sensorValue);  // Without average, so its faster
  delay(10);                    // Do experiment with the delay (default: 10)
}