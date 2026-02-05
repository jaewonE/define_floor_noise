# Inter-floor Noise Detection with TinyML (On-Sensor AI)

[ [English](https://github.com/jaewonE/define_floor_noise) | [한국어](https://github.com/JaewonE/define_floor_noise/blob/main/README.ko.md) ]

- **DBpia**: https://www.dbpia.co.kr/journal/articleDetail?nodeId=NODE11743327
- **PDF**: [`paper/Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems.pdf`](https://github.com/JaewonE/define_floor_noise/blob/main/paper/Addressing%20Inter-floor%20Noise%20Issues%20in%20Apartment%20Buildings%20using%20On-Sensor%20AI%20Embedded%20with%20TinyML%20on%20Ultra-Low-Power%20Systems.pdf)

This repository contains the implementation assets behind the paper:
**“Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems”** (March 2024).

The project targets real-time inter-floor noise discrimination using Arduino Nano 33 BLE boards and an on-device CNN (TensorFlow Lite for Microcontrollers), avoiding server-side inference.

The original chronological research log has been reorganized into a portfolio-first structure focused on reproducibility, traceability, and paper-to-code mapping.

## Paper

- PDF: [`paper/Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems.pdf`](https://github.com/JaewonE/define_floor_noise/blob/main/paper/Addressing%20Inter-floor%20Noise%20Issues%20in%20Apartment%20Buildings%20using%20On-Sensor%20AI%20Embedded%20with%20TinyML%20on%20Ultra-Low-Power%20Systems.pdf)
- Summary: [`docs/paper_summary.md`](https://github.com/JaewonE/define_floor_noise/blob/main/docs/paper_summary.md)
- Paper-to-code map: [`docs/paper_to_code_map.md`](https://github.com/JaewonE/define_floor_noise/blob/main/docs/paper_to_code_map.md)

## Quickstart

1. Create an environment and install dependencies:

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

2. Run repository sanity checks:

```bash
make smoke
```

3. Rebuild dataset windows (optional, if regenerating processed data):

```bash
make preprocess
```

4. Train/evaluate/export model:

```bash
make train
```

5. Convert `.tflite` to C array for firmware:

```bash
make tflite2cc
```

## Reproducing Key Results

- Full reproduction guide: [`docs/reproduce.md`](https://github.com/JaewonE/define_floor_noise/blob/main/docs/reproduce.md)
- Experimental summary artifacts: [`experiments/results/`](https://github.com/JaewonE/define_floor_noise/blob/main/experiments/results)

## Repository Structure

- `paper/`: included paper PDF and citation metadata
- `docs/`: methodology summary, mapping, reproduction, audit notes
- `src/`: reproducible Python scripts (collection helpers, preprocessing, training)
- `embedded/`: on-device TinyML firmware (final `floor_noise_v5`) + BLE examples
- `data/`: raw captures and processed train/eval windows
- `notebooks/`: paper-related notebooks + legacy tutorial notebook
- `experiments/`: trained artifacts and reported result tables
- `assets/`: project images used in docs
- `archive/`: original chronological project layout for traceability

## What’s In / What’s Not

### In this repo

- Raw/processed data files used during development (small-to-midsize local datasets)
- Model artifacts (`.tflite`, `.cc`) and firmware source
- Reproducible scripts for preprocessing/training/export

### Not in this repo

- Physical hardware setup automation (manual installation on-site)
- Full experimental environment replication (building/floor setup constraints)
- Guaranteed bit-identical retraining outcomes across all TensorFlow versions/hardware

## Citation

If you use this repository, please cite:

> J.-W. Kwak and I.-Y. Choi, “Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems,” _Journal of The Korea Society of Computer and Information_, vol. 29, no. 3, pp. 75–81, Mar. 2024.
