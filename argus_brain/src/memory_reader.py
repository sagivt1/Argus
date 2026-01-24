"""
Reads and displays data from a shared memory segment created by the C++ engine.
This acts as the "brain" or consumer of the data produced by the "engine".
"""
import mmap
import time
import os

# Configuration for the shared memory segment.
# These values MUST match the configuration in the C++ writer application.
SHM_NAME = "ArgusShm"
SIZE = 1024 * 1024


def main():
    """
    Waits for the shared memory to be created, then enters an infinite
    loop to read the first byte and display it on the console.
    """
    print(f"Argus Brain: Searching for Synapse '{SHM_NAME}'...")

    shm = None
    # Poll until the shared memory segment is available.
    # This allows the reader to start before the writer.
    while not shm:
        try:
            # Attach to the existing shared memory segment by its tagname.
            # On Windows, -1 signifies a non-file-backed mapping.
            # We open in read-only mode to prevent accidental corruption of the data stream.
            shm = mmap.mmap(-1, SIZE, tagname=SHM_NAME, access=mmap.ACCESS_READ)
            print("Synapse Connected! Receiving Data...")
        except FileNotFoundError:
            # The writer application hasn't created the memory yet. Wait and retry.
            time.sleep(0.5)

    # Continuously read and display the data from the shared memory.
    while True:
        # Reset the memory map's pointer to the beginning of the buffer for each read.
        shm.seek(0)
        # Read the first byte, where the writer places the incrementing counter.
        val = shm.read_byte()

        # Print the value to the console, overwriting the same line each time.
        # flush=True is critical to force immediate output, preventing the OS
        # from buffering the text and making the updates appear delayed or frozen.
        print(f"\rIncoming Signal: {val}", end="", flush=True)

        # Pause briefly to target a display rate of roughly 30 FPS.
        time.sleep(0.03)

if __name__ == "__main__":
    main()
