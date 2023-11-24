## 5. 샘플 데이터 분석 및 CNN 모델 생성

### 5-1 데이터 분석 및 시각화

수집된 데이터는 그 자체로 의미가 있었지만, 시각화를 통해 더 직관적으로 이해할 수 있었다. 실시간으로 x, y, z 축의 데이터를 그래프로 표현하는 작업을 진행했으며, 이 과정에서 데이터의 흐름과 패턴뿐만 아니라 예상치 못한 노이즈나 오차에 대한 인식도 향상되었다.

이 시각화 작업은 프로젝트 구성원들에게도 데이터의 특성과 중요성을 이해하는 데 큰 도움이 되었다. 실시간 그래프를 통해 층간소음의 발생 패턴, 강도, 지속 시간 등 다양한 정보를 한눈에 파악할 수 있었다.

이 활동을 통해 가속도계 센서의 활용법과 데이터의 특성 및 중요성을 깊이 있게 이해했다. 이 경험은 앞으로의 데이터 분석 및 모델링 작업의 기반이 되었으며, 이후 연구에서도 이 데이터 수집 및 시각화 경험을 바탕으로 더 정교한 분석을 진행할 예정이다.

이 활동에 대한 근거로 가속도계 시각화 결과를 담은 'freq.gif' 파일을 제출한다. 아래는 해당 gif 파일의 캡처본으로, 화면에 보이는 것과 같이 x, y, z 축에 대한 값을 실시간으로 그래프로 그린다.

<img src="https://raw.githubusercontent.com/jaewonE/define_floor_noise/main/images/realtime_acceleration_values.png" alt="realtime_acceleration_values">

<br>

### 5-2 CNN 모델 생성

샘플 데이터를 기반으로 CNN 모델을 생성하고 tflite 및 cc 파일로의 변환을 수행하였다.

CNN 모델을 생성하는 내용은 train.train.ipynb 파일에 구체적으로 명시되어 있다.

이때 CNN 모델을 생성하는 코드 부분에서 tensorflow의 6491886 git version을 사용한다. 이는 tensorflow lite가 독립적인 레포지토리로 분리되기 이전의 버전이다.

이전 버전으로 되돌리는 방법은 Train_a_gesture_recognition_model_for_microcontroller_use.ipynb 파일을 참고하면 된다.
