import os
import pathlib
import usb.backend.libusb1
from freewili import FreeWili

# Use specific COM port if needed
os.environ["FREEWILI_COM_PORT"] = "COM13"

# Load USB backend
os.add_dll_directory(os.getcwd())
backend = usb.backend.libusb1.get_backend(find_library=lambda x: "libusb-1.0.dll")
if backend is None:
    print("❌ libusb not found.")
    exit(1)

# Locate WASM file
project_root = pathlib.Path(__file__).parent.resolve()
wasm_file = project_root / "button_light1.wasm"  # ✅ CHANGED HERE

# Connect to FreeWili
print("🔎 Connecting to FreeWili...")
device_result = FreeWili.find_first()
if device_result.is_err():
    print(f"❌ Could not connect:\n{device_result.unwrap_err()}")
    exit(1)

device = device_result.unwrap()
print(f"✅ Connected to FreeWili: {device}")

# Upload + run script
print(f"📤 Uploading: {wasm_file.name}")
device.send_file(wasm_file, None, None).expect("Failed to upload")
print(f"▶️ Running: {wasm_file.name}")
device.run_script(wasm_file.name).expect("Failed to run")

print("🚀 Done! FreeWili is now running button_light.wasm.")
