## 7. 실제 데이터 분석 및 CNN 모델 생성

### 7-1 데이터 분석 및 시각화

수집된 데이터를 적절히 전처리하여 활용가능한 데이터로 구성하였다.

이에 대한 내용은 create_dataset.ipynb 파일에 구체적으로 명시되어 있다.

아래는 데이터 분석 및 시각화를 수행한 예시 이미지이다.

<img src="https://raw.githubusercontent.com/jaewonE/define_floor_noise/main/images/create_dataset_sample_image.png" alt="create_dataset_sample_image">

<br>

### 7-2 데이터 전처리 유효성 검사

모델의 입력 벡터로 구성하기 위해 적절한 데이터 전처리 과정을 수행하였다.

해당 내용은 validate_data_trans.ipynb에 구체적으로 명시되어 있다.

<br>

### 7-3. CNN 모델 생성

실제 데이터를 기반으로 CNN 모델을 생성하고 tflite 및 cc 파일로의 변환을 수행하였다.

이에 대한 자세한 내용은 mode.ipynb 파일에 구체적으로 명시되어 있다.

<br>

이외 파일에 대한 설명은 아래와 같다.

- origin_data: Arduino Nano 33 BLE 보드를 천장에 부착한 뒤 층간소음을 발생시켰을 때의 원본 데이터이다. 각 파일명은 층간소음 발생 행위이다.

- dataset: 적절한 데이터 전처리 과정에 따라 층간소음 데이터인 noise와 층간소음이 발생하지 않은 데이터인 white 구분되어 있다.

  - validate_data_trans : 데이터 전처리 방법의 유효성을 확인하는 과정을 담고 있다.

  - create_dataset : 데이터 전처리 과정을 담고 있다.
