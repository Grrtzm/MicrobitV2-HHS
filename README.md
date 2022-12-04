# MicrobitV2-HHS Arduino Library
Microbit Version 2 library for first year 'Haagse Hogeschool' (HHS) HBO-ICT aka 'The Hague University of Applied Sciences' (THUAS).

It was built around the [NRF52_Radio_library by aiunderstand](https://github.com/aiunderstand/NRF52_Radio_library).

It was tested on Microbits V2.00 containing nRF52833; the aiunderstand version didn't work. I hardcoded the 52833 includes in the files for it to compile.

Sorry; file names and comments are in Dutch, my students are all Dutch.

Please install the [STM32duino LSM303AGR] (https://reference.arduino.cc/reference/en/libraries/stm32duino-lsm303agr/) library first. Some examples depend on it.

I included some fun examples:
- Motion sensor plotter demo
- Compass
- Temperature sensor based on accerometer
- Lightsensor using matrix display
- Sound demo: Super Mario Bros
- Sound demo: Nokia sound
- Radio demo: simple broadcast transmit and receive
- Radio demo: using handshaking
- Radio demo: 'measure' distance from other radio based on RSSI

Have fun! Gert.

# NRF52_Radio_library

The nrf52_radio library API's are the same as the nrf52_radio ones. With this addition, it is now possible to control the radio of the nrf52's such as the one in the microbit V2. An example can be found to let a Microbit V1 (nrf52) and V2 (nrf52) communicate over radio.

Note: this library is not battletested. Benefit of using simple 2.4 GHz radio is that you dont need to hassle with proprietary (or open source) Bluetooth/BLE SoftDevices which are often also flash memory heavy. Many projects dont need the heavy security and other protocol overhead which often come at a steep learning curve. If your project needs simple send and receive bytes, to one or even multiple devices, this library is useful.

```
#include "NRF52_Radio_library.h"
NRF52_Radio MicrobitRadio = NRF52_Radio();
MicrobitRadio.enable();

SENDING
FrameBuffer *myDataSendData;
uint8_t msg[] = { 0x00, 0x00}; ; //max 32 

myDataSendData = new FrameBuffer();
myDataSendData->length = 5;
myDataSendData->group = 10;
myDataSendData->version = 12;
myDataSendData->protocol = 14;
memcpy(myDataSendData->payload, &msg, sizeof(uint8_t) *2);

MicrobitRadio.send(myDataSendData);
	
RECEIVING
FrameBuffer* myData = MicrobitRadio.recv();
if (myData != NULL) {
	uint8_t data0 = myData->payload[0]; 
	uint8_t data1 = myData->payload[1]; 
	delete myData;
}
```


# NRF52_Radio_library

This library is based on the Driver from Landcaster University microbit-dal Radio implementation.
To use this library you should have the Microbit Board from sanddeepmistry install, you can get that from here
https://sandeepmistry.github.io/arduino-nRF5/package_nRF5_boards_index.json


This library will for now work in 1Mbit simple mode, it is fully function with the runtime also used in the Python and the Java runtime for the microbit.

You have the following function to play with.

* int setTransmitPower(int power); power 0-7
* int setFrequencyBand(int band); band 0-100
* int getRSSI();
* int enable();
* int disable();
* int setGroup(uint8_t group); group 0-255
* int dataReady();
* FrameBuffer* recv();
* int send(FrameBuffer *buffer); frameBuffer struct

```javascript
struct FrameBuffer
{
    uint8_t         length;                             // The length of the remaining bytes in the                  packet. includes protocol/version/group fields, excluding the length field itself.
    uint8_t         version;                            // Protocol version code.
    uint8_t         group;                              // ID of the group to which this packet belongs.
    uint8_t         protocol;                           // Inner protocol number c.f. those issued by IANA for IP protocols

    uint8_t         payload[MICROBIT_RADIO_MAX_PACKET_SIZE];    // User / higher layer protocol data
    FrameBuffer     *next;                              // Linkage, to allow this and other protocols to queue packets pending processing.
    int             rssi;                               // Received signal strength of this frame.
};

```



Example of the use can be found in the example library.


But in general you need to do the following.
```
#include "NRF52_Radio_library.h"

NRF52_Radio MicrobitRadio = NRF52_Radio();
```

In the setup
You can just enable the Radio, the it is ready
MicrobitRadio.enable();


 And then in the loop.
```
static long currentMillis;

//Check if there is any data in the buffer
	FrameBuffer* myData = MicrobitRadio.recv();
	if (myData != NULL) {
		Serial.print(myData->length);
		Serial.print("    ");
		Serial.print(myData->version);
		Serial.print("    ");
		Serial.print(myData->group);
		Serial.print("    ");
		Serial.print(myData->protocol);
		Serial.print("    ");
		Serial.print(myData->payload[10]);
		Serial.print("    ");
		Serial.println(MicrobitRadio.dataReady());
		delete myData;  //Remember to delete it



//Fill in some data in the datastruct
		myDataSendData->length = 3;
		myDataSendData->group = 10;
		myDataSendData->version = 12;
		myDataSendData->protocol = 14;

//send the data each 5 sec
		if (millis() - currentMillis >= interval)
		{

			Serial.println(currentMillis);
			MicrobitRadio.send(myDataSendData);
			currentMillis = millis();
		}
	}

	delay(10);
	digitalWrite(LED, LOW);
	delay(10);
}
```
