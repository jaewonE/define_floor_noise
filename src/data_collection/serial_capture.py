"""Capture Arduino serial sensor values into a text file.

Expected serial format: `x,y,z` per line at ~10 ms intervals.
"""

from __future__ import annotations

import argparse
from pathlib import Path

import serial


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--port",
        default="/dev/cu.usbmodem1201",
        help="Serial device path (example: /dev/cu.usbmodem1201)",
    )
    parser.add_argument("--baud-rate", type=int, default=9600)
    parser.add_argument(
        "--output",
        default="data/raw/sample/live_capture.txt",
        help="Output text file path",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    with serial.Serial(args.port, args.baud_rate) as ser, output_path.open(
        "w", encoding="utf-8"
    ) as out_file:
        print(f"Recording data from {args.port}. Press Ctrl+C to stop...")
        try:
            while True:
                line = ser.readline().decode("ascii", errors="ignore").strip()
                if line:
                    out_file.write(line + "\n")
        except KeyboardInterrupt:
            print(f"Data recording stopped. Saved to {output_path}")


if __name__ == "__main__":
    main()
