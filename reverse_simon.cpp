#include "fwwasm.h"

// Button Event IDs
#define EVENT_RED FWGUI_EVENT_RED_BUTTON
#define EVENT_YEL FWGUI_EVENT_YELLOW_BUTTON
#define EVENT_GRN FWGUI_EVENT_GREEN_BUTTON
#define EVENT_BLU FWGUI_EVENT_BLUE_BUTTON
#define EVENT_GRA FWGUI_EVENT_GRAY_BUTTON

// LED indices
#define LED_RED 5
#define LED_YEL 2
#define LED_GRN 3
#define LED_BLU 4

// Color values
#define COLOR_RED 255, 0, 0
#define COLOR_YEL 255, 255, 0
#define COLOR_GRN 0, 255, 0
#define COLOR_BLU 0, 0, 255

// Delay and game settings
#define WAIT_TIME 100

void clearLEDs() {
    for (int i = 0; i < 7; i++) {
        setBoardLED(i, 0, 0, 0, 100, LEDManagerLEDMode::ledsimplevalue);
    }
}

int getPressedButton() {
    unsigned char data[FW_GET_EVENT_DATA_MAX] = {};
    int evt = getEventData(data);
    if (evt == EVENT_RED) return 0;
    if (evt == EVENT_YEL) return 1;
    if (evt == EVENT_GRN) return 2;
    if (evt == EVENT_BLU) return 3;
    if (evt == EVENT_GRA) return 4;
    return -1;
}

int getRandomUnlit(int excluded[3]) {
    while (1) {
        int guess = (wilirand() + millis()) % 4;
        bool found = false;
        for (int i = 0; i < 3; i++) {
            if (excluded[i] == guess) {
                found = true;
                break;
            }
        }
        if (!found) return guess;
    }
}

int main() {
    while (1) {
        clearLEDs();

        int lit[3];
        lit[0] = (wilirand() + millis()) % 4;
        lit[1] = (lit[0] + 1) % 4;
        lit[2] = (lit[0] + 2) % 4;

        for (int i = 0; i < 3; i++) {
            int led = lit[i];
            switch (led) {
                case 0: setBoardLED(LED_RED, COLOR_RED, 300, LEDManagerLEDMode::ledpulsefade); break;
                case 1: setBoardLED(LED_YEL, COLOR_YEL, 300, LEDManagerLEDMode::ledpulsefade); break;
                case 2: setBoardLED(LED_GRN, COLOR_GRN, 300, LEDManagerLEDMode::ledpulsefade); break;
                case 3: setBoardLED(LED_BLU, COLOR_BLU, 300, LEDManagerLEDMode::ledpulsefade); break;
            }
        }

        int correct = getRandomUnlit(lit);

        int guess = -1;
        unsigned int start = millis();
        while (millis() - start < 5000) {
            if (hasEvent()) {
                guess = getPressedButton();
                break;
            }
            waitms(50);
        }

        clearLEDs();
        if (guess == correct) {
            setBoardLED(LED_GRN, 0, 255, 0, 200, LEDManagerLEDMode::ledflashfade);
        } else {
            setBoardLED(LED_RED, 255, 0, 0, 300, LEDManagerLEDMode::ledflashfade);
        }

        waitms(1000);
    }

    return 0;
}
