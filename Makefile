.PHONY: smoke preprocess train tflite2cc

smoke:
	python src/utils/smoke_check.py

preprocess:
	python src/preprocess/build_dataset.py

train:
	python src/models/train_and_export.py

tflite2cc:
	./tools/tflite_to_cc.sh experiments/artifacts/model.tflite embedded/floor_noise_v5/floor_noise_model_data.cpp model_tflite
