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

# === Resolve paths ===
project_root = pathlib.Path(__file__).parent.resolve()
wasm_file = project_root / "leds.wasm"  # <-- use "led_blink.wasm" if needed
fwi_file = project_root / "pip_boy.fwi"
radio_dir = project_root / "radio"
sub_files = ["red", "blue", "green", "yellow", "white", "off"]

# === Check for main .wasm file ===
if not wasm_file.exists():
    print(f"❌ Missing: {wasm_file.name}")
    exit(1)

# === Connect to FreeWili safely ===
print("🔎 Scanning for FreeWili...")
device_result = FreeWili.find_first()

if device_result.is_err():
    print(f"❌ Could not connect to FreeWili:\n{device_result.unwrap_err()}")
    exit(1)

device = device_result.unwrap()
print(f"✅ Connected to FreeWili: {device}")

# === Upload optional FWI file ===
if fwi_file.exists():
    print(f"📤 Uploading: {fwi_file.name}")
    device.send_file(fwi_file, None, None).expect("Failed to upload FWI")

# === Upload radio sub files ===
for name in sub_files:
    sub_path = radio_dir / f"{name}.sub"
    if sub_path.exists():
        print(f"📤 Uploading: {sub_path.name}")
        device.send_file(sub_path, None, None).expect(f"Failed to upload {name}.sub")
    else:
        print(f"⚠️ Missing sub: {sub_path.name}")

# === Upload and run the .wasm file ===
print(f"📤 Uploading: {wasm_file.name}")
device.send_file(wasm_file, None, None).expect(f"Failed to upload {wasm_file.name}")

print(f"▶️ Running: {wasm_file.name}")
device.run_script(wasm_file.name).expect(f"Failed to run {wasm_file.name}")

print("🚀 Upload complete! Your FreeWili is running the script!")
