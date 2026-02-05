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
  constexpr int raster_height = 100;
  constexpr int raster_width = 3;
  constexpr int raster_channels = 1;
  constexpr int raster_byte_count = raster_height * raster_width * raster_channels;
  float raster_buffer[raster_byte_count];

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
  // size: 4 // data: (1,100, 3, 1), type: kTfLiteFloat32
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
  // size: 1 // data: (1,1), type: kTfLiteFloat32
  TfLiteTensor* model_output = interpreter->output(0);
  if ((model_output->dims->size != 1) || (model_output->dims->data[0] != 1) ||
      (model_output->dims->data[1] != label_count) ||
      (model_output->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad output tensor parameters in model");
    return;
  }

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

  getSensorData(raster_buffer);

  // // Pass to the model and run the interpreter
  TfLiteTensor* model_input = interpreter->input(0);
  for (int i = 0; i < raster_byte_count; i++) {
    model_input->data.f[i] = raster_buffer[i];
  }
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
    return;
  }
  TfLiteTensor* output = interpreter->output(0);
  Serial.print("Pred: 층간소음 ");
  Serial.print(output->data.f[0] > output->data.f[1] ? "X (" : "O (");
  Serial.print(output->data.f[0]);
  Serial.print(", ");
  Serial.print(output->data.f[1]); 
  Serial.println(")");

}
