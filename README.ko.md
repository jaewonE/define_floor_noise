# TinyML(On-Sensor AI)을 활용한 층간 소음 감지

[ [English](https://github.com/JaewonE/define_floor_noise/blob/main/README) | [한국어](https://github.com/JaewonE/define_floor_noise/blob/main/README.ko.md) ]

이 저장소는 다음 논문의 구현 자산(assets)을 포함하고 있습니다:
**“초저전력 시스템상에서 TinyML이 탑재된 On-Sensor AI를 활용한 공동주택 층간 소음 문제 해결”** (2024년 3월).
(원제: _Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems_)

이 프로젝트는 Arduino Nano 33 BLE 보드와 온디바이스 CNN(TensorFlow Lite for Microcontrollers)을 사용하여 서버 측 추론 없이 실시간으로 층간 소음을 구별하는 것을 목표로 합니다.

기존의 시간 순서대로 작성되었던 연구 로그는 재현성, 추적 가능성, 그리고 논문과 코드 간의 매핑에 중점을 둔 포트폴리오 우선 구조로 재구성되었습니다.

## 논문 (Paper)

- PDF: [`paper/Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems.pdf`](https://github.com/JaewonE/define_floor_noise/blob/main/paper/Addressing%20Inter-floor%20Noise%20Issues%20in%20Apartment%20Buildings%20using%20On-Sensor%20AI%20Embedded%20with%20TinyML%20on%20Ultra-Low-Power%20Systems.pdf)
- Summary: [`docs/paper_summary.md`](https://github.com/JaewonE/define_floor_noise/blob/main/docs/paper_summary.md)
- Paper-to-code map: [`docs/paper_to_code_map.md`](https://github.com/JaewonE/define_floor_noise/blob/main/docs/paper_to_code_map.md)

## 빠른 시작 (Quickstart)

1. 환경 생성 및 의존성 설치:

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

```

2. 저장소 무결성 점검(Sanity Check) 실행:

```bash
make smoke

```

3. 데이터셋 윈도우 재구축 (선택 사항, 전처리된 데이터를 다시 생성하는 경우):

```bash
make preprocess

```

4. 모델 학습/평가/내보내기:

```bash
make train

```

5. 펌웨어용 C 배열로 `.tflite` 변환:

```bash
make tflite2cc

```

## 주요 결과 재현 (Reproducing Key Results)

- 전체 재현 가이드: [`docs/reproduce.md`](https://github.com/JaewonE/define_floor_noise/blob/main/docs/reproduce.md)
- 실험 요약 산출물(artifacts): [`experiments/results/`](https://github.com/JaewonE/define_floor_noise/blob/main/experiments/results)

## 저장소 구조 (Repository Structure)

- `paper/`: 포함된 논문 PDF 및 인용 메타데이터
- `docs/`: 방법론 요약, 매핑, 재현, 감사(audit) 노트
- `src/`: 재현 가능한 Python 스크립트 (수집 헬퍼, 전처리, 학습)
- `embedded/`: 온디바이스 TinyML 펌웨어 (최종 `floor_noise_v5`) + BLE 예제
- `data/`: 원시 캡처 데이터 및 전처리된 학습/평가 윈도우
- `notebooks/`: 논문 관련 노트북 + 레거시 튜토리얼 노트북
- `experiments/`: 학습된 산출물 및 보고된 결과 표
- `assets/`: 문서에 사용된 프로젝트 이미지
- `archive/`: 추적 가능성을 위한 기존의 시간 순서별 프로젝트 레이아웃

## 포함된 내용 / 포함되지 않은 내용 (What’s In / What’s Not)

### 이 저장소에 포함됨

- 개발 중 사용된 원시/전처리 데이터 파일 (소-중규모 로컬 데이터셋)
- 모델 산출물 (`.tflite`, `.cc`) 및 펌웨어 소스
- 전처리/학습/내보내기를 위한 재현 가능한 스크립트

### 이 저장소에 포함되지 않음

- 물리적 하드웨어 설정 자동화 (현장 수동 설치 필요)
- 전체 실험 환경 복제 (건물/층 설정 제약 사항 존재)
- 모든 TensorFlow 버전/하드웨어에서 비트 단위(bit-identical)까지 동일한 재학습 결과 보장

## 인용 (Citation)

이 저장소를 사용하시는 경우, 다음과 같이 인용해 주세요:

> 곽재원, 최인영, “초저전력 시스템상에서 TinyML이 탑재된 On-Sensor AI를 활용한 공동주택 층간 소음 문제 해결,” _한국컴퓨터정보학회논문지_, vol. 29, no. 3, pp. 75–81, 2024년 3월.
