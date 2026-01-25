import json
import sys

from detector import ObjectDetector


def main():
    """
    Initializes the ObjectDetector, runs inference on a sample image,
    and prints the structured results to standard output.
    """
    print("Argus Brain Initializing....")

    try:
        brain = ObjectDetector()
    except Exception as e:
        # Fail fast if model weights are missing or the environment is misconfigured.
        print(f"CRITICAL: Failed to load AI Model: {e}")
        sys.exit(1)

    # Default to a remote sample for zero-setup verification.
    # This can be overridden by providing a local file path or 
    # URL as a command-line argument.
    test_image = "https://ultralytics.com/images/zidane.jpg"
    if len(sys.argv) > 1:
        test_image = sys.argv[1]

    print(f"Argus Brain: Analyzing {test_image}...")

    # Execute the core object detection inference.
    results = brain.detect(test_image)

    # Serialize results to JSON for machine-readable, structured output.
    print(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
