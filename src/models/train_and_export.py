"""Train the floor-noise CNN, evaluate, and export TFLite artifacts."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import pandas as pd
import tensorflow as tf
from sklearn.metrics import confusion_matrix, f1_score
from sklearn.model_selection import train_test_split


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset-dir", default="data/processed/dataset")
    parser.add_argument("--epochs", type=int, default=20)
    parser.add_argument("--batch-size", type=int, default=32)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--output-dir", default="experiments/artifacts")
    parser.add_argument("--metrics-path", default="experiments/results/train_metrics.json")
    return parser.parse_args()


def load_dataset(dataset_dir: Path) -> tuple[np.ndarray, np.ndarray]:
    noise_files = sorted((dataset_dir / "noise").glob("*.csv"))
    white_files = sorted((dataset_dir / "white").glob("*.csv"))

    if not noise_files or not white_files:
        raise FileNotFoundError(f"Dataset not found under {dataset_dir}")

    noise = np.asarray([pd.read_csv(p).values for p in noise_files], dtype=np.float32)
    white = np.asarray([pd.read_csv(p).values for p in white_files], dtype=np.float32)

    y_noise = np.asarray([(1, 0)] * len(noise), dtype=np.float32)
    y_white = np.asarray([(0, 1)] * len(white), dtype=np.float32)

    x = np.concatenate([noise, white], axis=0).reshape((-1, 128, 3, 1))
    y = np.concatenate([y_noise, y_white], axis=0)
    return x, y


def build_model() -> tf.keras.Model:
    return tf.keras.Sequential(
        [
            tf.keras.layers.Conv2D(
                16, (3, 3), activation="relu", input_shape=(128, 3, 1), padding="same"
            ),
            tf.keras.layers.MaxPool2D((3, 3), padding="same"),
            tf.keras.layers.Conv2D(8, (2, 2), activation="relu", padding="same"),
            tf.keras.layers.MaxPool2D((2, 2), padding="same"),
            tf.keras.layers.Flatten(),
            tf.keras.layers.Dense(16, activation="relu"),
            tf.keras.layers.Dense(2, activation="softmax"),
        ]
    )


def evaluate_outputs(y_true: np.ndarray, y_pred_prob: np.ndarray) -> dict[str, float]:
    y_true_cls = np.argmax(y_true, axis=1)
    y_pred_cls = np.argmax(y_pred_prob, axis=1)

    acc = float(np.mean(y_true_cls == y_pred_cls))
    pred_pos = int(np.sum(y_pred_cls))
    true_pos = int(np.sum((y_true_cls * y_pred_cls) == 1))
    recall_den = int(np.sum(y_true_cls))

    precision = float(true_pos / pred_pos) if pred_pos else 0.0
    recall = float(true_pos / recall_den) if recall_den else 0.0
    f1 = float(f1_score(y_true_cls, y_pred_cls))

    cm = confusion_matrix(y_true_cls, y_pred_cls).tolist()

    return {
        "accuracy": acc,
        "precision": precision,
        "recall": recall,
        "f1_score": f1,
        "confusion_matrix": cm,
    }


def main() -> None:
    args = parse_args()
    tf.random.set_seed(args.seed)
    np.random.seed(args.seed)

    x, y = load_dataset(Path(args.dataset_dir))
    x_train, x_test, y_train, y_test = train_test_split(
        x, y, test_size=0.1, random_state=args.seed, stratify=np.argmax(y, axis=1)
    )
    x_train, x_val, y_train, y_val = train_test_split(
        x_train,
        y_train,
        test_size=0.2222222222,
        random_state=args.seed,
        stratify=np.argmax(y_train, axis=1),
    )

    model = build_model()
    model.compile(optimizer="adam", loss="binary_crossentropy", metrics=["accuracy"])
    history = model.fit(
        x_train,
        y_train,
        epochs=args.epochs,
        batch_size=args.batch_size,
        validation_data=(x_val, y_val),
        verbose=2,
    )

    y_pred = model.predict(x_test, verbose=0)
    metrics = evaluate_outputs(y_test, y_pred)

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    keras_path = output_dir / "model.keras"
    tflite_path = output_dir / "model.tflite"

    model.save(keras_path)

    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()
    tflite_path.write_bytes(tflite_model)

    interpreter = tf.lite.Interpreter(model_path=str(tflite_path))
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    tflite_correct = 0
    for i in range(len(x_test)):
        inp = np.array([x_test[i]], dtype=np.float32)
        interpreter.set_tensor(input_details[0]["index"], inp)
        interpreter.invoke()
        out = interpreter.get_tensor(output_details[0]["index"])[0]
        if int(np.argmax(out)) == int(np.argmax(y_test[i])):
            tflite_correct += 1

    metrics["tflite_accuracy"] = float(tflite_correct / len(x_test))
    metrics["train_samples"] = int(len(x_train))
    metrics["val_samples"] = int(len(x_val))
    metrics["test_samples"] = int(len(x_test))
    metrics["epochs"] = int(args.epochs)
    metrics["history_final"] = {
        "loss": float(history.history["loss"][-1]),
        "accuracy": float(history.history["accuracy"][-1]),
        "val_loss": float(history.history["val_loss"][-1]),
        "val_accuracy": float(history.history["val_accuracy"][-1]),
    }

    metrics_path = Path(args.metrics_path)
    metrics_path.parent.mkdir(parents=True, exist_ok=True)
    metrics_path.write_text(json.dumps(metrics, indent=2), encoding="utf-8")

    cm_df = pd.DataFrame(
        metrics["confusion_matrix"],
        index=["true_noise", "true_white"],
        columns=["pred_noise", "pred_white"],
    )
    cm_df.to_csv(metrics_path.with_name("confusion_matrix.csv"))

    print(json.dumps(metrics, indent=2))
    print(f"Saved: {keras_path}")
    print(f"Saved: {tflite_path}")
    print(f"Saved: {metrics_path}")


if __name__ == "__main__":
    main()
