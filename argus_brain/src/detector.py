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

        # Warmup: Run dummy inference to initialize CUDA context and prevent lag on first frame
        print("[Brain] Model Loaded. Warming up engine...")
        try:
            self.model(torch.zeros(1, 3, 640, 640).to(self.device), verbose=False)
        except Exception:
            pass  # Non-critical: some CPU backends fail on dummy tensors

    def _get_optimal_device(self):
        """
        Determen the best avialable hardware accelaration.
        """

        if torch.cuda.is_available():
            return "cuda"  # Prioritize NVIDIA CUDA
        elif torch.backends.mps.is_available():
            return "mps"  # Apple Silicon (Metal Performance Shaders)
        else:
            return "cpu"

    def detect(self, source):
        """
        Run inference on a source (image path or numpy array)
        return a list of dicts: {'label': str, 'conf': float, 'box': [x1, y1, x2, y2]}
        """

        # stream=False: Ensure full processing before return (blocking)
        # verbose=False: Suppress internal library logging
        results = self.model(source, device=self.device, verbose=False)

        detections = []
        for result in results:
            for box in result.boxes:
                # Convert GPU tensors to native Python types for JSON serialization
                coords = box.xyxy[0].tolist()
                conf = float(box.conf[0])
                cls_id = int(box.cls[0])
                label = result.names[cls_id]

                detections.append({"label": label, "conf": conf, "box": coords})

        return detections
