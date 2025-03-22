"""Uploads all required files to FreeWili and runs radio.wasm."""

import os
import pathlib
import usb.backend.libusb1
from freewili import FreeWili

# === Load USB backend ===
os.add_dll_directory(os.getcwd())
backend = usb.backend.libusb1.get_backend(
    find_library=lambda x: "libusb-1.0.dll"
)

if backend is None:
    print("❌ libusb backend not found. Make sure libusb-1.0.dll is in this folder.")
    exit(1)

# === Resolve file paths ===
project_root = pathlib.Path(__file__).parent.resolve()
wasm_file = project_root / "radio.wasm"
fwi_file = project_root / "pip_boy.fwi"
radio_dir = project_root / "radio"
sub_files = ["red", "blue", "green", "yellow", "white", "off"]

# === Check critical files ===
if not wasm_file.exists():
    print(f"❌ Missing: {wasm_file}")
    exit(1)

# === Connect to FreeWili ===
device = FreeWili.find_first().unwrap()
print(f"✅ Connected to FreeWili: {device}")

# === Upload pip_boy.fwi (if exists) ===
if fwi_file.exists():
    print(f"📤 Uploading: {fwi_file.name}")
    device.send_file(fwi_file, None, None).expect("Failed to upload FWI")

# === Upload .sub files ===
for name in sub_files:
    sub_path = radio_dir / f"{name}.sub"
    if sub_path.exists():
        print(f"📤 Uploading: {sub_path.name}")
        device.send_file(sub_path, None, None).expect(f"Failed to upload {name}.sub")
    else:
        print(f"⚠️ Missing: {sub_path}")

# === Upload and Run the .wasm file ===
print(f"📤 Uploading: {wasm_file.name}")
device.send_file(wasm_file, None, None).expect("Failed to upload radio.wasm")

print(f"▶️ Running: {wasm_file.name}")
device.run_script(wasm_file.name).expect("Failed to run radio.wasm")

print("🚀 All files uploaded and radio.wasm is running!")
