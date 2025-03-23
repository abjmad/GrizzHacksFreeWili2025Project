import os
import pathlib
import usb.backend.libusb1
from freewili import FreeWili

# === Load libusb backend ===
os.add_dll_directory(os.getcwd())
backend = usb.backend.libusb1.get_backend(
    find_library=lambda x: "libusb-1.0.dll"
)

if backend is None:
    print("❌ libusb backend not found. Make sure libusb-1.0.dll is in this folder.")
    exit(1)

# === === Change this to choose the file === ===
target_wasm = "menu.wasm"  # <-- CHANGE THIS to e.g. "tilt_led.wasm" or "battleship.wasm"

# === Resolve paths ===
project_root = pathlib.Path(__file__).parent.resolve()
wasm_path = project_root / target_wasm

if not wasm_path.exists():
    print(f"❌ Missing: {target_wasm}")
    exit(1)

# === Connect to FreeWili safely ===
print("🔎 Scanning for FreeWili...")
device_result = FreeWili.find_first()

if device_result.is_err():
    print(f"❌ Could not connect to FreeWili:\n{device_result.unwrap_err()}")
    exit(1)

device = device_result.unwrap()
print(f"✅ Connected to FreeWili: {device}")

# === Upload selected .wasm file ===
print(f"📤 Uploading: {target_wasm}")
device.send_file(wasm_path, None, None).expect(f"Failed to upload {target_wasm}")

# === Run it ===
print(f"▶️ Running: {target_wasm}")
device.run_script(target_wasm).expect(f"Failed to run {target_wasm}")

print("🚀 Upload complete!")
