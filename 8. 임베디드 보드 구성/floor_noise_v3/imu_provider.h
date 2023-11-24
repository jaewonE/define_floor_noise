#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

namespace {

  constexpr int stroke_transmit_stride = 2;
  constexpr int stroke_transmit_max_length = 160;
  constexpr int stroke_max_length = stroke_transmit_max_length * stroke_transmit_stride;
  constexpr int stroke_points_byte_count = 2 * sizeof(int8_t) * stroke_transmit_max_length;
  constexpr int stroke_struct_byte_count = (2 * sizeof(int32_t)) + stroke_points_byte_count;
  constexpr int moving_sample_count = 50;
  
  static float current_velocity[3] = {0.0f, 0.0f, 0.0f};
  static float current_gravity[3] = {0.0f, 0.0f, 0.0f};
  static float current_gyroscope_drift[3] = {0.0f, 0.0f, 0.0f};
  
  static int32_t stroke_length = 0;
  static uint8_t stroke_struct_buffer[stroke_struct_byte_count] = {};
  static int32_t* stroke_state = reinterpret_cast<int32_t*>(stroke_struct_buffer);
  static int32_t* stroke_transmit_length = reinterpret_cast<int32_t*>(stroke_struct_buffer + sizeof(int32_t));
  static int8_t* stroke_points = reinterpret_cast<int8_t*>(stroke_struct_buffer + (sizeof(int32_t) * 2));

  // A buffer holding the last 600 sets of 3-channel values from the gyroscope.
  constexpr int gyroscope_data_length = 600 * 3;
  float gyroscope_data[gyroscope_data_length] = {};

  // The next free entry in the data array.
  int gyroscope_data_index = 0;
  float gyroscope_sample_rate = 0.0f;

  // 보드 셋업
  void SetupIMU() {
  
    // Make sure we are pulling measurements into a FIFO.
    // If you see an error on this line, make sure you have at least v1.1.0 of the
    // Arduino_LSM9DS1 library installed.
    IMU.setContinuousMode();
  
    gyroscope_sample_rate = IMU.gyroscopeSampleRate();
  }

  void getSensorData(float arr[300]) {
    unsigned long lastTime = 0; // 마지막 측정 시간을 저장할 변수
    Serial.println("Collecting data...");

    for (int i = 0; i < 300; i+=3) {
      // 현재 시간이 마지막 측정 시간으로부터 0.01초가 지났는지 확인
      while (millis() - lastTime < 10) {}

      // 마지막 측정 시간 업데이트
      lastTime = millis();

      // 가속도 측정
      if (IMU.accelerationAvailable()) {
        float x, y, z;
        IMU.readAcceleration(x, y, z);
        arr[i] = x;
        arr[i + 1] = y;
        arr[i + 2] = z;
      }
    }
  }
}