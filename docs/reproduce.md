# Reproduction Guide

## 1. Environment

Recommended:
- Python 3.10+ (tested with modern 3.x)
- Arduino IDE for Nano 33 BLE firmware upload

Install dependencies:

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## 2. Sanity Check

```bash
make smoke
```

Expected:
- Confirms required files exist
- Confirms processed dataset shape/count consistency (`noise == white`, sample shape `(128,3)`)

## 3. Rebuild Processed Dataset (Optional)

Use when regenerating windows from raw `.txt` captures:

```bash
python src/preprocess/build_dataset.py \
  --origin-dir data/raw/real/new_origin_data \
  --white-file data/raw/real/white_data.txt \
  --output-dir data/processed/dataset \
  --window-size 128
```

Outputs:
- `data/processed/dataset/noise/*.csv`
- `data/processed/dataset/white/*.csv`
- `data/processed/dataset/metadata.json`

## 4. Train and Evaluate CNN

```bash
python src/models/train_and_export.py \
  --dataset-dir data/processed/dataset \
  --epochs 20 \
  --output-dir experiments/artifacts \
  --metrics-path experiments/results/train_metrics.json
```

Outputs:
- `experiments/artifacts/model.keras`
- `experiments/artifacts/model.tflite`
- `experiments/results/train_metrics.json`
- `experiments/results/confusion_matrix.csv`

## 5. Convert TFLite to C for Firmware

```bash
./tools/tflite_to_cc.sh \
  experiments/artifacts/model.tflite \
  embedded/floor_noise_v5/floor_noise_model_data.cpp \
  model_tflite
```

Expected:
- Updated C array source with model bytes and length symbol

## 6. Firmware Deployment

Open in Arduino IDE:
- `embedded/floor_noise_v5/floor_noise_v5.ino`

Board target:
- Arduino Nano 33 BLE

Dependencies:
- TensorFlow Lite Micro-compatible Arduino setup
- IMU/BLE libraries used in source includes

## 7. Recreating Paper Figures/Tables

### Table 1
- Pre-entered from the paper into:
  - `experiments/results/paper_table1_results.csv`

### Training/validation curves and signal visualizations
- Regenerated via notebook cells:
  - `notebooks/paper_pipeline/model.ipynb`
  - `notebooks/paper_pipeline/create_dataset.ipynb`
  - `notebooks/paper_pipeline/validate_data_trans.ipynb`

## 8. Reproducibility Scope

### Fully reproducible from repository contents
- Dataset integrity checks and folder-level pipeline walkthrough
- Scripted model training/evaluation/export path
- TFLite-to-C conversion path
- Firmware source-level logic inspection

### Partially reproducible
- Exact paper-reported numeric results (sensitive to TensorFlow version, randomization, and hardware/runtime differences)
- Full on-site physical test behavior (depends on real apartment environment and board placement)

### Not fully reproducible without additional resources
- Physical deployment conditions (building geometry, sensor placement, real occupants/actions)
- End-to-end BLE alert behavior under identical field conditions

## 9. Known Differences vs Paper Text

- Paper text describes threshold/count logic values in prose.
- Repository final firmware snapshot (`embedded/floor_noise_v5`) contains tuned constants that differ from those exact prose values.
- Historical firmware variants are preserved under `archive/chronological/step_08_embedded_board_builds` for traceability.
