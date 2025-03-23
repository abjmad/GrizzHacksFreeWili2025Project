#include <fwwasm.h>

// Thresholds to detect tilt (in accelerometer units)
#define TILT_THRESHOLD 300

// LED indexes for directions
#define LED_FORWARD 0
#define LED_BACKWARD 1
#define LED_LEFT 2
#define LED_RIGHT 3

// Utility to turn all LEDs off
void clearLEDs() {
    for (int i = 0; i < 7; ++i) {
        setBoardLED(i, 0, 0, 0, 100, LEDManagerLEDMode::ledsimplevalue);
    }
}

auto main() -> int {
    // Enable accelerometer streaming every 100ms
    setSensorSettings(
        1, 0,
        100,
        1, 0,
        1, 1,
        1, 2,
        0, 0,
        0, 0
    );



    // Turn off all LEDs to start
    clearLEDs();

    while (true) {
        if (hasEvent()) {
            unsigned char data[FW_GET_EVENT_DATA_MAX];
            int eventType = getEventData(data);

            // 🔴 Exit script if red button is pressed
            if (eventType == FWGUI_EVENT_RED_BUTTON) {
                clearLEDs();
                exitToMainAppMenu();
            }

            // 📡 Handle accelerometer tilt
            if (eventType == FWGUI_EVENT_GUI_SENSOR_DATA) {
                short accelX = (data[4] << 8) | data[3];
                short accelY = (data[6] << 8) | data[5];

                clearLEDs();

                if (accelY < -TILT_THRESHOLD) {
                    setBoardLED(LED_FORWARD, 255, 0, 0, 250, LEDManagerLEDMode::ledpulsefade);
                } else if (accelY > TILT_THRESHOLD) {
                    setBoardLED(LED_BACKWARD, 255, 255, 0, 250, LEDManagerLEDMode::ledpulsefade);
                }

                if (accelX < -TILT_THRESHOLD) {
                    setBoardLED(LED_LEFT, 0, 0, 255, 250, LEDManagerLEDMode::ledpulsefade);
                } else if (accelX > TILT_THRESHOLD) {
                    setBoardLED(LED_RIGHT, 0, 255, 0, 250, LEDManagerLEDMode::ledpulsefade);
                }

                waitms(100);
            }
        }
    }

    return 0;
}
