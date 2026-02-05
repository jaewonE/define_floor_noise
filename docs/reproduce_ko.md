# 재현 가이드(Reproduction Guide)

## 1. 환경

권장:
- Python 3.10+ (현대적인 3.x에서 테스트)
- Arduino Nano 33 BLE 펌웨어 업로드를 위한 Arduino IDE

의존성 설치:

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## 2. 무결성(스모크) 체크

```bash
make smoke
```

기대 결과:
- 필수 파일 존재 여부 확인
- 처리된 데이터셋의 형태/개수 일관성 확인 (`noise == white`, 샘플 shape `(128,3)`)

## 3. 처리된 데이터셋 재생성(선택)

원시 `.txt` 캡처에서 윈도우를 다시 생성해야 할 때 사용합니다.

```bash
python src/preprocess/build_dataset.py   --origin-dir data/raw/real/new_origin_data   --white-file data/raw/real/white_data.txt   --output-dir data/processed/dataset   --window-size 128
```

출력:
- `data/processed/dataset/noise/*.csv`
- `data/processed/dataset/white/*.csv`
- `data/processed/dataset/metadata.json`

## 4. CNN 학습 및 평가

```bash
python src/models/train_and_export.py   --dataset-dir data/processed/dataset   --epochs 20   --output-dir experiments/artifacts   --metrics-path experiments/results/train_metrics.json
```

출력:
- `experiments/artifacts/model.keras`
- `experiments/artifacts/model.tflite`
- `experiments/results/train_metrics.json`
- `experiments/results/confusion_matrix.csv`

## 5. 펌웨어용 TFLite → C 변환

```bash
./tools/tflite_to_cc.sh   experiments/artifacts/model.tflite   embedded/floor_noise_v5/floor_noise_model_data.cpp   model_tflite
```

기대 결과:
- 모델 바이트 및 길이 심볼(symbol)을 포함하는 C 배열 소스가 갱신됨

## 6. 펌웨어 배포

Arduino IDE에서 다음 파일을 엽니다.
- `embedded/floor_noise_v5/floor_noise_v5.ino`

보드 타깃:
- Arduino Nano 33 BLE

의존 요소:
- TensorFlow Lite Micro 호환 Arduino 설정
- 소스 include에서 사용하는 IMU/BLE 라이브러리

## 7. 논문 그림/표 재현

### Table 1
- 논문 표 값을 CSV로 미리 입력해 둠:
  - `experiments/results/paper_table1_results.csv`

### 학습/검증 곡선 및 신호 시각화
- 노트북 셀 실행으로 재생성:
  - `notebooks/paper_pipeline/model.ipynb`
  - `notebooks/paper_pipeline/create_dataset.ipynb`
  - `notebooks/paper_pipeline/validate_data_trans.ipynb`

## 8. 재현성 범위

### 레포지토리 내용만으로 완전히 재현 가능한 범위
- 레포 구조 및 논문-코드 추적성
- 데이터 레이아웃 무결성 체크 (`python src/utils/smoke_check.py`)
- 스크립트 기반 전처리 명령 경로 (`src/preprocess/build_dataset.py`)
- 스크립트 기반 학습/평가/내보내기 경로 (`src/models/train_and_export.py`)
- TFLite→C 변환 경로 (`tools/tflite_to_cc.sh`)
- 펌웨어 소스 수준 로직(임계값/반복 로직) 확인

### 부분적으로 재현 가능한 범위
- 논문에 보고된 정확한 수치 결과(버전/난수/하드웨어·런타임 차이에 민감)
- 현장 물리 테스트의 정확한 동작(실제 아파트 환경/보드 설치 위치에 의존)

### 추가 리소스 없이는 완전 재현이 어려운 범위
- 논문과 동일한 물리적 배치 조건에서의 설치/동작 결과
- 동일한 현장 조건에서의 BLE 알림(end-to-end) 동작

## 9. 논문 본문과의 알려진 차이(Known Differences)

- 논문 본문은 임계값/카운트 로직 값을 문장으로 서술합니다.
- 레포의 최종 펌웨어 스냅샷(`embedded/floor_noise_v5`)에는 튜닝된 상수가 포함되어 있어, 본문 서술 값과 정확히 일치하지 않을 수 있습니다.
- 추적 가능성을 위해 과거 펌웨어 변형은 `archive/chronological/step_08_embedded_board_builds`에 보존되어 있습니다.
