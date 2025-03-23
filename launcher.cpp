#include <fwwasm.h>

auto main() -> int {
    // Tell the user what to do
    showDialogMsgBox("🟢 Green = Start Tilt LEDs\n🔴 Red = Exit", 0, 0, 0, -1, 0);

    while (true) {
        if (hasEvent()) {
            unsigned char data[FW_GET_EVENT_DATA_MAX];
            int eventType = getEventData(data);

            if (eventType == FWGUI_EVENT_GREEN_BUTTON) {
                runZoomIOScript("/scripts/tilt_led.wasm");  // 🟢 Launch the tilt script
            }

            if (eventType == FWGUI_EVENT_RED_BUTTON) {
                exitToMainAppMenu();  // 🔴 Exit
            }
        }

        waitms(100);  // Slight pause to reduce CPU usage
    }

    return 0;
}
