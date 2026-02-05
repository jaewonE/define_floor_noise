"""Preset wrapper for real-environment capture settings."""

from __future__ import annotations

import subprocess
import sys


def main() -> int:
    cmd = [
        sys.executable,
        "src/data_collection/serial_capture.py",
        "--port",
        "/dev/cu.usbmodem1101",
        "--output",
        "data/raw/real/live_capture.txt",
    ]
    return subprocess.call(cmd)


if __name__ == "__main__":
    raise SystemExit(main())
