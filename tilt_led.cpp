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
        1, 0,             // Stream accel, not temp
        100,              // 100 ms rate
        1, 0,             // Enable accel X, plot index 0 (not used here)
        1, 1,             // Enable accel Y, plot index 1
        1, 2,             // Enable accel Z, plot index 2
        0, 0,             // Temp plot disabled
        0, 0              // Temp F disabled
    );

    clearLEDs();  // Turn off all LEDs

    while (true) {
        if (hasEvent()) {
            unsigned char data[FW_GET_EVENT_DATA_MAX];
            int eventType = getEventData(data);

            if (eventType == FWGUI_EVENT_GUI_SENSOR_DATA) {
                // X and Y axis data (signed 16-bit from byte array)
                short accelX = (data[4] << 8) | data[3];
                short accelY = (data[6] << 8) | data[5];

                clearLEDs();  // Reset LEDs before updating

                // Forward/backward
                if (accelY < -TILT_THRESHOLD) {
                    setBoardLED(LED_FORWARD, 255, 0, 0, 250, LEDManagerLEDMode::ledpulsefade);  // Red
                } else if (accelY > TILT_THRESHOLD) {
                    setBoardLED(LED_BACKWARD, 255, 255, 0, 250, LEDManagerLEDMode::ledpulsefade);  // Yellow
                }

                // Left/right
                if (accelX < -TILT_THRESHOLD) {
                    setBoardLED(LED_LEFT, 0, 0, 255, 250, LEDManagerLEDMode::ledpulsefade);  // Blue
                } else if (accelX > TILT_THRESHOLD) {
                    setBoardLED(LED_RIGHT, 0, 255, 0, 250, LEDManagerLEDMode::ledpulsefade);  // Green
                }

                waitms(100);  // Prevent spamming updates
            }
        }
    }

    return 0;
}
