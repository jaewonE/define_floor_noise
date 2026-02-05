# 논문-코드 매핑(Paper-to-Code Map)

이 문서는 논문 섹션을 레포지토리의 구체적인 아티팩트(코드/데이터/노트북/펌웨어) 경로에 **정식(canonical)** 으로 매핑한 자료입니다.

## 섹션 매핑

| 논문 섹션 | 아티팩트 경로 | 무엇을 하는가 | 실행 방법 | 기대 출력 |
|---|---|---|---|---|
| 서론 / 문제 정의 | `paper/Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems.pdf`, `docs/paper_summary.md` | 문제 정의 및 시스템 목표를 제공 | 파일 열기 | 논문 맥락 및 파이프라인 요약 |
| 데이터 수집 방법 | `src/data_collection/arduino_capture.ino`, `src/data_collection/serial_capture.py`, `src/data_collection/serial_capture_real.py`, `data/raw/real/origin_data`, `data/raw/real/new_origin_data`, `data/raw/real/white_data.txt`, `data/raw/sample/total_noise.txt`, `data/raw/sample/total_white.txt` | IMU 스트림을 수집하고 원시 텍스트 신호를 저장 | `python src/data_collection/serial_capture.py --port <SERIAL_PORT> --output data/raw/real/live_capture.txt` | `x,y,z` 텍스트 로그 파일 |
| 전처리 / 데이터셋 구성 | `src/preprocess/build_dataset.py`, `notebooks/paper_pipeline/create_dataset.ipynb`, `notebooks/paper_pipeline/validate_data_trans.ipynb`, `notebooks/paper_pipeline/new_data_trans.ipynb`, `data/processed/dataset` | 원시 기록을 128x3 노이즈/화이트 윈도우로 변환 | `python src/preprocess/build_dataset.py` | `data/processed/dataset/noise/*.csv`, `data/processed/dataset/white/*.csv`, 메타데이터 JSON |
| 모델 학습 및 평가 | `src/models/train_and_export.py`, `notebooks/paper_pipeline/model.ipynb` | CNN 학습, 지표 평가, 모델 내보내기(export) | `python src/models/train_and_export.py --epochs 20` | `experiments/artifacts/model.keras`, `experiments/artifacts/model.tflite`, 지표 JSON/혼동행렬(CM) CSV |
| TFLite / TinyML 변환 | `tools/tflite_to_cc.sh`, `experiments/artifacts/model.tflite`, `experiments/artifacts/model.cc`, `notebooks/tutorials/c_trans_tutorial.ipynb` | TFLite 모델 바이트를 C 배열 소스로 변환 | `./tools/tflite_to_cc.sh experiments/artifacts/model.tflite experiments/artifacts/model.cc model_tflite` | `model_tflite[]` 및 길이 상수를 포함한 C 소스 |
| 임베디드 배포 로직 | `embedded/floor_noise_v5/floor_noise_v5.ino`, `embedded/floor_noise_v5/imu_provider.h`, `embedded/floor_noise_v5/floor_noise_model_data.cpp`, `embedded/bluetooth_examples/*.ino` | 디바이스 내 추론, 임계값 로직, BLE 신호 전송 수행 | Arduino IDE에서 Nano 33 BLE 대상으로 빌드/업로드 | 디바이스 측 층간소음 분류 및 알림 동작 |
| 실험 및 보고 결과 | `experiments/results/paper_table1_results.csv`, `docs/reproduce.md`, `notebooks/paper_pipeline/`의 노트북 플롯 | 논문 표 값 저장 및 그림/표 재현 경로 참조 제공 | CSV 열기 또는 노트북 셀 실행 | 논문 지표/플롯의 일부를 재현 또는 추적 가능 |

## 핵심 질문에 대한 직접 답변

### 데이터 수집 코드는 어디에 있나?
- `src/data_collection/arduino_capture.ino`
- `src/data_collection/serial_capture.py`
- `src/data_collection/serial_capture_real.py`

### 전처리는 어디에 있나?
- `src/preprocess/build_dataset.py`
- `notebooks/paper_pipeline/create_dataset.ipynb`
- `notebooks/paper_pipeline/validate_data_trans.ipynb`
- `notebooks/paper_pipeline/new_data_trans.ipynb`

### 모델 학습과 평가는 어디에 있나?
- `src/models/train_and_export.py`
- `notebooks/paper_pipeline/model.ipynb`

### TFLite/TinyML 변환은 어디에 있나?
- `tools/tflite_to_cc.sh`
- `notebooks/tutorials/c_trans_tutorial.ipynb` (레거시 튜토리얼/참고 맥락)

### 임베디드/펌웨어 로직은 어디에 있나?
- `embedded/floor_noise_v5/floor_noise_v5.ino`
- `embedded/floor_noise_v5/imu_provider.h`
- `embedded/floor_noise_v5/floor_noise_model_data.cpp`

### 실험 결과(그림/표)는 어디에서 재현되나?
- `experiments/results/paper_table1_results.csv` (Table 1 값)
- 학습/평가 플롯은 `notebooks/paper_pipeline/` 내 노트북 셀에서 생성
- 재현 명령과 한계는 `docs/reproduce.md`에 문서화

## 충실도(Fidelity) 참고
- 레포지토리는 `archive/chronological/step_08_embedded_board_builds`에 펌웨어 반복 버전을 보존합니다.
- 최종 펌웨어 스냅샷(`embedded/floor_noise_v5`)은 임계값 상수를 튜닝한 값으로 포함하며, 논문 본문에 서술된 일부 임계값/카운트와 정확히 일치하지 않을 수 있습니다. 이 차이는 재현성을 과장하지 않기 위해 문서에 명시했습니다.
