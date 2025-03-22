import os
import usb.core
import usb.util
import usb.backend.libusb1

# Manually add current directory to PATH so Python sees the DLL
os.add_dll_directory(os.getcwd())

# Force PyUSB to load the DLL
backend = usb.backend.libusb1.get_backend(
    find_library=lambda x: "libusb-1.0.dll"
)

if backend is None:
    print("❌ libusb backend not found. Check the path or DLL.")
else:
    dev = usb.core.find(backend=backend)
    if dev is None:
        print("✅ Backend loaded, but no USB device found.")
    else:
        print("✅ USB device found:", hex(dev.idVendor), hex(dev.idProduct))
