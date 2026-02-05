#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo "Usage: $0 <input.tflite> <output.cc> [symbol_name]"
  echo "Example: $0 experiments/artifacts/model.tflite embedded/floor_noise_v5/floor_noise_model_data.cpp model_tflite"
  exit 1
fi

input_tflite="$1"
output_cc="$2"
symbol_name="${3:-model_tflite}"

if ! command -v xxd >/dev/null 2>&1; then
  echo "Error: xxd not found. Install vim-common (Linux) or Xcode CLT/macOS tools."
  exit 1
fi

xxd -i "$input_tflite" \
  | sed -E "s/^unsigned char [a-zA-Z0-9_]+\[\]/unsigned char ${symbol_name}[]/" \
  | sed -E "s/^unsigned int [a-zA-Z0-9_]+_len/unsigned int ${symbol_name}_len/" \
  > "$output_cc"

echo "Wrote $output_cc"
