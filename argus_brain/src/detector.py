import torch
from ultralytics import YOLO


class ObjectDetector:
    """
    Encapsulates the YOLO object detection model, handling device selection,
    model loading, and inference result processing.
    """
    def __init__(self, model_path="yolov8n.pt"):
        """
        Initializes the YOLO model and auto-selects hardware acceleration.
        """
        self.device = self._get_optimal_device()
        print(f"[Brain] Loading Model on Device: {self.device}")

        # Loads the pre-trained YOLO model weights from the specified path.
        self.model = YOLO(model_path)

        # Warmup: Run a dummy inference to initialize the CUDA context.
        # This prevents a noticeable lag on the first real inference call.
        print("[Brain] Model Loaded. Warming up engine...")
        try:
            self.model(torch.zeros(1, 3, 640, 640).to(self.device), verbose=False)
        except Exception:
            # This is non-critical as some CPU-only 
            # backends might fail on dummy tensors.
            pass

    def _get_optimal_device(self):
        """
        Determines the best available hardware acceleration device.
        """
        if torch.cuda.is_available():
            return "cuda"  # Prioritize NVIDIA CUDA for best performance.
        elif torch.backends.mps.is_available():
            return "mps"   # Fallback for Apple Silicon (Metal Performance Shaders).
        else:
            return "cpu"   # Default to CPU if no specialized hardware is found.

    def detect(self, source):
        """
        Runs inference on a source (e.g., image path, URL, or numpy array).

        Returns:
            A list of dicts, where each dict represents a detected object:
            {'label': str, 'conf': float, 'box': [x1, y1, x2, y2]}
        """
        # stream=False: Ensures all results are processed before returning 
        # (blocking call).
        # verbose=False: Suppresses the library's default logging output.
        results = self.model(source, device=self.device, verbose=False)

        detections = []
        for result in results:
            for box in result.boxes:
                # Convert tensors from the GPU to native Python types.
                # This is crucial for subsequent operations like JSON serialization.
                coords = box.xyxy[0].tolist()
                conf = float(box.conf[0])
                cls_id = int(box.cls[0])
                label = result.names[cls_id]

                detections.append({"label": label, "conf": conf, "box": coords})

        return detections
