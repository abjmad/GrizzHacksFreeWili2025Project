import os
import time
from freewili import FreeWiliDevice

# Replace with your FreeWili device's information
device = FreeWiliDevice.connect()


def delete_all_scripts():
    try:
        # Get a list of all files on the FreeWili device
        files = device.list_files()
        print("Files on FreeWili device:", files)

        # Loop through the files and delete them
        for file in files:
            if file.endswith('.wasm'):  # Assuming scripts have the '.wasm' extension
                print(f"Deleting: {file}")
                device.delete_file(file)
                time.sleep(1)  # Give some time between each delete operation

        print("All scripts deleted successfully!")
    except Exception as e:
        print(f"Error deleting files: {e}")


if __name__ == "__main__":
    delete_all_scripts()
