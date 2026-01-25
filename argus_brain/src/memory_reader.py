"""
Reads and displays data from a shared memory segment created by the C++ engine.
This acts as the "brain" or consumer of the data produced by the "engine".
"""
import mmap
import struct
import time

import numpy as np

# Configuration for the shared memory segment.
# These values MUST match the configuration in the C++ writer application.
SHM_NAME = "ArgusShm"

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
            # We open in read-only mode to prevent accidental 
            # corruption of the data stream.
            shm = mmap.mmap(-1, 1024, tagname=SHM_NAME, access=mmap.ACCESS_READ)
            print("Synapse Connected! Receiving Data...")
        except FileNotFoundError:
            # The writer application hasn't created the memory yet. Wait and retry.
            time.sleep(0.5)

    # Read the 16-byte header, formatted as four unsigned ints ('IIII').
    header_size = 16
    shm.seek(0)
    header_data = shm.read(header_size)
    frame_id, width, height, channels = struct.unpack('IIII', header_data)

    print(f"Stream Config: {width}x{height} ({channels} ch)")

    # Now that the full size is known from the header, re-map the memory segment.
    total_size = header_size + (width * height * channels)
    shm.close()

    # retry loop for remapping (sometimes Windows takes a ms to release the handle)
    while True:
       try:
            shm = mmap.mmap(-1, total_size, tagname=SHM_NAME, access=mmap.ACCESS_READ)
            break
       except Exception as _:
           time.sleep(0.1)

    last_frame_id = -1

    # Create a numpy array that points directly to shared memory
    # offset=16 skip the header
    try:
        shared_array = np.ndarray(
            shape=(height, width, channels), 
            dtype=np.uint8, 
            buffer=shm, 
            offset=header_size
        )
    except TypeError:
        print("Error: Could not create numpy buffer. Checking alignment...")
        return
    
    while True:
        # Check the header (Frame ID is at byte 0)
        # We read just the first 4 bytes for speed
        shm.seek(0)
        # Read current frame ID
        current_frame_id = struct.unpack('I', shm.read(4))[0]

        if current_frame_id != last_frame_id:
            # The data in 'shared_array' updates automatically because it points to RAM!
            # We just need to analyze it.
            
            # Simple Analysis: Average Color
            # (We use .copy() here only if we need to persist the data, 
            # for analysis we can read shared_array directly)
            avg_color = shared_array.mean(axis=(0, 1))
            print(f"\rFrame {current_frame_id}: Avg Color RGB{avg_color.astype(int)}", end="", flush=True)
            last_frame_id = current_frame_id

        # Yield execution to prevent 100% CPU usage in polling loop.
        time.sleep(0.001)

if __name__ == "__main__":
    main()