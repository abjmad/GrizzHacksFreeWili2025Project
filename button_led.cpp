#include <fwwasm.h>

void clearLEDs() {
    for (int i = 0; i < 7; i++) {
        setBoardLED(i, 0, 0, 0, 100, LEDManagerLEDMode::ledsimplevalue);
    }
}

auto main() -> int {
    clearLEDs();

    while (true) {
        if (hasEvent()) {
            unsigned char data[FW_GET_EVENT_DATA_MAX];
            int eventType = getEventData(data);

            clearLEDs();

            if (eventType == FWGUI_EVENT_GREEN_BUTTON) {
                setBoardLED(0, 0, 255, 0, 250, LEDManagerLEDMode::ledpulsefade);  // Green light on LED 0
            } else if (eventType == FWGUI_EVENT_BLUE_BUTTON) {
                setBoardLED(1, 0, 0, 255, 250, LEDManagerLEDMode::ledpulsefade);  // Blue light on LED 1
            }

            waitms(100);  // debounce / prevent flicker
        }
    }

    return 0;
}
