# Paper Summary (Pipeline-Focused)

## Problem
The paper addresses inter-floor impact noise in apartment buildings with a low-cost, real-time system. Instead of sending sensor data to a server, inference runs directly on an ultra-low-power MCU board (on-sensor AI).

## Inputs and Data Collection
- Hardware: Arduino Nano 33 BLE with onboard IMU.
- Setup: Boards attached to multiple ceiling positions in the lower floor.
- Sampling: Gyroscope/acceleration streams at ~104 Hz, operationally sampled at ~10 ms intervals.
- Defined activity classes (6): walking, random walking, running, hammer blow, rolling dumbbell, desk drag.

## Data Processing and Dataset
- Sensor windows of length 128 (x, y, z channels) are used as model input.
- Binary target: `Noise` vs `Silence`.
- Dataset reported in paper: 9,588 windows total, split 7:2:1 for train/validation/test.

## Model
- CNN-based binary classifier.
- Architecture described in paper:
  - Conv2D (3x3, 16 channels)
  - MaxPool2D (3x3)
  - Conv2D (2x2, 8 channels)
  - MaxPool2D (2x2)
  - Dense (16)
  - Dense (2, output classes)
- Loss/optimization: binary cross-entropy with gradient-based optimization.
- Reported performance: ~99% accuracy, F1 ~0.98.

## TinyML Conversion and Deployment
- Trained model exported to TensorFlow Lite (`.tflite`).
- `.tflite` converted to C array (`xxd -i`) for firmware embedding.
- Embedded logic performs repeated checks over time windows and applies thresholded decision logic before final inter-floor-noise alerting.

## Runtime Logic
- One inference consumes a 128-sample window (~1.28 s at ~10 ms/sample).
- Paper logic includes vibration thresholding + repeated-count logic over multiple checks before issuing a final alert.
- BLE is used to notify the upper-floor device.

## Evaluation
- On-site tests across the 6 activities.
- Paper Table 1 reports total accuracy of 96% over 600 tests.

## Important Reproducibility Note
This repository preserves both:
- paper-critical artifacts (dataset/model/firmware), and
- iterative historical variants in `archive/chronological/`.

Some threshold parameters in the final firmware snapshot differ from text values in the paper, and are documented in `docs/paper_to_code_map.md` and `docs/reproduce.md`.
