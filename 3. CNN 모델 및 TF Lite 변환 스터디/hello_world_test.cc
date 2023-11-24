/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// C/C++ 내장 헤더 파일
#include <math.h> // 수학 함수를 사용하기 위한 헤더 파일, 예를 들어 sin 함수

// TensorFlow Lite Micro 관련 헤더 파일
#include "tensorflow/lite/core/c/common.h"                     // TensorFlow Lite의 C API에 대한 공통 헤더
#include "tensorflow/lite/micro/micro_interpreter.h"           // 마이크로 인터프리터 헤더
#include "tensorflow/lite/micro/micro_log.h"                   // 로깅을 위한 헤더
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"   // 연산자 해결자 헤더
#include "tensorflow/lite/micro/micro_profiler.h"              // 프로파일러 헤더
#include "tensorflow/lite/micro/recording_micro_interpreter.h" // 기록 중인 마이크로 인터프리터 헤더
#include "tensorflow/lite/micro/system_setup.h"                // 시스템 설정 헤더
#include "tensorflow/lite/schema/schema_generated.h"           // 스키마 헤더

// TensorFlow Lite Micro의 Hello World 예제에 필요한 헤더 파일들
#include "tensorflow/lite/micro/examples/hello_world/models/hello_world_float_model_data.h" // 부동소수점 모델 데이터
#include "tensorflow/lite/micro/examples/hello_world/models/hello_world_int8_model_data.h" // 정수형(8비트) 모델 데이터

namespace { // 시작: 익명 네임스페이스 (이 네임스페이스 내의 식별자는 이 파일 내에서만 접근 가능)

// HelloWorldOpResolver: MicroMutableOpResolver의 별칭(alias)을 정의합니다.
// MicroMutableOpResolver<1>은 하나의 연산자를 등록할 수 있는 클래스입니다.
using HelloWorldOpResolver = tflite::MicroMutableOpResolver<1>;

// RegisterOps 함수: HelloWorldOpResolver 인스턴스에 연산자를 등록합니다.
// 인자: op_resolver: HelloWorldOpResolver 타입의 참조, 연산자를 등록할 대상입니다.
// 반환값: kTfLiteOk: 연산자 등록이 성공적으로 완료되었음을 나타냅니다.
TfLiteStatus RegisterOps(HelloWorldOpResolver &op_resolver) {
  // op_resolver에 완전 연결 레이어(Fully Connected Layer) 연산자를 등록합니다.
  // AddFullyConnected(): 완전 연결 레이어 연산자를 등록하는 메서드입니다.
  TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
  return kTfLiteOk; // 연산자 등록 성공을 나타내는 상수를 반환합니다.
} // RegisterOps 함수 종료

} // namespace

// 메모리와 지연 시간을 프로파일링하는 함수
TfLiteStatus ProfileMemoryAndLatency() {
  tflite::MicroProfiler profiler;                  // 프로파일러 객체 생성
  HelloWorldOpResolver op_resolver;                // 연산자 해결자 객체 생성
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver)); // 연산자 등록 함수 호출

  constexpr int kTensorArenaSize = 3000;  // 텐서를 저장하기 위한 메모리 영역 크기 설정
  uint8_t tensor_arena[kTensorArenaSize]; // 텐서 메모리 영역 배열 선언

  constexpr int kNumResourceVariables = 24; // 리소스 변수의 수

  // 기록용 마이크로 할당자와 마이크로 인터프리터 생성
  tflite::RecordingMicroAllocator *allocator(tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
  tflite::RecordingMicroInterpreter interpreter(
      tflite::GetModel(g_hello_world_float_model_data), op_resolver, allocator,
      tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables), &profiler);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors()); // 텐서 할당
  TFLITE_CHECK_EQ(interpreter.inputs_size(), 1);        // 입력 텐서 수 확인
  interpreter.input(0)->data.f[0] = 1.f;                // 첫 번째 입력 텐서에 1.0 설정
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());          // 모델 실행

  MicroPrintf("");              // 줄 바꿈 출력
  profiler.LogTicksPerTagCsv(); // 프로파일링 결과 CSV 형식으로 출력

  MicroPrintf("");                                    // 줄 바꿈 출력
  interpreter.GetMicroAllocator().PrintAllocations(); // 메모리 할당 정보 출력
  return kTfLiteOk;
}

