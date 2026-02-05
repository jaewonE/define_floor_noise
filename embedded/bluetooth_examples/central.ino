// // #include <ArduinoBLE.h>

// // const char *deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
// // const char *deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

// // void setup() {
// //   Serial.begin(9600);
// //   while (!Serial)
// //     ;

// //   if (!BLE.begin()) {
// //     Serial.println("* Starting Bluetooth® Low Energy module failed!");
// //     while (1)
// //       ;
// //   }

// //   BLE.setLocalName("Nano 33 BLE (Central)");
// //   BLE.advertise();

// //   Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
// //   Serial.println(" ");
// // }

// // void loop() { connectToPeripheral(); }

// // void connectToPeripheral() {
// //   BLEDevice peripheral;

// //   Serial.println("- Discovering peripheral device...");

// //   do {
// //     BLE.scanForUuid(deviceServiceUuid);
// //     peripheral = BLE.available();
// //   } while (!peripheral);

// //   if (peripheral) {
// //     Serial.println("* Peripheral device found!");
// //     Serial.print("* Device MAC address: ");
// //     Serial.println(peripheral.address());
// //     Serial.print("* Device name: ");
// //     Serial.println(peripheral.localName());
// //     Serial.print("* Advertised service UUID: ");
// //     Serial.println(peripheral.advertisedServiceUuid());
// //     Serial.println(" ");
// //     BLE.stopScan();
// //     controlPeripheral(peripheral);
// //   }
// // }

// // void controlPeripheral(BLEDevice peripheral) {
// //   Serial.println("- Connecting to peripheral device...");

// //   if (peripheral.connect()) {
// //     Serial.println("* Connected to peripheral device!");
// //     Serial.println(" ");
// //   } else {
// //     Serial.println("* Connection to peripheral device failed!");
// //     Serial.println(" ");
// //     return;
// //   }

// //   Serial.println("- Discovering peripheral device attributes...");
// //   if (peripheral.discoverAttributes()) {
// //     Serial.println("* Peripheral device attributes discovered!");
// //     Serial.println(" ");
// //   } else {
// //     Serial.println("* Peripheral device attributes discovery failed!");
// //     Serial.println(" ");
// //     peripheral.disconnect();
// //     return;
// //   }

// //   BLECharacteristic gestureCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);

// //   if (!gestureCharacteristic) {
// //     Serial.println("* Peripheral device does not have gesture_type characteristic!");
// //     peripheral.disconnect();
// //     return;
// //   } else if (!gestureCharacteristic.canWrite()) {
// //     Serial.println("* Peripheral does not have a writable gesture_type characteristic!");
// //     peripheral.disconnect();
// //     return;
// //   }

// //   while (peripheral.connected()) {
// //     int gesture = random(1, 100);
// //     Serial.print("* Writing value to gesture_type characteristic: ");
// //     Serial.println(gesture);
// //     gestureCharacteristic.writeValue((byte)gesture);
// //     Serial.println("* Writing value to gesture_type characteristic done!");
// //     Serial.println(" ");
// //   }
// //   Serial.println("- Peripheral device disconnected!");
// // }
// #include <ArduinoBLE.h>

// void setup() {
// 	Serial.begin(9600);
// 	while (!Serial);
// 	// begin initialization
// 	if (!BLE.begin()) {
// 		Serial.println("starting BLE failed!");
// 		while (1);
// 	}
// 	Serial.println("BLE Central scan");
// 	// start scanning for peripheral
// 	BLE.scan();
// }

// void loop() {
// 	// check if a peripheral has been discovered
// 	BLEDevice peripheral = BLE.available();

// 	if (peripheral) {
// 		// discovered a peripheral
// 		Serial.println("Discovered a peripheral");
// 		Serial.println("-----------------------");

// 		// print address
// 		Serial.print("Address: ");
// 		Serial.println(peripheral.address());

// 		// print the local name, if present
// 		if (peripheral.hasLocalName()) {
// 			Serial.print("Local Name: ");
// 			Serial.println(peripheral.localName());
// 		}

// 		// print the advertised service UUIDs, if present
// 		if (peripheral.hasAdvertisedServiceUuid()) {
// 			Serial.print("Service UUIDs: ");
// 			for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
// 				Serial.print(peripheral.advertisedServiceUuid(i));
// 				Serial.print(" ");
// 			}
// 			Serial.println();
// 		}
// 	}
// }

#include <ArduinoBLE.h>

const int buttonPin = 2;
int oldButtonState = LOW;

void setup() {
	Serial.begin(9600);
	while (!Serial);
	// configure the button pin as input
	pinMode(buttonPin, INPUT);
	// initialize the BLE hardware
	BLE.begin();
	Serial.println("BLE Central - LED control");
	// start scanning for peripherals
	BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
}

void loop() {
	// check if a peripheral has been discovered
	BLEDevice peripheral = BLE.available();

	if (peripheral) {
		// discovered a peripheral, print out address, local name, and advertised service
		Serial.print("Found: ");
		Serial.print(peripheral.address());
		Serial.print(peripheral.localName());
		Serial.println(peripheral.advertisedServiceUuid());

		if (peripheral.localName() != "LED") return;
		// stop scanning
		BLE.stopScan();
		controlLED(peripheral);
		// peripheral disconnected, start scanning again
		BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
	}
}

void controlLED(BLEDevice peripheral) {
	// connect to the peripheral
	Serial.println("Connecting ...");

	if (peripheral.connect()) {
		Serial.println("Connected");
	} else {
		Serial.println("Failed to connect!");
		return;
	}
	// retrieve the LED characteristic
	BLECharacteristic ledCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");

	if (!ledCharacteristic) {
		Serial.println("Peripheral does not have LED characteristic!");
		peripheral.disconnect();
		return;
	} else if (!ledCharacteristic.canWrite()) {
		Serial.println("Peripheral does not have a writable LED characteristic!");
		peripheral.disconnect();
		return;
	}

	while (peripheral.connected()) {
		// read the button pin
		int buttonState = digitalRead(buttonPin);

		if (oldButtonState != buttonState) {
			// button changed
			oldButtonState = buttonState;
			if (buttonState) {
				Serial.println("button pressed");
				ledCharacteristic.writeValue((byte)0x01);
			} else {
				Serial.println("button released");
				ledCharacteristic.writeValue((byte)0x00);
			}
		}
	}
	Serial.println("Peripheral disconnected");
}