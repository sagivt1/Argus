import torch
from ultralytics import YOLO


class ObjectDetector:
    def __init__(self, model_path="yolov8n.pt"):
        """
        Init the YOLO model and auto-select hardware accelaration.
        """
        self.device = self._get_optimal_device()
        print(f"[Brain] Loading Model on Device: {self.device}")

        self.model = YOLO(model_path)

        # Warmup: Run a dummy inference so the first real frame isn't slow
        print("[Brain] Model Loaded. Warming up engine...")
        try:
            self.model(torch.zeros(1, 3, 640, 640).to(self.device), verbose=False)
        except Exception:
            pass  # ignore warmup errors on some cpu

    def _get_optimal_device(self):
        """
        Determen the best avialable hardware accelaration.
        """

        if torch.cuda.is_available():
            return "cuda"  # nvidia gpi
        elif torch.backends.mps.is_available():
            return "mps"  # Apple Silicon
        else:
            return "cpu"  # Universal Fallback

    def detect(self, source):
        """
        Run inference on a source (image path or numpy array)
        return a list of dicts: {'label': str, 'conf': float, 'box': [x1, y1, x2, y2]}
        """

        # Run inference
        # stream=False: Process completely before returning
        # verbose: keep console clean
        results = self.model(source, device=self.device, verbose=False)

        detections = []
        for result in results:
            for box in result.boxes:
                # Extract the data
                coords = box.xyxy[0].tolist()
                conf = float(box.conf[0])
                cls_id = int(box.cls[0])
                label = result.names[cls_id]

                detections.append({"label": label, "conf": conf, "box": coords})

        return detections