// 부동소수점 모델을 로드하고 추론을 수행하는 함수
TfLiteStatus LoadFloatModelAndPerformInference() {
  const tflite::Model *model = ::tflite::GetModel(g_hello_world_float_model_data); // 부동소수점 모델 데이터 로드
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);                        // 모델의 스키마 버전 확인

  HelloWorldOpResolver op_resolver;                // 연산자 해결자 객체 생성
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver)); // 연산자 등록 함수 호출

  constexpr int kTensorArenaSize = 3000;  // 텐서를 저장하기 위한 메모리 영역 크기 설정
  uint8_t tensor_arena[kTensorArenaSize]; // 텐서 메모리 영역 배열 선언

  // 마이크로 인터프리터 객체 생성
  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);
  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors()); // 텐서 할당

  // 예측값과 기대값 사이의 허용 오차 설정
  float epsilon = 0.05f;
  constexpr int kNumTestValues = 4;
  float golden_inputs[kNumTestValues] = {0.f, 1.f, 3.f, 5.f}; // 테스트 입력값 설정

  for (int i = 0; i < kNumTestValues; ++i) {
    /*
    TfLitePtrUnion 타입의 경우 data.f를 통해 접근한다.
    이때 n차원 텐서를 1차원으로 평탄화하여 접근해야 한다.
    예를 들어 [[1,2,3], [4,5,6]] 이라는 텐서가 있다면
    int arr[2][3] = {{1,2,3}, {4,5,6}};
    for(i=0; i<6;i++) {
      interpreter.input(0)->data.f[i] = arr[i/3][i%3];
    }
    */
    /*
    1. 최적의 값
    2. 순전파 계산
    3. 손실값 계산
    4. 업데이트
    */
    interpreter.input(0)->data.f[0] = golden_inputs[i]; // 입력 텐서에 테스트값 설정
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());        // 모델 실행
    float y_pred = interpreter.output(0)->data.f[0];    // 예측값 추출
    // TF_LITE_MICRO_EXPECT_LE: TFLITE_CHECK_LE 구버전
    // TFLITE_CHECK_LE(x,y): x가 yㅂㅎ다 작거나 같은지 검사
    TFLITE_CHECK_LE(abs(sin(golden_inputs[i]) - y_pred), epsilon); // 예측값과 기대값 비교
  }

  return kTfLiteOk;
}

// 양자화 모델을 로드하고 추론을 수행하는 함수
TfLiteStatus LoadQuantModelAndPerformInference() {
  const tflite::Model *model = ::tflite::GetModel(g_hello_world_int8_model_data); // 양자화 모델 데이터 로드
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);                       // 모델의 스키마 버전 확인

  HelloWorldOpResolver op_resolver;                // 연산자 해결자 객체 생성
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver)); // 연산자 등록 함수 호출

  constexpr int kTensorArenaSize = 3000;  // 텐서를 저장하기 위한 메모리 영역 크기 설정
  uint8_t tensor_arena[kTensorArenaSize]; // 텐서 메모리 영역 배열 선언

  // 마이크로 인터프리터 객체 생성
  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);
  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors()); // 텐서 할당

  TfLiteTensor *input = interpreter.input(0); // 입력 텐서 포인터 가져오기
  // TF_LITE_MICRO_NE(nullptr, input);            // 구버전의 TFLITE_CHECK_NE 매크로
  TFLITE_CHECK_NE(input, nullptr); // 입력 텐서 포인터 검사

  TfLiteTensor *output = interpreter.output(0); // 출력 텐서 포인터 가져오기
  TFLITE_CHECK_NE(output, nullptr);             // 출력 텐서 포인터 검사

  float output_scale = output->params.scale;         // 출력 텐서의 스케일 파라미터
  int output_zero_point = output->params.zero_point; // 출력 텐서의 제로 포인트 파라미터

  float epsilon = 0.05; // 예측값과 기대값 사이의 허용 오차 설정

  constexpr int kNumTestValues = 4;
  float golden_inputs_float[kNumTestValues] = {0.77, 1.57, 2.3, 3.14}; // 테스트 입력값 설정(부동소수점)

  int8_t golden_inputs_int8[kNumTestValues] = {-96, -63, -34, 0}; // 테스트 입력값 설정(8비트 정수)

  for (int i = 0; i < kNumTestValues; ++i) {
    input->data.int8[0] = golden_inputs_int8[i];                              // 입력 텐서에 테스트값 설정
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());                              // 모델 실행
    float y_pred = (output->data.int8[0] - output_zero_point) * output_scale; // 양자화된 예측값을 부동소수점으로 변환
    TFLITE_CHECK_LE(abs(sin(golden_inputs_float[i]) - y_pred), epsilon); // 예측값과 기대값 비교
  }

  return kTfLiteOk;
}

int main(int argc, char *argv[]) {
  tflite::InitializeTarget();                                 // 타겟(하드웨어) 초기화
  TF_LITE_ENSURE_STATUS(ProfileMemoryAndLatency());           // 메모리 및 지연 프로파일링 함수 호출
  TF_LITE_ENSURE_STATUS(LoadFloatModelAndPerformInference()); // 부동소수점 모델 추론 함수 호출
  TF_LITE_ENSURE_STATUS(LoadQuantModelAndPerformInference()); // 양자화 모델 추론 함수 호출
  MicroPrintf("~~~ALL TESTS PASSED~~~\n");                    // 모든 테스트 통과 메시지 출력
  return kTfLiteOk;                                           // 성공 상태 반환
}
