import json
import sys

from detector import ObjectDetector


def main():
    print("Argus Brain Initializing....")
   
    try:
        brain = ObjectDetector()
    except Exception:
        # Fail fast if model weights are missing or incompatible
        print("CRITICAL: Faild to load AI Model {e}")
        sys.exit(1)

    # Default to remote sample for zero-setup verification
    test_image = "https://ultralytics.com/images/zidane.jpg"
    if len(sys.argv) > 1:
        test_image = sys.argv[1]
    
    print(f"Argus Brain: Analyzing {test_image}...")

    # run interface
    results = brain.detect(test_image)

    # output result 
    print(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
