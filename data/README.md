# Data Layout

- `raw/sample/`: sample-stage captures and collection-environment media
- `raw/real/`: real environment raw recordings (`origin_data`, `new_origin_data`, `white_data.txt`)
- `processed/dataset/`: model-ready windowed CSV data (`noise/`, `white/`)

The processed dataset is currently committed and used by default in training scripts.
