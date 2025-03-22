import os
import usb.backend.libusb1

# Inject libusb backend early (before freewili tries to use it)
os.add_dll_directory(os.getcwd())

backend = usb.backend.libusb1.get_backend(
    find_library=lambda x: "libusb-1.0.dll"
)

if backend is None:
    print("❌ libusb backend still not found.")
else:
    print("✅ libusb backend loaded.")

    # Now import and use FreeWili
    import result
    from freewili import FreeWili

    # Try to find the first FreeWili device
    try:
        device = FreeWili.find_first().expect("Failed to find a FreeWili")
    except result.UnwrapError as ex:
        print(f"❌ {ex}")

    # Find all connected FreeWili devices
    devices = FreeWili.find_all()
    print(f"✅ Found {len(devices)} FreeWili(s)")

    for i, free_wili in enumerate(devices, start=1):
        print(f"{i}. {free_wili}")
        print(f"\tMain: {free_wili.main}")
        print(f"\tDisplay: {free_wili.display}")
        print(f"\tFTDI: {free_wili.ftdi}")
