import os
import pathlib
import usb.backend.libusb1
from freewili import FreeWili

# Load libusb
os.add_dll_directory(os.getcwd())
backend = usb.backend.libusb1.get_backend(find_library=lambda x: "libusb-1.0.dll")

if backend is None:
    print("❌ libusb backend not found.")
    exit(1)

# Path setup
project_root = pathlib.Path(__file__).parent.resolve()
images_dir = project_root / "images"

# Connect to FreeWili
print("🔎 Scanning for FreeWili...")
device_result = FreeWili.find_first()
if device_result.is_err():
    print(f"❌ Connection error: {device_result.unwrap_err()}")
    exit(1)

device = device_result.unwrap()
print(f"✅ Connected to FreeWili: {device}")

# Upload images
for image_path in images_dir.glob("*.jpg"):
    print(f"📤 Uploading: {image_path.name}")
    device.send_file(image_path, "/images", None).expect(f"Failed to upload {image_path.name}")

print("🚀 All images uploaded to FreeWili's /images folder!")
