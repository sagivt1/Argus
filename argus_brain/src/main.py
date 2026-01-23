import sys
import json
from detector import ObjectDetector

def main():
    print("Argus Brain Initializing....")
   
    try:
        brain = ObjectDetector()
    except Exception as e:
        print(f"CRITICAL: Faild to load AI Model")
        sys.exit(1)

    # test input
    # use standart testing image if no file found
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
