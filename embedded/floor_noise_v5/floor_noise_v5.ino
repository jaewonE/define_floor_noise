#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "floor_noise_model_data.h"
#include "imu_provider.h"

#define BLE_SENSE_UUID(val) ("4798e0f2-" val "-4d68-af64-8a8f5258404e")

namespace {

  const int VERSION = 0x00000000;

  // Constants for image rasterization
  constexpr int raster_height = 128;
  constexpr int raster_width = 3;
  constexpr int raster_channels = 1;
  constexpr int raster_byte_count = raster_height * raster_width * raster_channels;
  int8_t raster_buffer[raster_byte_count];

  // BLE settings
  BLEService        service                       (BLE_SENSE_UUID("0000"));
  BLECharacteristic strokeCharacteristic          (BLE_SENSE_UUID("300a"), BLERead, stroke_struct_byte_count);
  
  // String to calculate the local and device name
  String name;
  
  // Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
  constexpr int kTensorArenaSize = 30 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
  
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  
  constexpr int label_count = 2;
  const char* labels[label_count] = {"0", "1"};

  // 1.28초에 한번씩 노이즈 여부를 측정하며 현재 측정 시점을 기준으로
  // noise_window_size 내에 noise_count_threshold 이상의 노이즈가 
  // 감지될 경우 층간소음으로 판단하여 LED를 점등한다.
  constexpr int noise_window_size = 10;
  constexpr int noise_count_threshold = 2;
  int noise_index = 0;  // 현재 윈도우 인덱스
  bool noise_window[noise_window_size] = {false};

  // 움직임이 있는지 정의하는 기준값.
  constexpr int8_t move_threshold = 5;

}  // namespace

void setup() {
  // Start serial
  Serial.begin(9600);
  Serial.println("Started");
  randomSeed(42);

  // Start IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialized IMU!");
    while (1);
  }
  SetupIMU();

  // Start BLE
  if (!BLE.begin()) {
    Serial.println("Failed to initialized BLE!");
    while (1);
  }
  String address = BLE.address();

  // Output BLE settings over Serial
  Serial.print("address = ");
  Serial.println(address);

  address.toUpperCase();

  name = "BLESense-";
  name += address[address.length() - 5];
  name += address[address.length() - 4];
  name += address[address.length() - 2];
  name += address[address.length() - 1];

  Serial.print("name = ");
  Serial.println(name);

  BLE.setLocalName(name.c_str());
  BLE.setDeviceName(name.c_str());
  BLE.setAdvertisedService(service);

  service.addCharacteristic(strokeCharacteristic);

  BLE.addService(service);
  BLE.advertise();

  static tflite::MicroErrorReporter micro_error_reporter;  // NOLINT
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(model_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // 사용되는 레이어의 종류를 선언한다. 선언된 레이어만을 사용하는 것임으로 중복작성은 필요없다.
  // dropout 계층은 추론에서 효과가 없음으로 모델 변환 과정에서 무시된다.(필요없다).
  // https://github.com/tensorflow/tflite-micro/blob/main/tensorflow/lite/micro/micro_mutable_op_resolver.h#L421
  static tflite::MicroMutableOpResolver<5> micro_op_resolver; 
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddReshape();              
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddSoftmax();
  // micro_op_resolver.AddLogistic(); 
  // static tflite::ops::micro::AllOpsResolver micro_op_resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  interpreter->AllocateTensors();

  // Set model input settings
  // size: 4 // data: (1,100, 3, 1), type: kTfLiteInt8
  TfLiteTensor* model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != raster_height) ||
      (model_input->dims->data[2] != raster_width) ||
      (model_input->dims->data[3] != raster_channels) ||
      (model_input->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }

  // Set model output settings
  // size: 2 // data: (1,2), type: kTfLiteInt8
  TfLiteTensor* model_output = interpreter->output(0);
  if ((model_output->dims->size != 2) || (model_output->dims->data[0] != 1) ||
      (model_output->dims->data[1] != label_count) ||
      (model_output->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad output tensor parameters in model");
    return;
  }
// 16 6
}

void resetWindow() {
  for(int i=0; i<noise_window_size; i++) {
    noise_window[i] = false;
  }
}

bool hasMoved(int8_t arr[384]) {
  int abs_sum = 0;
  for(int i=2; i<384;i+=3) {
    abs_sum = arr[i] < 0 ? -1 * arr[i] : arr[i];
  }
  // Serial.print("abs_sum: ");
  Serial.print("[");
  Serial.print(abs_sum);
  Serial.print("]");
  return abs_sum >= move_threshold;
}

void loop() {
  BLEDevice central = BLE.central();
  
  // if a central is connected to the peripheral:
  static bool was_connected_last = false;  
  if (central && !was_connected_last) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
  }
  was_connected_last = central;

  // make sure IMU data is available then read in data
  const bool data_available = IMU.gyroscopeAvailable();
  if (!data_available) {
    return;
  }

  getSensorData2(raster_buffer);
  // for(int i=1; i<=384;i++) {
  //   Serial.print(raster_buffer[i-1]);
  //   Serial.print(", ");
  //   if(i % 3 == 0) {
  //     Serial.println();
  //   }
  // }
  bool isFloorNoise = false;
  if(hasMoved(raster_buffer)) {
    // // Pass to the model and run the interpreter
    TfLiteTensor* model_input = interpreter->input(0);
    for (int i = 0; i < raster_byte_count; i++) {
      model_input->data.f[i] = (float)(raster_buffer[i]);
    }
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
      return;
    }
    TfLiteTensor* output = interpreter->output(0);
    isFloorNoise = output->data.f[0] >= output->data.f[1];

    // 상태 출력
    Serial.print("Pred: 층간소음 ");
    Serial.print(isFloorNoise ? "O (" : "X (");
    Serial.print(output->data.f[0]);
    Serial.print(", ");
    Serial.print(output->data.f[1]); 
    Serial.println(")");

  }

  noise_window[noise_index] = isFloorNoise;
  noise_index = (noise_index + 1) % noise_window_size;

  // 층간소음 판단 로직
  int noise_sum = 0;
  // Serial.print("noise_window: ");
  for (int i = 0; i < noise_window_size; i++) {
    // Serial.print(noise_window[i] ? "true, ": "false ");
    if (noise_window[i] == true) {
      noise_sum++;
    }
  }
  // Serial.println();
  Serial.print("누적 층간소음 횟수: ");
  Serial.println(noise_sum);
  Serial.println();


  if (noise_sum >= noise_count_threshold) {
    // 층간소음으로 판단될 경우 0.5초간 점등하기를 5번 반복한다.
    for(int i=0; i<5;i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }
    resetWindow();
  } else {
    // 층간소음이 아님, LED 끄기
    digitalWrite(LED_BUILTIN, LOW);
  }
}
