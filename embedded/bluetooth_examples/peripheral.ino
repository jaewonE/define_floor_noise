// #include <ArduinoBLE.h>

// const char *deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
// const char *deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

// int gesture = -1;

// BLEService gestureService(deviceServiceUuid);
// BLEByteCharacteristic gestureCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);

// void setup() {
//   Serial.begin(9600);
//   while (!Serial)
//     ;

//   pinMode(LEDR, OUTPUT);
//   pinMode(LEDG, OUTPUT);
//   pinMode(LEDB, OUTPUT);
//   pinMode(LED_BUILTIN, OUTPUT);

//   digitalWrite(LEDR, HIGH);
//   digitalWrite(LEDG, HIGH);
//   digitalWrite(LEDB, HIGH);
//   digitalWrite(LED_BUILTIN, LOW);

//   if (!BLE.begin()) {
//     Serial.println("- Starting Bluetooth® Low Energy module failed!");
//     while (1)
//       ;
//   }

//   BLE.setLocalName("Arduino Nano 33 BLE (Peripheral)");
//   BLE.setAdvertisedService(gestureService);
//   gestureService.addCharacteristic(gestureCharacteristic);
//   BLE.addService(gestureService);
//   gestureCharacteristic.writeValue(-1);
//   BLE.advertise();

//   Serial.println("Nano 33 BLE (Peripheral Device)");
//   Serial.println(" ");
// }

// void loop() {
//   BLEDevice central = BLE.central();
//   Serial.println("- Discovering central device...");
//   // delay(500);

//   if (central) {
//     Serial.println("* Connected to central device!");
//     Serial.print("* Device MAC address: ");
//     Serial.println(central.address());
//     Serial.println(" ");

//     while (central.connected()) {
//       if (gestureCharacteristic.written()) {
//         gesture = gestureCharacteristic.value();
//         writeGesture(gesture);
//       }
//     }

//     Serial.println("* Disconnected to central device!");
//   }
// }

// void writeGesture(int gesture) {
//   Serial.println("- Characteristic <gesture_type> has changed!");
//   Serial.print("* New value: ");
//   Serial.println(gesture);

//   for (int i = 0; i < 3; i++) {
//     digitalWrite(LEDR, LOW);
//     digitalWrite(LEDG, HIGH);
//     digitalWrite(LEDB, HIGH);
//     digitalWrite(LED_BUILTIN, LOW);
//     delay(100);
//   }
// }

#include <ArduinoBLE.h>

const int ledPin = LED_BUILTIN;
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
	Serial.begin(9600);
	while (!Serial);

	pinMode(ledPin, OUTPUT); // use the LED pin as an output

	// begin initialization
	if (!BLE.begin()) {
		Serial.println("starting BLE failed!");
		while (1);
	}
	// set the local name peripheral advertises
	BLE.setLocalName("LEDCallback");
	// set the UUID for the service this peripheral advertises
	BLE.setAdvertisedService(ledService);
	// add the characteristic to the service
	ledService.addCharacteristic(switchCharacteristic);
	// add service
	BLE.addService(ledService);
	// assign event handlers for connected, disconnected to peripheral
	BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
	BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
	// assign event handlers for characteristic
	switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
	// set an initial value for the characteristic
	switchCharacteristic.setValue(0);
	// start advertising
	BLE.advertise();

	Serial.println(("Bluetooth device active, waiting for connections..."));
}

void loop() {
	// poll for BLE events
	BLE.poll();
}

void blePeripheralConnectHandler(BLEDevice central) {
	// central connected event handler
	Serial.print("Connected event, central: ");
	Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
	// central disconnected event handler
	Serial.print("Disconnected event, central: ");
	Serial.println(central.address());
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
	// central wrote new value to characteristic, update LED
	Serial.print("Characteristic event, written: ");

	if (switchCharacteristic.value()) {
		Serial.println("LED on");
		digitalWrite(ledPin, HIGH);
	} else {
		Serial.println("LED off");
		digitalWrite(ledPin, LOW);
	}
}