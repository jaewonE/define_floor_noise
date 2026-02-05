"""Quick deterministic checks for repository integrity and dataset shape."""

from __future__ import annotations

import csv
from pathlib import Path


REQUIRED_PATHS = [
    Path("paper/Addressing Inter-floor Noise Issues in Apartment Buildings using On-Sensor AI Embedded with TinyML on Ultra-Low-Power Systems.pdf"),
    Path("data/raw/real/origin_data"),
    Path("data/processed/dataset/noise"),
    Path("data/processed/dataset/white"),
    Path("notebooks/paper_pipeline/model.ipynb"),
    Path("embedded/floor_noise_v5/floor_noise_v5.ino"),
    Path("experiments/artifacts/model.tflite"),
    Path("experiments/artifacts/model.cc"),
]


for p in REQUIRED_PATHS:
    if not p.exists():
        raise FileNotFoundError(f"Missing required path: {p}")

noise_files = sorted(Path("data/processed/dataset/noise").glob("*.csv"))
white_files = sorted(Path("data/processed/dataset/white").glob("*.csv"))

if len(noise_files) == 0 or len(white_files) == 0:
    raise RuntimeError("Dataset folders are empty")
if len(noise_files) != len(white_files):
    raise RuntimeError("Noise/white sample counts differ")

with noise_files[0].open("r", encoding="utf-8") as f:
    rows = list(csv.reader(f))

if len(rows) != 129:
    raise RuntimeError(
        f"Expected 129 rows including header for 128-sample window, got {len(rows)}"
    )

header = rows[0]
if header != ["x", "y", "z"]:
    raise RuntimeError(f"Unexpected CSV header: {header}")

print(
    "Smoke check OK | "
    f"noise={len(noise_files)} white={len(white_files)} rows_with_header={len(rows)}"
)
