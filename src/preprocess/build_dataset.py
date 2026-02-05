"""Build noise/white window datasets from raw TXT captures.

This script recreates the preprocessing logic used in the paper workflow:
- Load raw `x,y,z` sensor text files
- Detect windows with floor-vibration events from Z-axis deltas
- Save fixed-size windows as CSV files for model training
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import pandas as pd


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--origin-dir",
        default="data/raw/real/new_origin_data",
        help="Directory containing raw .txt vibration recordings",
    )
    parser.add_argument(
        "--white-file",
        default="data/raw/real/white_data.txt",
        help="Path to white-noise baseline txt file",
    )
    parser.add_argument(
        "--output-dir",
        default="data/processed/dataset",
        help="Output directory containing noise/ and white/ CSV files",
    )
    parser.add_argument("--window-size", type=int, default=128)
    parser.add_argument(
        "--threshold",
        type=float,
        default=None,
        help="Optional fixed threshold for Z-axis delta. Defaults to per-file mean.",
    )
    parser.add_argument("--seed", type=int, default=42)
    return parser.parse_args()


def iter_txt_files(path: Path) -> list[Path]:
    return sorted([p for p in path.rglob("*.txt") if p.is_file()])


def find_noise_windows(
    file_path: Path,
    window_size: int,
    threshold: float | None,
) -> np.ndarray:
    df = pd.read_csv(file_path, header=None, names=["x", "y", "z"])
    delta_df = df.diff().iloc[1:].abs()

    z_threshold = float(delta_df["z"].mean()) if threshold is None else threshold
    windows = []
    for start in range(0, len(delta_df) - window_size + 1, window_size):
        z_window = delta_df.iloc[start : start + window_size, 2]
        if (z_window > z_threshold).any():
            windows.append(df.iloc[start : start + window_size].values)

    if not windows:
        return np.empty((0, window_size, 3), dtype=np.float32)
    return np.asarray(windows)


def save_windows(noise_windows: np.ndarray, white_df: pd.DataFrame, out_dir: Path, seed: int) -> None:
    noise_dir = out_dir / "noise"
    white_dir = out_dir / "white"
    noise_dir.mkdir(parents=True, exist_ok=True)
    white_dir.mkdir(parents=True, exist_ok=True)

    for p in noise_dir.glob("*.csv"):
        p.unlink()
    for p in white_dir.glob("*.csv"):
        p.unlink()

    for i, window in enumerate(noise_windows):
        pd.DataFrame(window, columns=["x", "y", "z"]).to_csv(
            noise_dir / f"noise_{i}.csv", index=False
        )

    rng = np.random.default_rng(seed)
    white_values = white_df[["x", "y", "z"]].to_numpy()
    window_size = noise_windows.shape[1]
    replace = len(white_values) < window_size

    for i in range(len(noise_windows)):
        idx = rng.choice(len(white_values), size=window_size, replace=replace)
        sampled = pd.DataFrame(white_values[idx], columns=["x", "y", "z"])
        sampled.to_csv(white_dir / f"white_{i}.csv", index=False)


def main() -> None:
    args = parse_args()
    origin_dir = Path(args.origin_dir)
    white_file = Path(args.white_file)
    out_dir = Path(args.output_dir)

    files = iter_txt_files(origin_dir)
    if not files:
        raise FileNotFoundError(f"No .txt files found under {origin_dir}")

    all_noise = []
    for f in files:
        windows = find_noise_windows(f, args.window_size, args.threshold)
        if len(windows) > 0:
            all_noise.append(windows)

    if not all_noise:
        raise RuntimeError("No noise windows detected. Try lowering --threshold.")

    noise_windows = np.concatenate(all_noise, axis=0)
    white_df = pd.read_csv(white_file, header=None, names=["x", "y", "z"])

    save_windows(noise_windows, white_df, out_dir, seed=args.seed)

    metadata = {
        "origin_dir": str(origin_dir),
        "white_file": str(white_file),
        "window_size": args.window_size,
        "threshold": args.threshold,
        "noise_window_count": int(len(noise_windows)),
        "white_window_count": int(len(noise_windows)),
        "source_files": [str(f) for f in files],
    }
    (out_dir / "metadata.json").write_text(json.dumps(metadata, indent=2), encoding="utf-8")
    print(json.dumps(metadata, indent=2))


if __name__ == "__main__":
    main()
