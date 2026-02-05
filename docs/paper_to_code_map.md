# Paper-to-Code Map

This is the canonical mapping from paper sections to concrete repository artifacts.

## Section Mapping

| Paper section                        | Artifact path(s)                                                                                                                                                                                                                                                                                     | What it does                                                 | How to run                                                                                                  | Expected output                                                                                |
| ------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| Intro / Problem framing              | `paper/Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems.pdf`, `docs/paper_summary.md`                                                                                                                                   | Provides problem statement and system goal                   | Open file(s)                                                                                                | Paper context and summarized pipeline                                                          |
| Data collection method               | `src/data_collection/arduino_capture.ino`, `src/data_collection/serial_capture.py`, `src/data_collection/serial_capture_real.py`, `data/raw/real/origin_data`, `data/raw/real/new_origin_data`, `data/raw/real/white_data.txt`, `data/raw/sample/total_noise.txt`, `data/raw/sample/total_white.txt` | Captures IMU stream and stores raw text signals              | `python src/data_collection/serial_capture.py --port <SERIAL_PORT> --output data/raw/real/live_capture.txt` | `x,y,z` text log files                                                                         |
| Preprocessing / dataset construction | `src/preprocess/build_dataset.py`, `notebooks/paper_pipeline/create_dataset.ipynb`, `notebooks/paper_pipeline/validate_data_trans.ipynb`, `notebooks/paper_pipeline/new_data_trans.ipynb`, `data/processed/dataset`                                                                                  | Converts raw recordings into 128x3 noise/white windows       | `python src/preprocess/build_dataset.py`                                                                    | `data/processed/dataset/noise/*.csv`, `data/processed/dataset/white/*.csv`, metadata JSON      |
| Model training and evaluation        | `src/models/train_and_export.py`, `notebooks/paper_pipeline/model.ipynb`                                                                                                                                                                                                                             | Trains CNN, evaluates metrics, exports model                 | `python src/models/train_and_export.py --epochs 20`                                                         | `experiments/artifacts/model.keras`, `experiments/artifacts/model.tflite`, metrics JSON/CM CSV |
| TFLite / TinyML conversion           | `tools/tflite_to_cc.sh`, `experiments/artifacts/model.tflite`, `experiments/artifacts/model.cc`, `notebooks/tutorials/c_trans_tutorial.ipynb`                                                                                                                                                        | Converts TFLite model bytes into C array source              | `./tools/tflite_to_cc.sh experiments/artifacts/model.tflite experiments/artifacts/model.cc model_tflite`    | C source with `model_tflite[]` and length constant                                             |
| Embedded deployment logic            | `embedded/floor_noise_v5/floor_noise_v5.ino`, `embedded/floor_noise_v5/imu_provider.h`, `embedded/floor_noise_v5/floor_noise_model_data.cpp`, `embedded/bluetooth_examples/*.ino`                                                                                                                    | Runs on-device inference, threshold logic, and BLE signaling | Build/upload in Arduino IDE for Nano 33 BLE                                                                 | Device-side floor-noise classification and alert behavior                                      |
| Experiments and reported results     | `experiments/results/paper_table1_results.csv`, `docs/reproduce.md`, notebook plots from `notebooks/paper_pipeline/`                                                                                                                                                                                 | Stores table values and references figure recreation paths   | Open CSV or run notebook cells                                                                              | Recreated/traceable subset of paper metrics and plots                                          |

## Direct Answers to Critical Questions

### Where is the data collection code?

- `src/data_collection/arduino_capture.ino`
- `src/data_collection/serial_capture.py`
- `src/data_collection/serial_capture_real.py`

### Where is preprocessing?

- `src/preprocess/build_dataset.py`
- `notebooks/paper_pipeline/create_dataset.ipynb`
- `notebooks/paper_pipeline/validate_data_trans.ipynb`
- `notebooks/paper_pipeline/new_data_trans.ipynb`

### Where is model training and evaluation?

- `src/models/train_and_export.py`
- `notebooks/paper_pipeline/model.ipynb`

### Where is TFLite/TinyML conversion?

- `tools/tflite_to_cc.sh`
- `notebooks/tutorials/c_trans_tutorial.ipynb` (legacy tutorial context)

### Where is embedded/firmware logic?

- `embedded/floor_noise_v5/floor_noise_v5.ino`
- `embedded/floor_noise_v5/imu_provider.h`
- `embedded/floor_noise_v5/floor_noise_model_data.cpp`

### Where are experimental results, figures, and tables recreated?

- `experiments/results/paper_table1_results.csv` (Table 1 values)
- Training/evaluation plots are generated by notebook cells in `notebooks/paper_pipeline/`
- Reproduction commands and limits are documented in `docs/reproduce.md`

## Fidelity Notes

- The repository contains iterative firmware variants in `archive/chronological/step_08_embedded_board_builds`.
- The final firmware snapshot (`embedded/floor_noise_v5`) uses tuned threshold constants that do not exactly match all threshold counts described in paper text; this is documented to avoid overclaiming reproducibility.
