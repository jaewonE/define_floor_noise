#include <Arduino_LSM9DS1.h>

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate()); // 104 Hz
  Serial.println(" Hz");
}

void loop() {
  float x, y, z;
  if (!IMU.accelerationAvailable()) {
    return;
  }

  IMU.readAcceleration(x, y, z);

  Serial.print(x);
  Serial.print(',');
  Serial.print(y);
  Serial.print(',');
  Serial.println(z);

  delay(10); // 0.01
}
