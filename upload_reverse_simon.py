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

# === Paths ===
project_root = pathlib.Path(__file__).parent.resolve()
wasm_file = project_root / "reverse_simon.wasm"

# === Check that file exists ===
if not wasm_file.exists():
    print(f"❌ Missing: {wasm_file.name}")
    exit(1)

# === Connect to FreeWili ===
print("🔎 Scanning for FreeWili...")
device_result = FreeWili.find_first()

if device_result.is_err():
    print(f"❌ Could not connect:\n{device_result.unwrap_err()}")
    exit(1)

device = device_result.unwrap()
print(f"✅ Connected to FreeWili: {device}")

# === Upload & run ===
print(f"📤 Uploading: {wasm_file.name}")
device.send_file(wasm_file, None, None).expect(f"Failed to upload {wasm_file.name}")

print(f"▶️ Running: {wasm_file.name}")
device.run_script(wasm_file.name).expect(f"Failed to run {wasm_file.name}")

print("🚀 Upload complete! Your Reverse Simon game is running.")
