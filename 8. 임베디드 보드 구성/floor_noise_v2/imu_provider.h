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

  // 무한 반복하며 가속도계 값을 측정한다. 가속도값(x,y,z)를 측정한 뒤
  // current_gyroscope_data 배열에 저장한 뒤 저장된 갯수를 new_gyroscope_samples에 저장한다.
  void ReadGyroscope(int* new_gyroscope_samples) {
    // Keep track of whether we stored any new data
    *new_gyroscope_samples = 0;
    // Loop through new samples and add to buffer
    while (IMU.gyroscopeAvailable()) {
      // gyroscope_data_index는 버퍼 중에서 마지막으로 입력된 데이터의 index이다.
      const int gyroscope_index = (gyroscope_data_index % gyroscope_data_length);
      gyroscope_data_index += 3;
      // 새로 입력 가능한 위치에 대한 주소값을 가져와 값을 저장한다.
      float* current_gyroscope_data = &gyroscope_data[gyroscope_index];
      // Read each sample, removing it from the device's FIFO buffer
      if (!IMU.readGyroscope(
          current_gyroscope_data[0], current_gyroscope_data[1], current_gyroscope_data[2])) {
        Serial.println("Failed to read gyroscope data");
        break;
      }
      // 새로 입력한 값(x,y,z)의 개수를 측정
      *new_gyroscope_samples += 1;
      delay(10);
      Serial.println("inside loop");
    }
  }

  void collectIMUData(int (*accelData)[100][3]) {
    unsigned long lastTime = 0; // 마지막 측정 시간을 저장할 변수
    Serial.println("Collecting data...");

    for (int i = 0; i < 100; i++) {
      // 현재 시간이 마지막 측정 시간으로부터 0.01초가 지났는지 확인
      while (millis() - lastTime < 10) {}

      // 마지막 측정 시간 업데이트
      lastTime = millis();

      // 가속도 측정
      if (IMU.accelerationAvailable()) {
        float x, y, z;
        int val;
        IMU.readAcceleration(x, y, z);
        val = (int)(x*64);
        (*accelData)[i][0] = val < -128 ? -127 : val > 127 ? 127 : val;
        val = (int)(y*64);
        (*accelData)[i][1] = val < -128 ? -127 : val > 127 ? 127 : val;
        val = (int)(z*64);
        (*accelData)[i][2] = val < -128 ? -127 : val > 127 ? 127 : val;
      }
    }
  }

  void getSensorData(int8_t arr[300]) {
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
        int8_t val;
        IMU.readAcceleration(x, y, z);
        val = (int8_t)(x*64);
        arr[i] = val < -128 ? -127 : val > 127 ? 127 : val;
        val = (int8_t)(y*64);
        arr[i + 1] = val < -128 ? -127 : val > 127 ? 127 : val;
        val = (int8_t)(z*64);
        arr[i + 2] = val < -128 ? -127 : val > 127 ? 127 : val;
      }
    }
  }
}